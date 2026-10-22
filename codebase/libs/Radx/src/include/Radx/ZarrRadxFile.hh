#ifndef ZarrRadxFile_HH
#define ZarrRadxFile_HH

#include <string>
#include <Radx/RadxFile.hh>

class RadxVol;

/// Read a directory Zarr store into a RadxVol.
///
/// Accepts Zarr v2 (.zgroup/.zarray) and Zarr v3 (zarr.json) stores that
/// use a CfRadial2 group layout: one group per sweep, a range coordinate,
/// and moment arrays shaped (ray, range) or (range, ray). The ray axis may
/// be named time, azimuth, or elevation. Radar parameters, the
/// radar_calibration group, and per-ray metadata (beam width, calibration
/// index, antenna transition, scan rate, unambiguous range, transmit power)
/// are copied onto the volume. Chunk codecs supported are raw bytes,
/// zlib/gzip, zstd, and vlen-utf8.

class ZarrRadxFile : public RadxFile {

public:

  ZarrRadxFile();
  virtual ~ZarrRadxFile();

  /// Read a Zarr store directory into vol.
  /// Returns 0 on success, -1 on failure.

  virtual int readFromPath(const std::string &path, RadxVol &vol);

  /// True when path is a Zarr v2 or v3 directory store.

  static bool isZarr(const std::string &path);

};

#endif
