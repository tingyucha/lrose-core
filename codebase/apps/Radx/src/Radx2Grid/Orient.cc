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
///////////////////////////////////////////////////////////////
// Orient class.
// Compute echo orientation:
//   vertical (convective)
//   horizontal (stratiform, bright-band, anvil)
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Jan 2020
//
///////////////////////////////////////////////////////////////

#include "Orient.hh"
#include "RhiOrient.hh"
#include "OutputMdv.hh"
#include <toolsa/pjg.h>
#include <toolsa/mem.h>
#include <toolsa/sincos.h>
#include <toolsa/toolsa_macros.h>
#include <Radx/RadxRay.hh>
#include <Radx/RadxField.hh>
#include <Radx/RadxTime.hh>
#include <Radx/RadxPath.hh>
#include <Radx/RadxSweep.hh>
#include <Mdv/DsMdvx.hh>
#include <algorithm>
using namespace std;

// Constructor

Orient::Orient(const Params &params,
               RadxVol &readVol,
               vector<Interp::Field> &interpFields,
               vector<Interp::Ray *> &interpRays,
               const vector<double> &gridZLevels) :
        _params(params),
        _readVol(readVol),
        _interpFields(interpFields),
        _interpRays(interpRays),
        _gridZLevels(gridZLevels)

{
  
  _rhiMode = false;

  gettimeofday(&_timeA, NULL);
  
  _radarAltKm = 0.0;
  _maxNGates = 0;
  _startRangeKm = 0.0;
  _gateSpacingKm = 0.0;
  _maxRangeKm = 0.0;

  // set up thread objects

  _createThreads();

}

//////////////////////////////////////
// destructor

Orient::~Orient()

{
  
  _freeThreads();

}

//////////////////////////////////////////////////
// determine echo orientation

int Orient::findEchoOrientation()

{

  // check scan mode

  if (_rhiMode) {
    cerr << "WARNING - Orient::findEchoOrientation()" << endl;
    cerr << "  Vol is in RHI mode - so this is not applicable" << endl;
    return -1;
  }

  // check for DBZ field

  const vector<RadxRay *> &rays = _readVol.getRays();
  if (rays.size() < 1) {
    cerr << "WARNING - Orient::findEchoOrientation()" << endl;
    cerr << "  No rays in vol" << endl;
    return -1;
  }

  if (rays[0]->getField(_params.echo_orientation_dbz_field_name) == NULL) {
    cerr << "WARNING - Orient::findEchoOrientation()" << endl;
    cerr << "  Cannot find field: " << _params.echo_orientation_dbz_field_name << endl;
    return -1;
  }

  // start of processing

  _printRunTime("start findEchoOrientation");

  // clear RHIs

  for (size_t ii = 0; ii < _rhis.size(); ii++) {
    delete _rhis[ii];
  }
  _rhis.clear();
  
  // create the synthetic RHIs
  
  double azimuth = _params.synthetic_rhis_start_az;
  while (azimuth < 360.0) {
    
    RhiOrient *rhi = new RhiOrient(_params, _readVol, azimuth,
                                   _startRangeKm, _gateSpacingKm,
                                   _radarAltKm, _gridZLevels);
    _rhis.push_back(rhi);

  }

  // set radar params from volume

  if (_setRadarParams()) {
    cerr << "ERROR - Orient::findEchoOrientation()" << endl;
    return -1;
  }

  // compute echo orientation in pseudo RHIs

  if (_params.debug) {
    cerr << "  Computing echo orientation in pseudo RHIs ... " << endl;
  }
  _printRunTime("Orient - _computeOrientInRhis start");
  _computeOrientInRhis();
  _printRunTime("Orient - _computeOrientInRhis end");

  // TO DO
  // add check on success

  // start of processing

  _printRunTime("end findEchoOrientation");

  return 0;

}

//////////////////////////////////////////////////
// create the threading objects

void Orient::_createThreads()
{

  // initialize thread pool for computing orientation in RHIs
  
  for (int ii = 0; ii < _params.n_compute_threads; ii++) {
    ComputeOrientInRhi *thread = new ComputeOrientInRhi(this);
    _threadPoolOrientInRhi.addThreadToMain(thread);
  }

}

//////////////////////////////////////////////////
// free the threading objects

void Orient::_freeThreads()
{

  // NOTE - thread pools free their threads in the destructor

}

////////////////////////////////////////////////////////////
// Compute grid locations relative to radar

void Orient::_computeOrientInRhis()

