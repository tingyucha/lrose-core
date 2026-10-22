#include <Radx/ZarrRadxFile.hh>
#include <Radx/RadxVol.hh>
#include <Radx/RadxRay.hh>
#include <Radx/RadxField.hh>
#include <Radx/RadxTime.hh>
#include <Radx/RadxGeoref.hh>

#include <zstd.h>
#include <zlib.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <sys/stat.h>
#include <vector>

using std::string;
using std::vector;

namespace {

const size_t kMaxBytes = 512UL * 1024UL * 1024UL;

struct Json {
  enum Type { NUL, BOOL, NUMBER, STRING, ARRAY, OBJECT } type;
  bool b;
  double num;
  string str;
  vector<Json> arr;
  vector<std::pair<string, Json> > obj;

  Json() : type(NUL), b(false), num(0) {}

  const Json *find(const string &key) const {
    for (size_t ii = 0; ii < obj.size(); ii++) {
      if (obj[ii].first == key) {
        return &obj[ii].second;
      }
    }
    return NULL;
  }
};

void appendCodePoint(string &out, int cp);

class JsonParser {
public:
  explicit JsonParser(const string &text) : _s(text), _i(0) {}

  bool parse(Json &out, string &err) {
    skip();
    if (!value(out, err)) {
      return false;
    }
    skip();
    return true;
  }

private:
  const string &_s;
  size_t _i;

  void skip() {
    while (_i < _s.size() && std::isspace(static_cast<unsigned char>(_s[_i]))) {
      _i++;
    }
  }

  bool value(Json &out, string &err) {
    skip();
    if (_i >= _s.size()) {
      err = "unexpected end of JSON";
      return false;
    }
    char c = _s[_i];
    if (c == '{') return object(out, err);
    if (c == '[') return array(out, err);
    if (c == '"') return stringVal(out, err);
    if (c == 't' || c == 'f') return boolVal(out, err);
    if (c == 'n') return nullVal(out, err);
    return numberVal(out, err);
  }

  bool object(Json &out, string &err) {
    out = Json();
    out.type = Json::OBJECT;
    _i++;
    skip();
    if (_i < _s.size() && _s[_i] == '}') {
      _i++;
      return true;
    }
    while (_i < _s.size()) {
      Json key;
      if (!stringVal(key, err)) return false;
      skip();
      if (_i >= _s.size() || _s[_i] != ':') {
        err = "JSON object missing ':'";
        return false;
      }
      _i++;
      Json val;
      if (!value(val, err)) return false;
      out.obj.push_back(std::make_pair(key.str, val));
      skip();
      if (_i < _s.size() && _s[_i] == ',') {
        _i++;
        skip();
        continue;
      }
      if (_i < _s.size() && _s[_i] == '}') {
        _i++;
        return true;
      }
      err = "JSON object not closed";
      return false;
    }
    err = "JSON object not closed";
    return false;
  }

  bool array(Json &out, string &err) {
    out = Json();
    out.type = Json::ARRAY;
    _i++;
    skip();
    if (_i < _s.size() && _s[_i] == ']') {
      _i++;
      return true;
    }
    while (_i < _s.size()) {
      Json val;
      if (!value(val, err)) return false;
      out.arr.push_back(val);
      skip();
      if (_i < _s.size() && _s[_i] == ',') {
        _i++;
        skip();
        continue;
      }
      if (_i < _s.size() && _s[_i] == ']') {
        _i++;
        return true;
      }
      err = "JSON array not closed";
      return false;
    }
    err = "JSON array not closed";
    return false;
  }

  bool stringVal(Json &out, string &err) {
    if (_i >= _s.size() || _s[_i] != '"') {
      err = "JSON string expected";
      return false;
    }
    _i++;
    out = Json();
    out.type = Json::STRING;
    while (_i < _s.size()) {
      char c = _s[_i++];
      if (c == '"') return true;
      if (c != '\\') {
        out.str.push_back(c);
        continue;
      }
      if (_i >= _s.size()) {
        err = "bad JSON escape";
        return false;
      }
      char e = _s[_i++];
      if (e == '"' || e == '\\' || e == '/') out.str.push_back(e);
      else if (e == 'b') out.str.push_back('\b');
      else if (e == 'f') out.str.push_back('\f');
      else if (e == 'n') out.str.push_back('\n');
      else if (e == 'r') out.str.push_back('\r');
      else if (e == 't') out.str.push_back('\t');
      else if (e == 'u') {
        if (_i + 4 > _s.size()) {
          err = "short JSON unicode escape";
          return false;
        }
        int cp = 0;
        for (int k = 0; k < 4; k++) {
          char h = _s[_i++];
          cp <<= 4;
          if (h >= '0' && h <= '9') cp += h - '0';
          else if (h >= 'a' && h <= 'f') cp += h - 'a' + 10;
          else if (h >= 'A' && h <= 'F') cp += h - 'A' + 10;
          else {
            err = "bad JSON unicode escape";
            return false;
          }
        }
        appendCodePoint(out.str, cp);
      } else {
        err = "bad JSON escape";
        return false;
      }
    }
    err = "JSON string not closed";
    return false;
  }

  bool boolVal(Json &out, string &err) {
    out = Json();
    out.type = Json::BOOL;
    if (_s.compare(_i, 4, "true") == 0) {
      out.b = true;
      _i += 4;
      return true;
    }
    if (_s.compare(_i, 5, "false") == 0) {
      out.b = false;
      _i += 5;
      return true;
    }
    err = "bad JSON bool";
    return false;
  }

  bool nullVal(Json &out, string &err) {
    if (_s.compare(_i, 4, "null") != 0) {
      err = "bad JSON null";
      return false;
    }
    out = Json();
    _i += 4;
    return true;
  }

  bool numberVal(Json &out, string &err) {
    size_t start = _i;
    if (_s[_i] == '-' || _s[_i] == '+') _i++;
    bool any = false;
    while (_i < _s.size() && std::isdigit(static_cast<unsigned char>(_s[_i]))) {
      _i++;
      any = true;
    }
    if (_i < _s.size() && _s[_i] == '.') {
      _i++;
      while (_i < _s.size() && std::isdigit(static_cast<unsigned char>(_s[_i]))) {
        _i++;
        any = true;
      }
    }
    if (_i < _s.size() && (_s[_i] == 'e' || _s[_i] == 'E')) {
      _i++;
      if (_i < _s.size() && (_s[_i] == '+' || _s[_i] == '-')) _i++;
      while (_i < _s.size() && std::isdigit(static_cast<unsigned char>(_s[_i]))) {
        _i++;
        any = true;
      }
    }
    if (!any) {
      err = "bad JSON number";
      return false;
    }
    try {
      out = Json();
      out.type = Json::NUMBER;
      out.num = std::stod(_s.substr(start, _i - start));
    } catch (...) {
      err = "bad JSON number";
      return false;
    }
    return true;
  }

};

void appendCodePoint(string &out, int cp) {
  if (cp < 0x80) {
    out.push_back(static_cast<char>(cp));
  } else if (cp < 0x800) {
    out.push_back(static_cast<char>(0xc0 | (cp >> 6)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
  } else {
    out.push_back(static_cast<char>(0xe0 | (cp >> 12)));
    out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
  }
}

string lowerCopy(string s) {
  for (size_t ii = 0; ii < s.size(); ii++) {
    s[ii] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[ii])));
  }
  return s;
}

string codecBase(string name) {
  name = lowerCopy(name);
  size_t slash = name.find_last_of('/');
  if (slash != string::npos) name = name.substr(slash + 1);
  size_t dot = name.find_last_of('.');
  if (dot != string::npos) name = name.substr(dot + 1);
  return name;
}

bool readFileBytes(const string &path, vector<unsigned char> &buf, string &err) {
  std::ifstream in(path.c_str(), std::ios::binary);
  if (!in) {
    err = "cannot open " + path;
    return false;
  }
  in.seekg(0, std::ios::end);
  std::streamoff n = in.tellg();
  if (n < 0 || static_cast<size_t>(n) > kMaxBytes) {
    err = "chunk too large or unreadable: " + path;
    return false;
  }
  in.seekg(0, std::ios::beg);
  buf.resize(static_cast<size_t>(n));
  if (n > 0) {
    in.read(reinterpret_cast<char *>(&buf[0]), n);
    if (!in) {
      err = "short read: " + path;
      return false;
    }
  }
  return true;
}

bool readTextFile(const string &path, string &text) {
  vector<unsigned char> buf;
  string err;
  if (!readFileBytes(path, buf, err)) return false;
  text.assign(reinterpret_cast<const char *>(&buf[0]), buf.size());
  return true;
}

bool parseJsonFile(const string &path, Json &out, string &err) {
  string text;
  if (!readTextFile(path, text)) {
    err = "cannot read " + path;
    return false;
  }
  JsonParser parser(text);
  if (!parser.parse(out, err)) {
    err = path + ": " + err;
    return false;
  }
  return true;
}

bool isDir(const string &path) {
  struct stat st;
  if (stat(path.c_str(), &st) != 0) return false;
  return S_ISDIR(st.st_mode);
}

bool isFile(const string &path) {
  struct stat st;
  if (stat(path.c_str(), &st) != 0) return false;
  return S_ISREG(st.st_mode);
}

string joinPath(const string &a, const string &b) {
  if (a.empty()) return b;
  if (b.empty()) return a;
  if (a[a.size() - 1] == '/') return a + b;
  return a + "/" + b;
}

bool product(const vector<size_t> &dims, size_t &out) {
  out = 1;
  for (size_t ii = 0; ii < dims.size(); ii++) {
    if (dims[ii] != 0 && out > kMaxBytes / dims[ii]) return false;
    out *= dims[ii];
  }
  return true;
}

bool hostLittleEndian() {
  uint16_t x = 1;
  return *reinterpret_cast<unsigned char *>(&x) == 1;
}

void swapBytes(unsigned char *p, size_t n, size_t item) {
  if (item < 2) return;
  for (size_t ii = 0; ii + item <= n; ii += item) {
    for (size_t a = 0, b = item - 1; a < b; a++, b--) {
      std::swap(p[ii + a], p[ii + b]);
    }
  }
}

bool jsonToSizeList(const Json *node, vector<size_t> &out) {
  if (node == NULL || node->type != Json::ARRAY) return false;
  out.clear();
  for (size_t ii = 0; ii < node->arr.size(); ii++) {
    if (node->arr[ii].type != Json::NUMBER || node->arr[ii].num < 0) return false;
    out.push_back(static_cast<size_t>(node->arr[ii].num));
  }
  return true;
}

bool jsonToString(const Json *node, string &out) {
  if (node == NULL) return false;
  if (node->type == Json::STRING) {
    out = node->str;
    return true;
  }
  if (node->type == Json::ARRAY && !node->arr.empty()) {
    return jsonToString(&node->arr[0], out);
  }
  if (node->type == Json::NUMBER) {
    std::ostringstream ss;
    ss << node->num;
    out = ss.str();
    return true;
  }
  if (node->type == Json::BOOL) {
    out = node->b ? "true" : "false";
    return true;
  }
  return false;
}

bool jsonToNumber(const Json *node, double &out) {
  if (node == NULL) return false;
  if (node->type == Json::NUMBER) {
    out = node->num;
    return true;
  }
  if (node->type == Json::STRING) {
    if (lowerCopy(node->str) == "nan") {
      out = std::numeric_limits<double>::quiet_NaN();
      return true;
    }
    try {
      out = std::stod(node->str);
      return true;
    } catch (...) {
      return false;
    }
  }
  if (node->type == Json::ARRAY && node->arr.size() == 1) {
    return jsonToNumber(&node->arr[0], out);
  }
  return false;
}

struct Codec {
  string name;
  bool littleEndian;
  bool hasEndian;
  vector<int> transposeOrder;
  bool hasTranspose;
  Codec() : littleEndian(true), hasEndian(false), hasTranspose(false) {}
};

struct ArrayInfo {
  string path;
  vector<size_t> shape;
  vector<size_t> chunks;
  vector<string> dims;
  int itemSize;
  char kind;
  bool littleEndian;
  bool isString;
  int stringChars;
  bool hasFill;
  bool fillNan;
  double fill;
  double scale;
  double offset;
  string units;
  string longName;
  string standardName;
  int version;
  string chunkPrefix;
  string chunkSep;
  vector<Codec> codecs;
  Json attributes;

  ArrayInfo() :
    itemSize(4), kind('f'), littleEndian(true), isString(false),
    stringChars(0), hasFill(false), fillNan(false), fill(0),
    scale(1), offset(0), version(3), chunkSep("/"),
    attributes() {}
};

bool decodeZstd(vector<unsigned char> &buf, string &err) {
  unsigned long long need = ZSTD_getFrameContentSize(&buf[0], buf.size());
  if (need == ZSTD_CONTENTSIZE_ERROR) {
    err = "invalid zstd frame";
    return false;
  }
  if (need == ZSTD_CONTENTSIZE_UNKNOWN || need > kMaxBytes) {
    ZSTD_DStream *stream = ZSTD_createDStream();
    if (stream == NULL) {
      err = "zstd init failed";
      return false;
    }
    ZSTD_initDStream(stream);
    ZSTD_inBuffer in;
    in.src = &buf[0];
    in.size = buf.size();
    in.pos = 0;
    vector<unsigned char> out;
    size_t produced = 0;
    while (in.pos < in.size) {
      if (produced + (1 << 20) > kMaxBytes) {
        ZSTD_freeDStream(stream);
        err = "zstd output too large";
        return false;
      }
      out.resize(produced + (1 << 20));
      ZSTD_outBuffer ob;
      ob.dst = &out[produced];
      ob.size = out.size() - produced;
      ob.pos = 0;
      size_t ret = ZSTD_decompressStream(stream, &ob, &in);
      produced += ob.pos;
      if (ZSTD_isError(ret)) {
        ZSTD_freeDStream(stream);
        err = ZSTD_getErrorName(ret);
        return false;
      }
      if (ret == 0) break;
    }
    ZSTD_freeDStream(stream);
    out.resize(produced);
    buf.swap(out);
    return true;
  }
  vector<unsigned char> out(static_cast<size_t>(need));
  size_t got = ZSTD_decompress(&out[0], out.size(), &buf[0], buf.size());
  if (ZSTD_isError(got)) {
    err = ZSTD_getErrorName(got);
    return false;
  }
  out.resize(got);
  buf.swap(out);
  return true;
}

bool decodeZlib(vector<unsigned char> &buf, string &err) {
  z_stream strm;
  std::memset(&strm, 0, sizeof(strm));
  if (inflateInit2(&strm, 15 + 32) != Z_OK) {
    err = "zlib init failed";
    return false;
  }
  strm.next_in = &buf[0];
  strm.avail_in = static_cast<uInt>(buf.size());
  vector<unsigned char> out;
  unsigned char tmp[1 << 15];
  int rc = Z_OK;
  while (rc != Z_STREAM_END) {
    strm.next_out = tmp;
    strm.avail_out = sizeof(tmp);
    rc = inflate(&strm, Z_NO_FLUSH);
    size_t have = sizeof(tmp) - strm.avail_out;
    if (out.size() + have > kMaxBytes) {
      inflateEnd(&strm);
      err = "zlib output too large";
      return false;
    }
    out.insert(out.end(), tmp, tmp + have);
    if (rc == Z_STREAM_END) break;
    if (rc != Z_OK && rc != Z_BUF_ERROR) {
      inflateEnd(&strm);
      err = "zlib inflate failed";
      return false;
    }
    if (have == 0 && strm.avail_in == 0) break;
  }
  inflateEnd(&strm);
  buf.swap(out);
  return true;
}

bool applyCodecs(vector<unsigned char> &buf, ArrayInfo &info, string &err) {
  for (int ii = static_cast<int>(info.codecs.size()) - 1; ii >= 0; ii--) {
    const Codec &codec = info.codecs[ii];
    if (codec.name == "bytes") {
      if (codec.hasEndian) info.littleEndian = codec.littleEndian;
      continue;
    }
    if (codec.name == "vlen-utf8" || codec.name == "vlenutf8") {
      info.isString = true;
      continue;
    }
    if (codec.name == "transpose") {
      bool identity = true;
      for (size_t k = 0; k < codec.transposeOrder.size(); k++) {
        if (codec.transposeOrder[k] != static_cast<int>(k)) identity = false;
      }
      if (!identity) {
        err = "non-identity Zarr transpose codec is not supported";
        return false;
      }
      continue;
    }
    if (codec.name == "crc32c" || codec.name == "crc32") {
      if (buf.size() >= 4) buf.resize(buf.size() - 4);
      continue;
    }
    if (codec.name == "zstd") {
      if (!decodeZstd(buf, err)) return false;
      continue;
    }
    if (codec.name == "zlib" || codec.name == "gzip" || codec.name == "deflate") {
      if (!decodeZlib(buf, err)) return false;
      continue;
    }
    if (codec.name == "blosc" || codec.name == "sharding" ||
        codec.name.find("shard") != string::npos) {
      err = "Zarr codec '" + codec.name +
        "' is not supported. Write the store with zstd or zlib.";
      return false;
    }
    err = "unsupported Zarr codec '" + codec.name + "'";
    return false;
  }
  if (!info.isString && info.itemSize > 1 &&
      info.littleEndian != hostLittleEndian()) {
    swapBytes(&buf[0], buf.size(), static_cast<size_t>(info.itemSize));
  }
  return true;
}

bool loadArrayMeta(const string &path, int version, ArrayInfo &info, string &err);

bool fillFromDtype(const string &dtype, ArrayInfo &info, string &err) {
  string d = dtype;
  info.littleEndian = true;
  if (!d.empty() && (d[0] == '<' || d[0] == '>' || d[0] == '|')) {
    info.littleEndian = d[0] != '>';
    d = d.substr(1);
  }
  info.isString = false;
  info.stringChars = 0;
  if (d == "f4" || d == "float32") { info.kind = 'f'; info.itemSize = 4; return true; }
  if (d == "f8" || d == "float64") { info.kind = 'f'; info.itemSize = 8; return true; }
  if (d == "i1" || d == "int8") { info.kind = 'i'; info.itemSize = 1; return true; }
  if (d == "i2" || d == "int16") { info.kind = 'i'; info.itemSize = 2; return true; }
  if (d == "i4" || d == "int32") { info.kind = 'i'; info.itemSize = 4; return true; }
  if (d == "i8" || d == "int64") { info.kind = 'i'; info.itemSize = 8; return true; }
  if (d == "u1" || d == "uint8" || d == "bool") { info.kind = 'u'; info.itemSize = 1; return true; }
  if (d == "u2" || d == "uint16") { info.kind = 'u'; info.itemSize = 2; return true; }
  if (d == "u4" || d == "uint32") { info.kind = 'u'; info.itemSize = 4; return true; }
  if (d == "u8" || d == "uint64") { info.kind = 'u'; info.itemSize = 8; return true; }
  if (!d.empty() && (d[0] == 'U' || d[0] == 'S')) {
    int n = 1;
    if (d.size() > 1) n = std::atoi(d.c_str() + 1);
    if (n < 1) n = 1;
    info.isString = true;
    info.kind = d[0];
    info.stringChars = n;
    info.itemSize = (d[0] == 'U') ? n * 4 : n;
    return true;
  }
  if (d == "string" || d == "vlen-utf8") {
    info.isString = true;
    info.kind = 'S';
    info.itemSize = 0;
    return true;
  }
  err = "unsupported Zarr data type '" + dtype + "'";
  return false;
}

void applyAttributes(ArrayInfo &info) {
  const Json *attrs = &info.attributes;
  string text;
  if (jsonToString(attrs->find("units"), text)) info.units = text;
  if (jsonToString(attrs->find("long_name"), text)) info.longName = text;
  if (jsonToString(attrs->find("standard_name"), text)) info.standardName = text;
  double number = 0;
  if (jsonToNumber(attrs->find("scale_factor"), number)) info.scale = number;
  if (jsonToNumber(attrs->find("add_offset"), number)) info.offset = number;
  const Json *fill = attrs->find("_FillValue");
  if (fill == NULL) fill = attrs->find("missing_value");
  if (fill != NULL && jsonToNumber(fill, number)) {
    info.hasFill = true;
    info.fill = number;
    info.fillNan = !std::isfinite(number);
  }
  const Json *dims = attrs->find("_ARRAY_DIMENSIONS");
  vector<size_t> ignored;
  if (info.dims.empty() && dims != NULL && dims->type == Json::ARRAY) {
    for (size_t ii = 0; ii < dims->arr.size(); ii++) {
      if (dims->arr[ii].type == Json::STRING) info.dims.push_back(dims->arr[ii].str);
    }
  }
  (void) ignored;
}

bool parseCodec(const Json &node, Codec &codec, string &err) {
  string name;
  const Json *nameNode = node.find("name");
  if (nameNode == NULL) nameNode = node.find("id");
  if (!jsonToString(nameNode, name)) {
    err = "Zarr codec has no name";
    return false;
  }
  codec.name = codecBase(name);
  const Json *conf = node.find("configuration");
  if (conf == NULL) conf = node.find("config");
  if (conf != NULL) {
    string endian;
    if (jsonToString(conf->find("endian"), endian)) {
      codec.hasEndian = true;
      codec.littleEndian = lowerCopy(endian) != "big";
    }
    const Json *order = conf->find("order");
    if (order != NULL && order->type == Json::ARRAY) {
      codec.hasTranspose = true;
      for (size_t ii = 0; ii < order->arr.size(); ii++) {
        if (order->arr[ii].type == Json::NUMBER) {
          codec.transposeOrder.push_back(static_cast<int>(order->arr[ii].num));
        }
      }
    }
  }
  return true;
}

bool loadV3Array(const string &path, const Json &meta, ArrayInfo &info, string &err) {
  info.path = path;
  info.version = 3;
  info.chunkPrefix = "c";
  info.chunkSep = "/";
  if (!jsonToSizeList(meta.find("shape"), info.shape)) {
    err = "Zarr array missing shape: " + path;
    return false;
  }
  const Json *grid = meta.find("chunk_grid");
  if (grid != NULL) {
    string gridName;
    jsonToString(grid->find("name"), gridName);
    if (!gridName.empty() && codecBase(gridName) != "regular") {
      err = "only regular Zarr chunk grids are supported";
      return false;
    }
    const Json *conf = grid->find("configuration");
    if (conf != NULL) jsonToSizeList(conf->find("chunk_shape"), info.chunks);
  }
  if (info.chunks.empty()) info.chunks = info.shape;
  const Json *keyEnc = meta.find("chunk_key_encoding");
  if (keyEnc != NULL) {
    string encName;
    jsonToString(keyEnc->find("name"), encName);
    encName = codecBase(encName);
    const Json *conf = keyEnc->find("configuration");
    if (conf != NULL) {
      string sep;
      if (jsonToString(conf->find("separator"), sep) && !sep.empty()) info.chunkSep = sep;
    }
    if (encName == "v2") info.chunkPrefix.clear();
  }
  string dtype;
  const Json *dt = meta.find("data_type");
  if (dt == NULL) {
    err = "Zarr array missing data_type: " + path;
    return false;
  }
  if (dt->type == Json::STRING) dtype = dt->str;
  else if (!jsonToString(dt->find("name"), dtype)) {
    err = "unrecognized data_type: " + path;
    return false;
  }
  if (dt->type == Json::OBJECT) {
    const Json *conf = dt->find("configuration");
    if (conf != NULL) {
      string unit;
      if (jsonToString(conf->find("unit"), unit)) {
        if (unit == "ns") info.units = "nanoseconds since 1970-01-01 00:00:00";
        else if (unit == "us") info.units = "microseconds since 1970-01-01 00:00:00";
        else if (unit == "ms") info.units = "milliseconds since 1970-01-01 00:00:00";
        else if (unit == "s") info.units = "seconds since 1970-01-01 00:00:00";
      }
    }
  }
  if (!fillFromDtype(dtype, info, err)) return false;
  const Json *codecs = meta.find("codecs");
  if (codecs != NULL && codecs->type == Json::ARRAY) {
    for (size_t ii = 0; ii < codecs->arr.size(); ii++) {
      Codec codec;
      if (!parseCodec(codecs->arr[ii], codec, err)) return false;
      info.codecs.push_back(codec);
    }
  }
  double fill = 0;
  if (jsonToNumber(meta.find("fill_value"), fill)) {
    info.hasFill = true;
    info.fill = fill;
    info.fillNan = !std::isfinite(fill);
  }
  const Json *attrs = meta.find("attributes");
  if (attrs != NULL) info.attributes = *attrs;
  const Json *dims = meta.find("dimension_names");
  if (dims != NULL && dims->type == Json::ARRAY) {
    for (size_t ii = 0; ii < dims->arr.size(); ii++) {
      if (dims->arr[ii].type == Json::STRING) info.dims.push_back(dims->arr[ii].str);
    }
  }
  applyAttributes(info);
  return true;
}

bool loadV2Array(const string &path, ArrayInfo &info, string &err) {
  Json meta;
  if (!parseJsonFile(joinPath(path, ".zarray"), meta, err)) return false;
  info.path = path;
  info.version = 2;
  info.chunkPrefix.clear();
  info.chunkSep = ".";
  if (!jsonToSizeList(meta.find("shape"), info.shape)) {
    err = "Zarr v2 array missing shape: " + path;
    return false;
  }
  if (!jsonToSizeList(meta.find("chunks"), info.chunks)) info.chunks = info.shape;
  string dtype;
  if (!jsonToString(meta.find("dtype"), dtype)) {
    err = "Zarr v2 array missing dtype: " + path;
    return false;
  }
  if (!fillFromDtype(dtype, info, err)) return false;
  string order;
  if (jsonToString(meta.find("order"), order) && order == "F") {
    err = "Fortran-order Zarr arrays are not supported";
    return false;
  }
  const Json *filters = meta.find("filters");
  if (filters != NULL && filters->type == Json::ARRAY && !filters->arr.empty()) {
    err = "Zarr v2 filters are not supported";
    return false;
  }
  const Json *comp = meta.find("compressor");
  if (comp != NULL && comp->type == Json::OBJECT) {
    Codec codec;
    if (!parseCodec(*comp, codec, err)) return false;
    info.codecs.push_back(codec);
  }
  double fill = 0;
  if (jsonToNumber(meta.find("fill_value"), fill)) {
    info.hasFill = true;
    info.fill = fill;
    info.fillNan = !std::isfinite(fill);
  }
  Json attrs;
  string attrErr;
  if (parseJsonFile(joinPath(path, ".zattrs"), attrs, attrErr)) {
    info.attributes = attrs;
  }
  applyAttributes(info);
  return true;
}

bool loadArray(const string &path, ArrayInfo &info, string &err) {
  if (isFile(joinPath(path, "zarr.json"))) {
    Json meta;
    if (!parseJsonFile(joinPath(path, "zarr.json"), meta, err)) return false;
    string nodeType;
    jsonToString(meta.find("node_type"), nodeType);
    if (nodeType == "group") {
      err = path + " is a group, not an array";
      return false;
    }
    return loadV3Array(path, meta, info, err);
  }
  if (isFile(joinPath(path, ".zarray"))) return loadV2Array(path, info, err);
  err = "no Zarr array metadata in " + path;
  return false;
}

bool isGroupPath(const string &path) {
  if (isFile(joinPath(path, "zarr.json"))) {
    Json meta;
    string err;
    if (!parseJsonFile(joinPath(path, "zarr.json"), meta, err)) return false;
    string nodeType;
    jsonToString(meta.find("node_type"), nodeType);
    if (nodeType.empty()) {
      int format = 0;
      const Json *fmt = meta.find("zarr_format");
      if (fmt != NULL && fmt->type == Json::NUMBER) format = static_cast<int>(fmt->num);
      return format == 2 || format == 3;
    }
    return nodeType == "group";
  }
  return isFile(joinPath(path, ".zgroup"));
}

bool listChildren(const string &path, vector<string> &names, string &err) {
  DIR *dir = opendir(path.c_str());
  if (dir == NULL) {
    err = "cannot open directory " + path;
    return false;
  }
  names.clear();
  while (dirent *ent = readdir(dir)) {
    string name = ent->d_name;
    if (name == "." || name == ".." || name == "c") continue;
    if (!name.empty() && name[0] == '.') continue;
    string child = joinPath(path, name);
    if (!isDir(child)) continue;
    if (isGroupPath(child) || isFile(joinPath(child, "zarr.json")) ||
        isFile(joinPath(child, ".zarray")) || isFile(joinPath(child, ".zgroup"))) {
      names.push_back(name);
    }
  }
  closedir(dir);
  std::sort(names.begin(), names.end());
  return true;
}

bool loadGroupAttrs(const string &path, Json &attrs, string &err) {
  if (isFile(joinPath(path, "zarr.json"))) {
    Json meta;
    if (!parseJsonFile(joinPath(path, "zarr.json"), meta, err)) return false;
    const Json *node = meta.find("attributes");
    if (node != NULL) attrs = *node;
    return true;
  }
  if (isFile(joinPath(path, ".zattrs"))) {
    return parseJsonFile(joinPath(path, ".zattrs"), attrs, err);
  }
  attrs = Json();
  attrs.type = Json::OBJECT;
  return true;
}

string chunkKey(const ArrayInfo &info, const vector<size_t> &index) {
  std::ostringstream ss;
  if (!info.chunkPrefix.empty()) ss << info.chunkPrefix;
  if (index.empty()) return ss.str();
  for (size_t ii = 0; ii < index.size(); ii++) {
    if (ii == 0) {
      if (!info.chunkPrefix.empty()) ss << info.chunkSep;
    } else {
      ss << info.chunkSep;
    }
    ss << index[ii];
  }
  return ss.str();
}

bool partialShape(const ArrayInfo &info, const vector<size_t> &chunkIndex,
                  vector<size_t> &out) {
  out.resize(info.shape.size());
  for (size_t d = 0; d < info.shape.size(); d++) {
    size_t chunk = info.chunks[d] == 0 ? info.shape[d] : info.chunks[d];
    size_t start = chunkIndex[d] * chunk;
    if (start >= info.shape[d]) return false;
    size_t len = chunk;
    if (start + len > info.shape[d]) len = info.shape[d] - start;
    out[d] = len;
  }
  return true;
}

size_t cOffset(const vector<size_t> &index, const vector<size_t> &shape) {
  size_t off = 0;
  size_t stride = 1;
  for (int d = static_cast<int>(shape.size()) - 1; d >= 0; d--) {
    off += index[d] * stride;
    stride *= shape[d];
  }
  return off;
}

bool nextIndex(vector<size_t> &index, const vector<size_t> &limit) {
  if (limit.empty()) return false;
  for (int d = static_cast<int>(limit.size()) - 1; d >= 0; d--) {
    index[d]++;
    if (index[d] < limit[d]) return true;
    index[d] = 0;
  }
  return false;
}

bool readChunk(const ArrayInfo &info, const vector<size_t> &chunkIndex,
               vector<unsigned char> &decoded, string &err) {
  string key = chunkKey(info, chunkIndex);
  string path = joinPath(info.path, key);
  if (!isFile(path) && info.shape.empty()) {
    if (isFile(joinPath(info.path, "c/0"))) path = joinPath(info.path, "c/0");
    else if (isFile(joinPath(info.path, "0"))) path = joinPath(info.path, "0");
  }
  vector<size_t> part;
  size_t nPart = 1;
  size_t nFull = 1;
  if (!info.shape.empty()) {
    if (!partialShape(info, chunkIndex, part)) {
      err = "bad chunk index";
      return false;
    }
    if (!product(part, nPart) || !product(info.chunks, nFull)) {
      err = "chunk is too large";
      return false;
    }
  }
  if (!isFile(path)) {
    size_t n = info.isString ? nPart : nPart * static_cast<size_t>(std::max(info.itemSize, 1));
    decoded.assign(n, 0);
    return true;
  }
  if (!readFileBytes(path, decoded, err)) return false;
  ArrayInfo local = info;
  if (!applyCodecs(decoded, local, err)) return false;
  return true;
}

double elementAsDouble(const ArrayInfo &info, const unsigned char *p) {
  if (info.kind == 'f' && info.itemSize == 4) {
    float v;
    std::memcpy(&v, p, 4);
    return static_cast<double>(v);
  }
  if (info.kind == 'f' && info.itemSize == 8) {
    double v;
    std::memcpy(&v, p, 8);
    return v;
  }
  if (info.kind == 'i' || info.kind == 'u') {
    int64_t v = 0;
    uint64_t u = 0;
    if (info.itemSize == 1) {
      if (info.kind == 'i') v = *reinterpret_cast<const int8_t *>(p);
      else v = p[0];
    } else if (info.itemSize == 2) {
      uint16_t t;
      std::memcpy(&t, p, 2);
      if (info.kind == 'i') v = static_cast<int16_t>(t);
      else v = t;
    } else if (info.itemSize == 4) {
      uint32_t t;
      std::memcpy(&t, p, 4);
      if (info.kind == 'i') v = static_cast<int32_t>(t);
      else v = t;
    } else if (info.itemSize == 8) {
      std::memcpy(&u, p, 8);
      if (info.kind == 'u') return static_cast<double>(u);
      std::memcpy(&v, p, 8);
    }
    return static_cast<double>(v);
  }
  return std::numeric_limits<double>::quiet_NaN();
}

bool readDoubles(const ArrayInfo &info, vector<double> &out, string &err) {
  size_t nElem = 1;
  if (!product(info.shape, nElem)) {
    err = "array is too large: " + info.path;
    return false;
  }
  if (info.shape.empty()) nElem = 1;
  out.assign(nElem, std::numeric_limits<double>::quiet_NaN());
  if (info.isString) {
    err = "expected a numeric array: " + info.path;
    return false;
  }
  vector<size_t> nChunk(info.shape.size(), 1);
  for (size_t d = 0; d < info.shape.size(); d++) {
    size_t chunk = info.chunks[d] == 0 ? info.shape[d] : info.chunks[d];
    nChunk[d] = (info.shape[d] + chunk - 1) / chunk;
  }
  vector<size_t> chunkIndex(info.shape.size(), 0);
  bool more = true;
  if (info.shape.empty()) {
    vector<unsigned char> raw;
    if (!readChunk(info, chunkIndex, raw, err)) return false;
    if (raw.size() >= static_cast<size_t>(info.itemSize) && info.itemSize > 0) {
      out[0] = elementAsDouble(info, &raw[0]);
    }
    more = false;
  }
  while (more) {
    vector<unsigned char> raw;
    if (!readChunk(info, chunkIndex, raw, err)) return false;
    vector<size_t> part;
    partialShape(info, chunkIndex, part);
    size_t nPart = 1;
    product(part, nPart);
    size_t nFull = 1;
    product(info.chunks, nFull);
    size_t nDecoded = info.itemSize > 0 ? raw.size() / static_cast<size_t>(info.itemSize) : 0;
    const vector<size_t> *indexShape = &part;
    if (nDecoded == nFull) indexShape = &info.chunks;
    else if (nDecoded < nPart) {
      err = "decoded chunk is short: " + info.path;
      return false;
    }
    vector<size_t> local(part.size(), 0);
    bool walking = !part.empty();
    size_t seen = 0;
    while (walking && seen < nPart) {
      vector<size_t> global(part.size());
      bool inside = true;
      for (size_t d = 0; d < part.size(); d++) {
        size_t chunk = info.chunks[d] == 0 ? info.shape[d] : info.chunks[d];
        global[d] = chunkIndex[d] * chunk + local[d];
        if (global[d] >= info.shape[d] || local[d] >= part[d]) inside = false;
      }
      if (inside) {
        size_t src = cOffset(local, *indexShape);
        if ((src + 1) * static_cast<size_t>(info.itemSize) <= raw.size()) {
          out[cOffset(global, info.shape)] =
            elementAsDouble(info, &raw[src * static_cast<size_t>(info.itemSize)]);
        }
      }
      seen++;
      walking = nextIndex(local, part);
    }
    more = nextIndex(chunkIndex, nChunk);
  }
  return true;
}

string fixedString(const ArrayInfo &info, const unsigned char *p) {
  string out;
  if (info.kind == 'U') {
    for (int ii = 0; ii < info.stringChars; ii++) {
      uint32_t cp = 0;
      std::memcpy(&cp, p + ii * 4, 4);
      if (cp == 0) break;
      appendCodePoint(out, static_cast<int>(cp));
    }
  } else {
    size_t n = static_cast<size_t>(info.stringChars > 0 ? info.stringChars : info.itemSize);
    out.assign(reinterpret_cast<const char *>(p), strnlen(reinterpret_cast<const char *>(p), n));
  }
  return out;
}

