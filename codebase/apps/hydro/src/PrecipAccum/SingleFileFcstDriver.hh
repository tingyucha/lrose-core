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
/////////////////////////////////////////////////////////////
// SingleFileFcstDriver.hh
//
// SingleFileFcstDriver object
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Oct 2002
//
///////////////////////////////////////////////////////////////

#ifndef SingleFileFcstDriver_HH
#define SingleFileFcstDriver_HH

#include <toolsa/umisc.h>
#include <dsdata/DsTrigger.hh>

#include "Args.hh"
#include "Params.hh"
#include "MethodDriver.hh"
using namespace std;


////////////////
// SingleFileFcstDriver
//
// Derived class for running accumulation
//

class SingleFileFcstDriver : public MethodDriver {
  
public:
  
  // constructor
  
  SingleFileFcstDriver(const string &prog_name,
		   const Args &args, const Params &params);

  // destructor
  
  virtual ~SingleFileFcstDriver();

  // override run method
  
  virtual int Run();

protected:
  
private:
  
  time_t _triggerTime;
  DsTrigger *_dataTrigger;

  int _doAccum(time_t inputTime, int leadTime, const string filePath);

};

#endif
