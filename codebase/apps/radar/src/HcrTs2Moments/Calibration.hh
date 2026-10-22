// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
///////////////////////////////////////////////////////////
// Calibration.hh
// 
// Singleton - read and store calibration data
//
// EOL, NCAR, Boulder CO
//
// August 2007
//
// Mike Dixon
//
///////////////////////////////////////////////////////////

#ifndef Calibration_HH
#define Calibration_HH

#include "Params.hh"
#include "Beam.hh"
#include <cstdio>
#include <radar/IwrfCalib.hh>
#include <map>
#include <pthread.h>

typedef multimap<time_t, string, less<time_t> > FileMap;
typedef pair<const time_t, string > FilePair;

using namespace std;

/////////////////////////////////////////////////////
// clibration for specific pulse width

class Calibration
{

public:
  
  // Constructor

  Calibration(const Params &params,
              double pulse_width_us);

  // Destructor
  
  ~Calibration();
  
  // Read calibration for a given file.
  // Returns 0 on success, -1 on failure
  
  int readCal(const string &filePath);

  // get current values

  const IwrfCalib &getIwrfCalib() const { return _calib; }
  double getPulseWidthUs() const { return _pulseWidthUs; }
  time_t getCalTime() const { return _calTime; }
  const string &getCalPath() const { return _calFilePath; }

private:

  // data

  const Params &_params;
  
  IwrfCalib _calib;
  double _pulseWidthUs;
  time_t _calTime;
  string _calFilePath;

  // functions
  
  void _applyCorrections();
  
};


#endif