bool readStrings(const ArrayInfo &info, vector<string> &out, string &err) {
  size_t nElem = 1;
  if (!info.shape.empty() && !product(info.shape, nElem)) {
    err = "string array is too large";
    return false;
  }
  if (info.shape.empty()) nElem = 1;
  out.assign(nElem, "");
  vector<size_t> chunkIndex(info.shape.size(), 0);
  vector<unsigned char> raw;
  if (!readChunk(info, chunkIndex, raw, err)) return false;
  if (info.stringChars > 0 && info.itemSize > 0 &&
      raw.size() >= nElem * static_cast<size_t>(info.itemSize)) {
    for (size_t ii = 0; ii < nElem; ii++) {
      out[ii] = fixedString(info, &raw[ii * static_cast<size_t>(info.itemSize)]);
    }
    return true;
  }
  auto parseAt = [&](size_t pos, vector<string> &parsed) -> bool {
    parsed.assign(nElem, "");
    for (size_t ii = 0; ii < nElem; ii++) {
      if (pos + 4 > raw.size()) return false;
      uint32_t len = 0;
      std::memcpy(&len, &raw[pos], 4);
      pos += 4;
      if (len > raw.size() || pos + len > raw.size()) return false;
      parsed[ii].assign(reinterpret_cast<const char *>(&raw[pos]), len);
      pos += len;
    }
    return pos == raw.size();
  };
  vector<string> parsed;
  bool counted = false;
  if (raw.size() >= 4) {
    uint32_t head = 0;
    std::memcpy(&head, &raw[0], 4);
    if (head == nElem) counted = parseAt(4, parsed);
  }
  if (!counted && !parseAt(0, parsed)) {
    err = "truncated vlen string in " + info.path;
    return false;
  }
  out.swap(parsed);
  return true;
}

bool readScalarString(const string &store, const string &name, string &out) {
  ArrayInfo info;
  string err;
  string path = joinPath(store, name);
  if (!isDir(path)) return false;
  if (!loadArray(path, info, err)) return false;
  if (!info.isString && info.kind != 'U' && info.kind != 'S') return false;
  vector<string> values;
  if (!readStrings(info, values, err) || values.empty()) return false;
  out = values[0];
  return !out.empty();
}

bool readNumericMaybe(const string &store, const string &name, vector<double> &out) {
  ArrayInfo info;
  string err;
  string path = joinPath(store, name);
  if (!isDir(path) || !loadArray(path, info, err) || info.isString) return false;
  return readDoubles(info, out, err);
}

string attrString(const Json &attrs, const string &key) {
  string out;
  jsonToString(attrs.find(key), out);
  return out;
}

double attrNumber(const Json &attrs, const string &key, bool &ok) {
  double out = 0;
  ok = jsonToNumber(attrs.find(key), out);
  return out;
}

bool parseReferenceTime(const string &units, double &scale, double &refUnix) {
  string u = lowerCopy(units);
  scale = 1.0;
  if (u.find("nanosecond") != string::npos) scale = 1.0e-9;
  else if (u.find("microsecond") != string::npos) scale = 1.0e-6;
  else if (u.find("millisecond") != string::npos) scale = 1.0e-3;
  else if (u.find("minute") != string::npos) scale = 60.0;
  else if (u.find("hour") != string::npos) scale = 3600.0;
  else if (u.find("day") != string::npos) scale = 86400.0;
  size_t pos = u.find("since");
  if (pos == string::npos) {
    refUnix = 0;
    return !units.empty();
  }
  string rest = units.substr(pos + 5);
  while (!rest.empty() && std::isspace(static_cast<unsigned char>(rest[0]))) rest.erase(rest.begin());
  if (!rest.empty() && (rest[rest.size() - 1] == 'Z' || rest[rest.size() - 1] == 'z')) {
    rest.erase(rest.size() - 1);
  }
  int Y = 1970, M = 1, D = 1, h = 0, m = 0;
  double s = 0;
  if (std::sscanf(rest.c_str(), "%d-%d-%dT%d:%d:%lf", &Y, &M, &D, &h, &m, &s) < 3 &&
      std::sscanf(rest.c_str(), "%d-%d-%d %d:%d:%lf", &Y, &M, &D, &h, &m, &s) < 3 &&
      std::sscanf(rest.c_str(), "%d-%d-%d", &Y, &M, &D) < 3) {
    refUnix = 0;
    return false;
  }
  RadxTime when(Y, M, D, h, m, static_cast<int>(s), s - static_cast<int>(s));
  refUnix = static_cast<double>(when.utime()) + when.getSubSec();
  return true;
}

double timeToUnix(double raw, double scale, double refUnix) {
  if (!std::isfinite(raw)) return raw;
  if (scale == 1.0e-9) {
    int64_t ns = static_cast<int64_t>(std::llround(raw));
    int64_t sec = ns / 1000000000LL;
    int64_t rem = ns % 1000000000LL;
    if (rem < 0) {
      sec -= 1;
      rem += 1000000000LL;
    }
    return refUnix + static_cast<double>(sec) + static_cast<double>(rem) * 1.0e-9;
  }
  return refUnix + raw * scale;
}

bool isMissingRaw(const ArrayInfo &info, double raw) {
  if (!std::isfinite(raw)) return true;
  if (info.hasFill && !info.fillNan && raw == info.fill) return true;
  return false;
}

double scaledValue(const ArrayInfo &info, double raw) {
  if (isMissingRaw(info, raw)) return Radx::missingFl64;
  return raw * info.scale + info.offset;
}

int sweepOrdinal(const string &name, int fallback) {
  size_t pos = name.find_last_of('_');
  if (pos == string::npos || pos + 1 >= name.size()) return fallback;
  return std::atoi(name.c_str() + pos + 1);
}

Radx::SweepMode_t modeFromText(const string &text) {
  string m = lowerCopy(text);
  if (m.find("ppi") != string::npos || m.find("surveillance") != string::npos) {
    return Radx::SWEEP_MODE_AZIMUTH_SURVEILLANCE;
  }
  return Radx::sweepModeFromStr(m);
}

struct FieldSource {
  string name;
  ArrayInfo info;
  vector<double> data;
  bool rangeFirst;
};

class StoreReader {
public:
  bool debug;
  bool metadataOnly;
  string err;

  StoreReader() : debug(false), metadataOnly(false) {}

  bool read(const string &path, RadxVol &vol) {
    _root = path;
    while (!_root.empty() && _root[_root.size() - 1] == '/') _root.erase(_root.size() - 1);
    Json attrs;
    if (!loadGroupAttrs(_root, attrs, err)) return false;
    vol.clear();
    vol.setOrigFormat("ZARR");
    vol.setPathInUse(_root);
    copyGlobalAttrs(attrs, vol);
    double lat = 0, lon = 0, alt = 0;
    bool haveLat = false, haveLon = false, haveAlt = false;
    findLocation(lat, lon, alt, haveLat, haveLon, haveAlt);
    if (haveLat) vol.setLatitudeDeg(lat);
    if (haveLon) vol.setLongitudeDeg(lon);
    if (haveAlt) vol.setAltitudeKm(alt);
    vector<string> sweepNames;
    if (!sweepGroupNames(sweepNames)) return false;
    if (sweepNames.empty()) {
      err = "no sweep groups in " + _root;
      return false;
    }
    vector<double> fixedAngles;
    readNumericMaybe(_root, "sweep_fixed_angle", fixedAngles);
    int volumeNumber = 0;
    vector<double> volNum;
    if (readNumericMaybe(_root, "volume_number", volNum) && !volNum.empty()) {
      volumeNumber = static_cast<int>(volNum[0]);
    }
    vol.setVolumeNumber(volumeNumber);
    vector<double> freq;
    if (readNumericMaybe(_root, "frequency", freq)) {
      for (size_t ii = 0; ii < freq.size(); ii++) vol.addFrequencyHz(freq[ii]);
    }
    for (size_t isweep = 0; isweep < sweepNames.size(); isweep++) {
      double fixed = (isweep < fixedAngles.size()) ? fixedAngles[isweep]
                                                    : Radx::missingFl64;
      if (!readSweep(sweepNames[isweep], static_cast<int>(isweep), volumeNumber,
                     fixed, vol)) {
        return false;
      }
    }
    if (vol.getNRays() == 0) {
      err = "Zarr store contains no rays: " + _root;
      return false;
    }
    vol.loadSweepInfoFromRays();
    vol.loadVolumeInfoFromRays();
    if (debug) {
      std::cerr << "Zarr store " << _root
                << " sweeps=" << vol.getNSweeps()
                << " rays=" << vol.getNRays() << std::endl;
    }
    return true;
  }

private:
  string _root;

  void copyGlobalAttrs(const Json &attrs, RadxVol &vol) {
    string text;
    if (jsonToString(attrs.find("Conventions"), text)) vol.setConvention(text);
    if (jsonToString(attrs.find("version"), text)) vol.setVersion(text);
    if (jsonToString(attrs.find("title"), text)) vol.setTitle(text);
    if (jsonToString(attrs.find("source"), text)) vol.setSource(text);
    if (jsonToString(attrs.find("history"), text)) vol.setHistory(text);
    if (jsonToString(attrs.find("institution"), text)) vol.setInstitution(text);
    if (jsonToString(attrs.find("references"), text)) vol.setReferences(text);
    if (jsonToString(attrs.find("comment"), text)) vol.setComment(text);
    if (jsonToString(attrs.find("instrument_name"), text)) vol.setInstrumentName(text);
    if (jsonToString(attrs.find("site_name"), text)) vol.setSiteName(text);
    if (jsonToString(attrs.find("scan_name"), text)) vol.setScanName(text);
  }

  bool findScalarIn(const string &group, const string &name, double &out) {
    vector<double> values;
    if (readNumericMaybe(group, name, values) && !values.empty() &&
        std::isfinite(values[0])) {
      out = values[0];
      return true;
    }
    Json attrs;
    string ignored;
    if (!loadGroupAttrs(group, attrs, ignored)) return false;
    return jsonToNumber(attrs.find(name), out) && std::isfinite(out);
  }

  void findLocation(double &lat, double &lon, double &alt,
                    bool &haveLat, bool &haveLon, bool &haveAlt) {
    vector<string> groups;
    groups.push_back(_root);
    groups.push_back(joinPath(_root, "radar_parameters"));
    groups.push_back(joinPath(_root, "platform"));
    groups.push_back(joinPath(_root, "radar_calibration"));
    for (size_t ii = 0; ii < groups.size(); ii++) {
      if (!isDir(groups[ii]) && ii > 0) continue;
      if (!haveLat && findScalarIn(groups[ii], "latitude", lat)) haveLat = true;
      if (!haveLon && findScalarIn(groups[ii], "longitude", lon)) haveLon = true;
      double meters = 0;
      if (!haveAlt && (findScalarIn(groups[ii], "altitude", meters) ||
                       findScalarIn(groups[ii], "altitude_agl", meters))) {
        string units;
        ArrayInfo info;
        string ignored;
        if (loadArray(joinPath(groups[ii], "altitude"), info, ignored)) units = lowerCopy(info.units);
        if (units.find("km") != string::npos) alt = meters;
        else alt = meters / 1000.0;
        haveAlt = true;
      }
    }
  }

  bool sweepGroupNames(vector<string> &names) {
    vector<string> fromVar;
    ArrayInfo info;
    string localErr;
    if (loadArray(joinPath(_root, "sweep_group_name"), info, localErr)) {
      vector<string> values;
      if (info.isString && readStrings(info, values, localErr)) fromVar = values;
    }
    if (fromVar.empty()) {
      string one;
      if (readScalarString(_root, "sweep_group_name", one)) fromVar.push_back(one);
    }
    if (!fromVar.empty()) {
      vector<string> resolved;
      bool allFound = true;
      for (size_t ii = 0; ii < fromVar.size(); ii++) {
        string name = fromVar[ii];
        if (!name.empty() && name[0] == '/') name = name.substr(1);
        if (!isGroupPath(joinPath(_root, name))) {
          size_t dot = name.rfind('.');
          if (dot != string::npos &&
              isGroupPath(joinPath(_root, name.substr(0, dot)))) {
            name = name.substr(0, dot);
          }
        }
        if (!isGroupPath(joinPath(_root, name))) {
          allFound = false;
          break;
        }
        resolved.push_back(name);
      }
      if (allFound && !resolved.empty()) {
        names.swap(resolved);
        return true;
      }
    }
    vector<string> children;
    if (!listChildren(_root, children, err)) return false;
    for (size_t ii = 0; ii < children.size(); ii++) {
      if (children[ii].find("sweep") == 0 && isGroupPath(joinPath(_root, children[ii]))) {
        names.push_back(children[ii]);
      }
    }
    std::sort(names.begin(), names.end(), sweepNameLess);
    return true;
  }

  static bool sweepNameLess(const string &a, const string &b) {
    return sweepOrdinal(a, 0) < sweepOrdinal(b, 0);
  }