{

  if (_params.use_multiple_threads) {
    _computeOrientMultiThreaded();
  } else {
    _computeOrientSingleThreaded();
  }

}

//////////////////////////////////////////////////////
// compute grid rows in multi-threaded mode

void Orient::_computeOrientMultiThreaded()
{

  _threadPoolOrientInRhi.initForRun();
  
  // loop through the RHIs

  for (size_t ii = 0; ii < _rhis.size(); ii++) {
    // get a thread from the pool
    bool isDone = true;
    ComputeOrientInRhi *thread = 
      (ComputeOrientInRhi *) _threadPoolOrientInRhi.getNextThread(true, isDone);
    if (thread == NULL) {
      break;
    }
    if (isDone) {
      // if it is a done thread, return thread to the available pool
      _threadPoolOrientInRhi.addThreadToAvail(thread);
      // reduce ii by 1 since we did not actually process this RHI
      // since no thread is available
      ii--;
    } else {
      // available thread, set it running
      thread->signalRunToStart();
    } // isDone
  } // ii
  
  // collect remaining done threads
  
  _threadPoolOrientInRhi.setReadyForDoneCheck();
  while (!_threadPoolOrientInRhi.checkAllDone()) {
    ComputeOrientInRhi *thread = 
      (ComputeOrientInRhi *) _threadPoolOrientInRhi.getNextDoneThread();
    if (thread == NULL) {
      break;
    } else {
      _threadPoolOrientInRhi.addThreadToAvail(thread);
    }
  } // while

}

////////////////////////////////////////////////////////////
// Compute grid locations for one row

void Orient::_computeOrientSingleThreaded()

{

  for (size_t ii = 0; ii < _rhis.size(); ii++) {
    _rhis[ii]->computeEchoOrientation();
  }

}

//////////////////////////////////////////////////
// set the radar details

int Orient::_setRadarParams()

{

  // set radar altitude
  
  _radarAltKm = _readVol.getAltitudeKm();

  // set gate geometry

  _startRangeKm = _readVol.getStartRangeKm();
  _gateSpacingKm = _readVol.getGateSpacingKm();
  
  if (_startRangeKm <= -9990) {
    cerr << "ERROR - Orient::_setRadarParams()" << endl;
    cerr << "  Ray start range not set" << endl;
    cerr << "  startRangeKm: " << _startRangeKm << endl;
    cerr << "  You must use the option to override the gate geometry" << endl;
    cerr << "  in the param file" << endl;
    return -1;
  }
  
  if (_gateSpacingKm <= 0) {
    cerr << "ERROR - Orient::_setRadarParams()" << endl;
    cerr << "  Ray gate spacing not set" << endl;
    cerr << "  gateSpacingKm: " << _gateSpacingKm << endl;
    cerr << "  You must use the option to override the gate geometry" << endl;
    cerr << "  in the param file" << endl;
    return -1;
  }

  _maxNGates = 0;
  const vector<RadxRay *> &rays = _readVol.getRays();
  for (size_t ii = 0; ii < rays.size(); ii++) {
    const RadxRay *ray = rays[ii];
    int nGates = ray->getNGates();
    if (nGates > _maxNGates) {
      _maxNGates = nGates;
    }
  }
  _maxRangeKm = _startRangeKm + _maxNGates * _gateSpacingKm;

  return 0;

}

///////////////////////////////////////////////////////////////////////
// Print the elapsed run time since the previous call, in seconds.

void Orient::_printRunTime(const string& str, bool verbose /* = false */)
{
  if (verbose) {
    if (_params.debug < Params::DEBUG_VERBOSE) {
      return;
    }
  } else if (!_params.debug) {
    return;
  }
  struct timeval tvb;
  gettimeofday(&tvb, NULL);
  double deltaSec = tvb.tv_sec - _timeA.tv_sec
    + 1.e-6 * (tvb.tv_usec - _timeA.tv_usec);
  cerr << "TIMING, task: " << str << ", secs used: " << deltaSec << endl;
  _timeA.tv_sec = tvb.tv_sec;
  _timeA.tv_usec = tvb.tv_usec;
}

///////////////////////////////////////////////////////////////
// ComputeOrientInRhi thread
///////////////////////////////////////////////////////////////

// Constructor
Orient::ComputeOrientInRhi::ComputeOrientInRhi(Orient *obj) :
        _this(obj)
{
}  

// run method
void Orient::ComputeOrientInRhi::run()
{
  _this->_rhis[_index]->computeEchoOrientation();
}

