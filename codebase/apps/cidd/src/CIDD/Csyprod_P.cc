/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR                                                         */
/* ** University Corporation for Atmospheric Research (UCAR)                 */
/* ** National Center for Atmospheric Research (NCAR)                        */
/* ** Boulder, Colorado, USA                                                 */
/* ** BSD licence applies - redistribution and use in source and binary      */
/* ** forms, with or without modification, are permitted provided that       */
/* ** the following conditions are met:                                      */
/* ** 1) If the software is modified to produce derivative works,            */
/* ** such modified software should be clearly marked, so as not             */
/* ** to confuse it with the version available from UCAR.                    */
/* ** 2) Redistributions of source code must retain the above copyright      */
/* ** notice, this list of conditions and the following disclaimer.          */
/* ** 3) Redistributions in binary form must reproduce the above copyright   */
/* ** notice, this list of conditions and the following disclaimer in the    */
/* ** documentation and/or other materials provided with the distribution.   */
/* ** 4) Neither the name of UCAR nor the names of its contributors,         */
/* ** if any, may be used to endorse or promote products derived from        */
/* ** this software without specific prior written permission.               */
/* ** DISCLAIMER: THIS SOFTWARE IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESS  */
/* ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/* ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
////////////////////////////////////////////
// Csyprod_P.cc
//
// TDRP C++ code file for class 'Csyprod_P'.
//
// Code for program CIDD
//
// This file has been automatically
// generated by TDRP, do not modify.
//
/////////////////////////////////////////////

/**
 *
 * @file Csyprod_P.cc
 *
 * @class Csyprod_P
 *
 * This class is automatically generated by the Table
 * Driven Runtime Parameters (TDRP) system
 *
 * @note Source is automatically generated from
 *       paramdef file at compile time, do not modify
 *       since modifications will be overwritten.
 *
 *
 * @author Automatically generated
 *
 */