  bool readSweep(const string &name, int ordinal, int volumeNumber,
                 double rootFixed, RadxVol &vol) {
    string group = joinPath(_root, name);
    if (!isGroupPath(group)) {
      err = "missing sweep group " + name;
      return false;
    }
    ArrayInfo rangeInfo;
    if (!loadArray(joinPath(group, "range"), rangeInfo, err) ||
        !readDoublesInto(rangeInfo, err)) {
      err = "sweep " + name + " has no range array";
      return false;
    }
    vector<double> range = _doubles;
    if (range.empty()) {
      err = "sweep " + name + " has an empty range array";
      return false;
    }
    bool rangeMeters = true;
    if (lowerCopy(rangeInfo.units).find("km") != string::npos) rangeMeters = false;
    double startKm = rangeMeters ? range[0] / 1000.0 : range[0];
    double spacingKm = 0.25;
    if (range.size() > 1 && std::isfinite(range[0]) && std::isfinite(range[1])) {
      spacingKm = rangeMeters ? (range[1] - range[0]) / 1000.0 : (range[1] - range[0]);
    }
    if (!(spacingKm > 0)) spacingKm = 0.25;
    size_t nGates = range.size();

    vector<string> children;
    if (!listChildren(group, children, err)) return false;
    vector<FieldSource> fields;
    string rayDim;
    size_t nRays = 0;
    bool rangeFirst = false;
    for (size_t ii = 0; ii < children.size(); ii++) {
      if (children[ii] == "range") continue;
      ArrayInfo info;
      string localErr;
      if (!loadArray(joinPath(group, children[ii]), info, localErr)) continue;
      if (info.isString || info.shape.size() != 2) continue;
      int rangeAxis = -1;
      int rayAxis = -1;
      if (info.dims.size() == 2) {
        for (int ax = 0; ax < 2; ax++) {
          if (info.dims[ax] == "range") rangeAxis = ax;
        }
        if (rangeAxis >= 0) rayAxis = 1 - rangeAxis;
      } else if (info.shape[1] == nGates) {
        rangeAxis = 1;
        rayAxis = 0;
      } else if (info.shape[0] == nGates) {
        rangeAxis = 0;
        rayAxis = 1;
      }
      if (rangeAxis < 0) continue;
      if (rayDim.empty() && rayAxis >= 0 && info.dims.size() == 2) rayDim = info.dims[rayAxis];
      if (nRays == 0) nRays = info.shape[rayAxis];
      if (info.shape[rangeAxis] != nGates || info.shape[rayAxis] != nRays) continue;
      FieldSource field;
      field.name = children[ii];
      field.info = info;
      field.rangeFirst = rangeAxis == 0;
      rangeFirst = field.rangeFirst;
      if (!metadataOnly) {
        if (!readDoubles(info, field.data, err)) return false;
      }
      fields.push_back(field);
    }
    if (nRays == 0) {
      vector<double> az;
      if (readNumericMaybe(group, "azimuth", az) && az.size() > 1) nRays = az.size();
      else if (readNumericMaybe(group, "elevation", az) && az.size() > 1) nRays = az.size();
      else if (readNumericMaybe(group, "time", az)) nRays = az.size();
    }
    if (nRays == 0) {
      err = "sweep " + name + " has no rays";
      return false;
    }

    vector<double> azimuth, elevation, times, prt, nyquist, pulse, nSamples;
    readNumericMaybe(group, "azimuth", azimuth);
    readNumericMaybe(group, "elevation", elevation);
    ArrayInfo timeInfo;
    bool haveTime = loadArray(joinPath(group, "time"), timeInfo, err) &&
      readDoubles(timeInfo, times, err);
    if (!haveTime) {
      err.clear();
      times.clear();
    }
    readNumericMaybe(group, "prt", prt);
    readNumericMaybe(group, "nyquist_velocity", nyquist);
    readNumericMaybe(group, "pulse_width", pulse);
    readNumericMaybe(group, "n_samples", nSamples);

    string modeText;
    readScalarString(group, "sweep_mode", modeText);
    Radx::SweepMode_t mode = modeFromText(modeText);
    double fixed = rootFixed;
    vector<double> fixedVar;
    if (readNumericMaybe(group, "sweep_fixed_angle", fixedVar) && !fixedVar.empty()) {
      fixed = fixedVar[0];
    }
    if (!std::isfinite(fixed)) {
      if (mode == Radx::SWEEP_MODE_RHI && !azimuth.empty()) fixed = azimuth[0];
      else if (!elevation.empty()) fixed = elevation[0];
      else fixed = 0;
    }
    int sweepNumber = ordinal;
    vector<double> sweepNum;
    if (readNumericMaybe(group, "sweep_number", sweepNum) && !sweepNum.empty()) {
      sweepNumber = static_cast<int>(std::llround(sweepNum[0]));
    }

    double timeScale = 1;
    double timeRef = 0;
    if (haveTime) parseReferenceTime(timeInfo.units, timeScale, timeRef);

    vector<double> geoLat, geoLon, geoAlt;
    string geoGroup = joinPath(group, "georeference");
    if (!isDir(geoGroup)) geoGroup = joinPath(group, "georeferencing_correction");
    bool perRayGeo = false;
    if (isDir(geoGroup)) {
      readNumericMaybe(geoGroup, "latitude", geoLat);
      readNumericMaybe(geoGroup, "longitude", geoLon);
      readNumericMaybe(geoGroup, "altitude", geoAlt);
      perRayGeo = geoLat.size() == nRays && geoLon.size() == nRays;
    }

    for (size_t iray = 0; iray < nRays; iray++) {
      RadxRay *ray = new RadxRay;
      ray->setVolumeNumber(volumeNumber);
      ray->setSweepNumber(sweepNumber);
      ray->setSweepMode(mode);
      ray->setFixedAngleDeg(fixed);
      ray->setRangeGeom(startKm, spacingKm);
      ray->setAzimuthDeg(pickAngle(azimuth, iray, mode == Radx::SWEEP_MODE_RHI ? fixed : 0));
      ray->setElevationDeg(pickAngle(elevation, iray,
                                     mode == Radx::SWEEP_MODE_RHI ? 0 : fixed));
      if (iray < prt.size() && std::isfinite(prt[iray])) ray->setPrtSec(prt[iray]);
      if (iray < nyquist.size() && std::isfinite(nyquist[iray])) ray->setNyquistMps(nyquist[iray]);
      if (iray < pulse.size() && std::isfinite(pulse[iray])) {
        ray->setPulseWidthUsec(pulse[iray] * 1.0e6);
      }
      if (iray < nSamples.size() && std::isfinite(nSamples[iray])) {
        ray->setNSamples(static_cast<int>(std::llround(nSamples[iray])));
      }
      if (haveTime && iray < times.size()) {
        ray->setTime(timeToUnix(times[iray], timeScale, timeRef));
      }
      if (perRayGeo) {
        RadxGeoref geo;
        geo.setLatitude(geoLat[iray]);
        geo.setLongitude(geoLon[iray]);
        if (iray < geoAlt.size()) geo.setAltitudeKmMsl(geoAlt[iray] / 1000.0);
        ray->setGeoref(geo);
      }
      for (size_t ifield = 0; ifield < fields.size(); ifield++) {
        FieldSource &src = fields[ifield];
        if (isMetadataName(src.name)) continue;
        RadxField *field = new RadxField(src.name, src.info.units);
        if (!src.info.longName.empty()) field->setLongName(src.info.longName);
        if (!src.info.standardName.empty()) field->setStandardName(src.info.standardName);
        field->setMissingFl32(Radx::missingFl32);
        vector<Radx::fl32> gates(nGates, Radx::missingFl32);
        if (!metadataOnly) {
          for (size_t igate = 0; igate < nGates; igate++) {
            size_t idx = src.rangeFirst ? igate * nRays + iray : iray * nGates + igate;
            if (idx < src.data.size()) {
              double value = scaledValue(src.info, src.data[idx]);
              if (std::isfinite(value)) gates[igate] = static_cast<Radx::fl32>(value);
            }
          }
        }
        field->addDataFl32(nGates, &gates[0]);
        ray->addField(field);
      }
      ray->copyRangeGeomToFields();
      vol.addRay(ray);
    }
    return true;
  }

  bool readDoublesInto(const ArrayInfo &info, string &errNow) {
    return readDoubles(info, _doubles, errNow);
  }

  static double pickAngle(const vector<double> &values, size_t iray, double fallback) {
    if (values.size() == 1 && std::isfinite(values[0])) return values[0];
    if (iray < values.size() && std::isfinite(values[iray])) return values[iray];
    return fallback;
  }

  static bool isMetadataName(const string &name) {
    return name == "time" || name == "azimuth" || name == "elevation" ||
      name == "range" || name == "latitude" || name == "longitude" ||
      name == "altitude" || name == "sweep_number" || name == "sweep_mode" ||
      name == "sweep_fixed_angle" || name == "prt" || name == "prt_ratio" ||
      name == "pulse_width" || name == "nyquist_velocity" ||
      name == "unambiguous_range" || name == "n_samples" ||
      name == "follow_mode" || name == "prt_mode" || name == "polarization_mode";
  }

  vector<double> _doubles;
};

} // namespace

ZarrRadxFile::ZarrRadxFile() {
  _fileFormat = FILE_FORMAT_ZARR;
}

ZarrRadxFile::~ZarrRadxFile() {
}

bool ZarrRadxFile::isZarr(const string &path) {
  if (!isDir(path)) return false;
  if (isGroupPath(path)) return true;
  return isFile(joinPath(path, ".zgroup")) || isFile(joinPath(path, ".zarray"));
}

int ZarrRadxFile::readFromPath(const string &path, RadxVol &vol) {
  clearErrStr();
  StoreReader reader;
  reader.debug = _debug;
  reader.metadataOnly = _readMetadataOnly;
  if (!reader.read(path, vol)) {
    _addErrStr("ERROR - ZarrRadxFile::readFromPath");
    _addErrStr("  ", reader.err);
    vol.clear();
    return -1;
  }
  _fileFormat = FILE_FORMAT_ZARR;
  _pathInUse = path;
  return 0;
}