#include "Csyprod_P.hh"
#include <cstring>

  ////////////////////////////////////////////
  // Default constructor
  //

  Csyprod_P::Csyprod_P()

  {

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Csyprod_P";

    // initialize table

    _init();

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Copy constructor
  //

  Csyprod_P::Csyprod_P(const Csyprod_P& source)

  {

    // sync the source object

    source.sync();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Csyprod_P";

    // copy table

    tdrpCopyTable((TDRPtable *) source._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Destructor
  //

  Csyprod_P::~Csyprod_P()

  {

    // free up

    freeAll();

  }

  ////////////////////////////////////////////
  // Assignment
  //

  void Csyprod_P::operator=(const Csyprod_P& other)

  {

    // sync the other object

    other.sync();

    // free up any existing memory

    freeAll();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // copy table

    tdrpCopyTable((TDRPtable *) other._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = other._exitDeferred;

  }

  ////////////////////////////////////////////
  // loadFromArgs()
  //
  // Loads up TDRP using the command line args.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   char **params_path_p:
  //     If this is non-NULL, it is set to point to the path
  //     of the params file used.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Csyprod_P::loadFromArgs(int argc, char **argv,
                           char **override_list,
                           char **params_path_p,
                           bool defer_exit)
  {
    int exit_deferred;
    if (_tdrpLoadFromArgs(argc, argv,
                          _table, &_start_,
                          override_list, params_path_p,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadApplyArgs()
  //
  // Loads up TDRP using the params path passed in, and applies
  // the command line args for printing and checking.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   const char *param_file_path: the parameter file to be read in
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Csyprod_P::loadApplyArgs(const char *params_path,
                            int argc, char **argv,
                            char **override_list,
                            bool defer_exit)
  {
    int exit_deferred;
    if (tdrpLoadApplyArgs(params_path, argc, argv,
                          _table, &_start_,
                          override_list,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  //

  bool Csyprod_P::isArgValid(const char *arg)
  {
    return (tdrpIsArgValid(arg));
  }

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  // return number of args consumed.
  //

  int Csyprod_P::isArgValidN(const char *arg)
  {
    return (tdrpIsArgValidN(arg));
  }

  ////////////////////////////////////////////
  // load()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to load
  // up more than one class for a single application. It is a
  // lower-level routine than loadFromArgs, and hence more
  // flexible, but the programmer must do more work.
  //
  //   const char *param_file_path: the parameter file to be read in.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Csyprod_P::load(const char *param_file_path,
                   char **override_list,
                   int expand_env, int debug)
  {
    if (tdrpLoad(param_file_path,
                 _table, &_start_,
                 override_list,
                 expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadFromBuf()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to
  // load up more than one module for a single application,
  // using buffers which have been read from a specified source.
  //
  //   const char *param_source_str: a string which describes the
  //     source of the parameter information. It is used for
  //     error reporting only.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   const char *inbuf: the input buffer
  //
  //   int inlen: length of the input buffer
  //
  //   int start_line_num: the line number in the source which
  //     corresponds to the start of the buffer.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Csyprod_P::loadFromBuf(const char *param_source_str,
                          char **override_list,
                          const char *inbuf, int inlen,
                          int start_line_num,
                          int expand_env, int debug)
  {
    if (tdrpLoadFromBuf(param_source_str,
                        _table, &_start_,
                        override_list,
                        inbuf, inlen, start_line_num,
                        expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadDefaults()
  //
  // Loads up default params for a given class.
  //
  // See load() for more detailed info.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Csyprod_P::loadDefaults(int expand_env)
  {
    if (tdrpLoad(NULL,
                 _table, &_start_,
                 NULL, expand_env, FALSE)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // sync()
  //
  // Syncs the user struct data back into the parameter table,
  // in preparation for printing.
  //
  // This function alters the table in a consistent manner.
  // Therefore it can be regarded as const.
  //

  void Csyprod_P::sync(void) const
  {
    tdrpUser2Table(_table, (char *) &_start_);
  }

  ////////////////////////////////////////////
  // print()
  // 
  // Print params file
  //
  // The modes supported are:
  //
  //   PRINT_SHORT:   main comments only, no help or descriptions
  //                  structs and arrays on a single line
  //   PRINT_NORM:    short + descriptions and help
  //   PRINT_LONG:    norm  + arrays and structs expanded
  //   PRINT_VERBOSE: long  + private params included
  //

  void Csyprod_P::print(FILE *out, tdrp_print_mode_t mode)
  {
    tdrpPrint(out, _table, _className, mode);
  }

  ////////////////////////////////////////////
  // checkAllSet()
  //
  // Return TRUE if all set, FALSE if not.
  //
  // If out is non-NULL, prints out warning messages for those
  // parameters which are not set.
  //

  int Csyprod_P::checkAllSet(FILE *out)
  {
    return (tdrpCheckAllSet(out, _table, &_start_));
  }

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int Csyprod_P::checkIsSet(const char *paramName)
  {
    return (tdrpCheckIsSet(paramName, _table, &_start_));
  }

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void Csyprod_P::freeAll(void)
  {
    tdrpFreeAll(_table, &_start_);
  }

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  void Csyprod_P::usage(ostream &out)
  {
    out << "TDRP args: [options as below]\n"
        << "   [ -params/--params path ] specify params file path\n"
        << "   [ -check_params/--check_params] check which params are not set\n"
        << "   [ -print_params/--print_params [mode]] print parameters\n"
        << "     using following modes, default mode is 'norm'\n"
        << "       short:   main comments only, no help or descr\n"
        << "                structs and arrays on a single line\n"
        << "       norm:    short + descriptions and help\n"
        << "       long:    norm  + arrays and structs expanded\n"
        << "       verbose: long  + private params included\n"
        << "       short_expand:   short with env vars expanded\n"
        << "       norm_expand:    norm with env vars expanded\n"
        << "       long_expand:    long with env vars expanded\n"
        << "       verbose_expand: verbose with env vars expanded\n"
        << "   [ -tdrp_debug] debugging prints for tdrp\n"
        << "   [ -tdrp_usage] print this usage\n";
  }

  ////////////////////////////////////////////
  // arrayRealloc()
  //
  // Realloc 1D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int Csyprod_P::arrayRealloc(const char *param_name, int new_array_n)
  {
    if (tdrpArrayRealloc(_table, &_start_,
                         param_name, new_array_n)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // array2DRealloc()
  //
  // Realloc 2D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int Csyprod_P::array2DRealloc(const char *param_name,
                             int new_array_n1,
                             int new_array_n2)
  {
    if (tdrpArray2DRealloc(_table, &_start_, param_name,
                           new_array_n1, new_array_n2)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // _init()
  //
  // Class table initialization function.
  //
  //

  void Csyprod_P::_init()

  {

    TDRPtable *tt = _table;

    // Parameter 'debug'
    // ctype is '_debug_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("debug");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Debug option.\nIf set, debug messages will be printed appropriately.\n");
    tt->val_offset = (char *) &debug - &_start_;
    tt->enum_def.name = tdrpStrDup("debug_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("DEBUG_OFF");
      tt->enum_def.fields[0].val = DEBUG_OFF;
      tt->enum_def.fields[1].name = tdrpStrDup("DEBUG_NORM");
      tt->enum_def.fields[1].val = DEBUG_NORM;
      tt->enum_def.fields[2].name = tdrpStrDup("DEBUG_VERBOSE");
      tt->enum_def.fields[2].val = DEBUG_VERBOSE;
    tt->single_val.e = DEBUG_OFF;
    tt++;
    
    // Parameter 'short_requests'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("short_requests");
    tt->descr = tdrpStrDup("Gather Data Frame by Frame, Otherwise by loop span");
    tt->help = tdrpStrDup("Gathers Products valid for the current frame only - Speeds up gathering of large data sets, like METAR data");
    tt->val_offset = (char *) &short_requests - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'gzip_requests'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("gzip_requests");
    tt->descr = tdrpStrDup("Request data using GZIP compression");
    tt->help = tdrpStrDup("If true, symprod data will be GZIP compressed by the server.");
    tt->val_offset = (char *) &gzip_requests - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'prod_info'
    // ctype is '_prod_info_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRUCT_TYPE;
    tt->param_name = tdrpStrDup("prod_info");
    tt->descr = tdrpStrDup("Product Source and Rendering Information Array");
    tt->help = tdrpStrDup("The fields in the structure are as follows:\n\n  menu_label: label to be used for the product in menu.\n\n  url: for retrieving the data.\n    example: spdbp:transform_executable:[param_file]//host.dom:[port]:dir1/dir2\n\n  data_type: used when querying the SPDB data server\n    (set to 0 to retrieve all data).\n\n  render_type: Select how temporal clipping is done:\n    RENDER_ALL: Do no temporal clipping.\n    RENDER_ALL_VALID: Render all Valid in frame\n    RENDER_VALID_IN_LAST_FRAME: Render all Valid in the last frame\n    RENDER_LATEST_IN_FRAME: Render the latest product of like data_types (ID)  in the frame.\n    RENDER_LATEST_IN_LOOP:  Render the latest product of like data_types (ID)  valid in the movie loop.\n    RENDER_FIRST_BEFORE_FRAME_TIME:  Render the latest product of like data_types (ID) before the end of frame time\n    RENDER_FIRST_BEFORE_DATA_TIME:   Render the latest product of like data_types (ID  before the data time in each frame.\n    RENDER_FIRST_AFTER_DATA_TIME:    Render the earliest product after the data time in each frame.\n    RENDER_ALL_BEFORE_DATA_TIME:     Render all valid products before the data time in each frame.\n    RENDER_ALL_AFTER_DATA_TIME:      Render all valid products after the data time in each frame.\n    RENDER_GET_VALID:      Render all valid products returned with a GetValid Spdb call.\n    RENDER_GET_VALID_AT_FRAME_TIME:  Render products which have not expired at the frame time\n\nNote: all comparisons are less than or equal or greater than or equal\n\n  on_by_default: True or False\n\n  minutes_allow_before: Stretch the frame time this many minutes - prior\n  minutes_allow_after: Stretch the frame time this many minutes - after\n\n  text_off_threshold: Threshold where text disappears. Use the function:\n      log10(scaling_constant/km_distance_across_screen ) + 1.0\n      Use 0.0 to disable the off feature. (always displayed)\n\n  request_data_on_zoom: Flag indicating whether to request new data for\n      this product whenever the user changes the zoom.  Normally, this\n      should be set to false for efficiency reasons.  If the products\n      to be rendered would change with a zoom change then this should be\n      set to true.\n\n  request_data_on_vert_change: Flag indicating whether to request new data\n      for this product whenever the user changes the currently displayed\n      vertical level.  Normally, this should be set to false for efficiency\n      reasons.  If the products to be rendered would change with a zoom\n      change then this should be set to true.\n\n");
    tt->array_offset = (char *) &_prod_info - &_start_;
    tt->array_n_offset = (char *) &prod_info_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(prod_info_t);
    tt->array_n = 1;
    tt->struct_def.name = tdrpStrDup("prod_info_t");
    tt->struct_def.nfields = 10;
    tt->struct_def.fields = (struct_field_t *)
        tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
      tt->struct_def.fields[0].ftype = tdrpStrDup("string");
      tt->struct_def.fields[0].fname = tdrpStrDup("menu_label");
      tt->struct_def.fields[0].ptype = STRING_TYPE;
      tt->struct_def.fields[0].rel_offset = 
        (char *) &_prod_info->menu_label - (char *) _prod_info;
      tt->struct_def.fields[1].ftype = tdrpStrDup("string");
      tt->struct_def.fields[1].fname = tdrpStrDup("url");
      tt->struct_def.fields[1].ptype = STRING_TYPE;
      tt->struct_def.fields[1].rel_offset = 
        (char *) &_prod_info->url - (char *) _prod_info;
      tt->struct_def.fields[2].ftype = tdrpStrDup("int");
      tt->struct_def.fields[2].fname = tdrpStrDup("data_type");
      tt->struct_def.fields[2].ptype = INT_TYPE;
      tt->struct_def.fields[2].rel_offset = 
        (char *) &_prod_info->data_type - (char *) _prod_info;
      tt->struct_def.fields[3].ftype = tdrpStrDup("render_type_t");
      tt->struct_def.fields[3].fname = tdrpStrDup("render_type");
      tt->struct_def.fields[3].ptype = ENUM_TYPE;
      tt->struct_def.fields[3].rel_offset = 
        (char *) &_prod_info->render_type - (char *) _prod_info;
        tt->struct_def.fields[3].enum_def.name = tdrpStrDup("render_type_t");
        tt->struct_def.fields[3].enum_def.nfields = 12;
        tt->struct_def.fields[3].enum_def.fields = (enum_field_t *) tdrpMalloc
          (tt->struct_def.fields[3].enum_def.nfields * sizeof(enum_field_t));
        tt->struct_def.fields[3].enum_def.fields[0].name = tdrpStrDup("RENDER_ALL");
        tt->struct_def.fields[3].enum_def.fields[0].val = RENDER_ALL;
        tt->struct_def.fields[3].enum_def.fields[1].name = tdrpStrDup("RENDER_ALL_VALID");
        tt->struct_def.fields[3].enum_def.fields[1].val = RENDER_ALL_VALID;
        tt->struct_def.fields[3].enum_def.fields[2].name = tdrpStrDup("RENDER_VALID_IN_LAST_FRAME");
        tt->struct_def.fields[3].enum_def.fields[2].val = RENDER_VALID_IN_LAST_FRAME;
        tt->struct_def.fields[3].enum_def.fields[3].name = tdrpStrDup("RENDER_LATEST_IN_FRAME");
        tt->struct_def.fields[3].enum_def.fields[3].val = RENDER_LATEST_IN_FRAME;
        tt->struct_def.fields[3].enum_def.fields[4].name = tdrpStrDup("RENDER_LATEST_IN_LOOP");
        tt->struct_def.fields[3].enum_def.fields[4].val = RENDER_LATEST_IN_LOOP;
        tt->struct_def.fields[3].enum_def.fields[5].name = tdrpStrDup("RENDER_FIRST_BEFORE_FRAME_TIME");
        tt->struct_def.fields[3].enum_def.fields[5].val = RENDER_FIRST_BEFORE_FRAME_TIME;
        tt->struct_def.fields[3].enum_def.fields[6].name = tdrpStrDup("RENDER_FIRST_BEFORE_DATA_TIME");
        tt->struct_def.fields[3].enum_def.fields[6].val = RENDER_FIRST_BEFORE_DATA_TIME;
        tt->struct_def.fields[3].enum_def.fields[7].name = tdrpStrDup("RENDER_FIRST_AFTER_DATA_TIME");
        tt->struct_def.fields[3].enum_def.fields[7].val = RENDER_FIRST_AFTER_DATA_TIME;
        tt->struct_def.fields[3].enum_def.fields[8].name = tdrpStrDup("RENDER_ALL_BEFORE_DATA_TIME");
        tt->struct_def.fields[3].enum_def.fields[8].val = RENDER_ALL_BEFORE_DATA_TIME;
        tt->struct_def.fields[3].enum_def.fields[9].name = tdrpStrDup("RENDER_ALL_AFTER_DATA_TIME");
        tt->struct_def.fields[3].enum_def.fields[9].val = RENDER_ALL_AFTER_DATA_TIME;
        tt->struct_def.fields[3].enum_def.fields[10].name = tdrpStrDup("RENDER_GET_VALID");
        tt->struct_def.fields[3].enum_def.fields[10].val = RENDER_GET_VALID;
        tt->struct_def.fields[3].enum_def.fields[11].name = tdrpStrDup("RENDER_GET_VALID_AT_FRAME_TIME");
        tt->struct_def.fields[3].enum_def.fields[11].val = RENDER_GET_VALID_AT_FRAME_TIME;
      tt->struct_def.fields[4].ftype = tdrpStrDup("boolean");
      tt->struct_def.fields[4].fname = tdrpStrDup("on_by_default");
      tt->struct_def.fields[4].ptype = BOOL_TYPE;
      tt->struct_def.fields[4].rel_offset = 
        (char *) &_prod_info->on_by_default - (char *) _prod_info;
      tt->struct_def.fields[5].ftype = tdrpStrDup("double");
      tt->struct_def.fields[5].fname = tdrpStrDup("minutes_allow_before");
      tt->struct_def.fields[5].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[5].rel_offset = 
        (char *) &_prod_info->minutes_allow_before - (char *) _prod_info;
      tt->struct_def.fields[6].ftype = tdrpStrDup("double");
      tt->struct_def.fields[6].fname = tdrpStrDup("minutes_allow_after");
      tt->struct_def.fields[6].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[6].rel_offset = 
        (char *) &_prod_info->minutes_allow_after - (char *) _prod_info;
      tt->struct_def.fields[7].ftype = tdrpStrDup("double");
      tt->struct_def.fields[7].fname = tdrpStrDup("text_off_threshold");
      tt->struct_def.fields[7].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[7].rel_offset = 
        (char *) &_prod_info->text_off_threshold - (char *) _prod_info;
      tt->struct_def.fields[8].ftype = tdrpStrDup("boolean");
      tt->struct_def.fields[8].fname = tdrpStrDup("request_data_on_zoom");
      tt->struct_def.fields[8].ptype = BOOL_TYPE;
      tt->struct_def.fields[8].rel_offset = 
        (char *) &_prod_info->request_data_on_zoom - (char *) _prod_info;
      tt->struct_def.fields[9].ftype = tdrpStrDup("boolean");
      tt->struct_def.fields[9].fname = tdrpStrDup("request_data_on_vert_change");
      tt->struct_def.fields[9].ptype = BOOL_TYPE;
      tt->struct_def.fields[9].rel_offset = 
        (char *) &_prod_info->request_data_on_vert_change - (char *) _prod_info;
    tt->n_struct_vals = 10;
    tt->struct_vals = (tdrpVal_t *)
        tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
      tt->struct_vals[0].s = tdrpStrDup("RHI-ticks");
      tt->struct_vals[1].s = tdrpStrDup("spdbp:Rhi2Symprod://localhost::spdb/rhi");
      tt->struct_vals[2].i = 0;
      tt->struct_vals[3].e = RENDER_ALL_VALID;
      tt->struct_vals[4].b = pFALSE;
      tt->struct_vals[5].d = 20;
      tt->struct_vals[6].d = 10;
      tt->struct_vals[7].d = 0.4;
      tt->struct_vals[8].b = pFALSE;
      tt->struct_vals[9].b = pFALSE;
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }
