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
/********************************************************
 * Approximation of a discrete real function F(x) by     *
 * least squares                                         *
 * ----------------------------------------------------- *
 * Ref.: "Méthodes de calcul numérique, Tome 2 by Claude *
 *        Nowakowski, PSI Edition, 1984" [BIBLI 04].     *
 * ----------------------------------------------------- *
 * C++ version by J-P Moreau, Paris.                     *
 * (www.jpmoreau.fr)                                     *
 ********************************************************/
///////////////////////////////////////////////////////////////
// ForsytheFit.cc
//
// Regression fit to (x, y) data set using Forsythe polynomials
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// June 2020
//
///////////////////////////////////////////////////////////////

#include <rapmath/ForsytheFit.hh>
#include <toolsa/mem.h>
#include <rapmath/usvd.h>
#include <iostream>
using namespace std;

// #define DEBUG_PRINT

////////////////////////////////////////////////////
// constructor

ForsytheFit::ForsytheFit()
        
{
  _init();
  clear();
  setOrder(3);
}

////////////////////////////////////////////////////
// destructor

ForsytheFit::~ForsytheFit()
  
{
  // clear();
}

//////////////////////////////////////////////////////////////////
// clear the data values
  
void ForsytheFit::clear()
{
  _xObs.clear();
  _yObs.clear();
  _nObs = 0;
}

//////////////////////////////////////////////////////////////////
// set polynomial order

void ForsytheFit::setOrder(size_t order) 
{
  _order = order;
  _allocPolyArrays();
}

//////////////////////////////////////////////////////////////////
// add a data value

void ForsytheFit::addValue(double xx, double yy) 
{
  
  _xObs.push_back(xx);
  _yObs.push_back(yy);
  _nObs = _xObs.size();

}

//////////////////////////////////////////////////////////////////
// set the data values from vectors
    
void ForsytheFit::setValues(const vector<double> &xVals,
                            const vector<double> &yVals)

{
  
  if (xVals.size() != yVals.size()) {
    cerr << "ERROR - ForsytheFit::setValues()" << endl;
    cerr << "  x and y value vector lengths do not match" << endl;
    cerr << "  xVals.size(): " << xVals.size() << endl;
    cerr << "  yVals.size(): " << yVals.size() << endl;
    clear();
    return;
  }
    
  if (xVals.size() < 1) {
    clear();
    return;
  }

  _xObs = xVals;
  _yObs = yVals;
  _nObs = _xObs.size();

}

////////////////////////////////////////////////////
// get single y value, given the x value

double ForsytheFit::getYEst(double xx)
{
  if (_coeffs.size() < 1) {
    return 0.0;
  }
  double yy = _coeffs[0];
  for (size_t ii = 1; ii < _coeffs.size(); ii++) {
    yy += _coeffs[ii] * pow(xx, (double) ii);
  }
  return yy;
}

////////////////////////////////////////////////////
// get single y value, given the index

double ForsytheFit::getYEst(size_t index)
{
  if (index > _nObs - 1) {
    cerr << "ERROR - ForsytheFit::getYEst()" << endl;
    cerr << "  Index out of range: " << index << endl;
    cerr << "  Max index: " << _nObs - 1 << endl;
    return NAN;
  }
  return _yEst[index];
}

////////////////////////////////////////////////////
// get vector of estimated y values

vector<double> ForsytheFit::getYEst() const
{
  vector<double> yyEst;
  for (size_t ii = 0; ii < _nObs; ii++) {
    yyEst.push_back(_yEst[ii]);
  }
  return yyEst;
}

////////////////////////////////////////////////////
// perform a fit

int ForsytheFit::performFit()
  
{

  // allocate arrays
  
  _allocDataArrays();

  // check conditions

  if (_nObs < _order + 2) {
    cerr << "ERROR - ForsytheFit::performFit()" << endl;
    cerr << "  Not enough observations to  fit order: " << _order << endl;
    cerr << "  Min n obs: " << _order << endl;
    return -1;
  }

  int fitOrder;
  double sdev;

  _doFit(_order, 0, _nObs, fitOrder, _xObs, _yObs, _coeffs, sdev);  

  return 0;

}

/////////////////////////////
// initialization

void ForsytheFit::_init()

{

}

/////////////////////////////////////////////////////
// allocate space

void ForsytheFit::_allocDataArrays()

{
  
  _yEst.resize(_nObs);
  _cc.resize(_nObs);
  _ee.resize(_nObs);
  _vv.resize(_nObs);
  
}

void ForsytheFit::_allocPolyArrays()

{

  _aa.resize(_order + 2);
  _bb.resize(_order + 2);
  _c2.resize(_order + 2);
  _ff.resize(_order + 2);
  _coeffs.resize(_order + 2);

}

///////////////////////////////////////////////////////////////////////
//         LEAST SQUARES POLYNOMIAL FITTING PROCEDURE            
// ------------------------------------------------------------- 
// This program least squares fits a polynomial to input data.   
// forsythe orthogonal polynomials are used in the fitting.      
// The number of data points is nn.                               
// The data is input to the subroutine in xx[i], yy[i] pairs.      
// The coefficients are returned in coeffs[i],                        
// the smoothed data is returned in vv[i],                        
// the order of the fit is specified by m.                       
// The standard deviation of the fit is returned in sdev.
// There are two options available by use of the parameter ee:    
//  1. if ee = 0, the fit is to order mm,                          
//  2. if ee > 0, the order of fit increases towards mm, but will  
//     stop if the relative standard deviation does not decrease 
//     by more than e between successive fits.                   
// The order of the fit then obtained is ll.                      
///////////////////////////////////////////////////////////////////////

int ForsytheFit::_doFit(int mm, double ee, int nn, int &ll,
                        vector<double> &xx, vector<double> &yy,
                        vector<double> &coeffs, double &sdev)
{

  // arrays
  
  double _aa[1024], _bb[1024], _cc[1024], _ee[1024], _ff[1024];
  double _vv[1024], _c2[1024];
  
  // Function Body

  int mm1 = mm + 1;
  ll = 0;
  double v1 = 1e7;

  // Initialize the arrays

  for (int ii = 1; ii <= mm1; ++ii) {
    _aa[ii - 1] = 0.0;
    _bb[ii - 1] = 0.0;
    _ff[ii - 1] = 0.0;
  }

  for (int ii = 1; ii <= nn; ++ii) {
    _vv[ii - 1] = 0.0;
    _cc[ii - 1] = 0.0;
  }

  double d1 = sqrt((double) nn);
  double ww = d1;

  for (int ii = 1; ii <= nn; ++ii) {
    _ee[ii - 1] = 1. / ww;
  }

  double f1 = d1;
  double a1 = 0.0;
  for (int ii = 1; ii <= nn; ++ii) {
    a1 += xx[ii-1] * _ee[ii - 1] * _ee[ii - 1];
  }

  double c1 = 0.0;
  for (int ii = 1; ii <= nn; ++ii) {
    c1 += yy[ii-1] * _ee[ii - 1];
  }

  _bb[0] = 1. / f1;
  _ff[0] = _bb[0] * c1;
  for (int ii = 1; ii <= nn; ++ii) {
    _vv[ii - 1] += +_ee[ii - 1] * c1;
  }

  int orderSoFar = 1;

  // Save latest results

  while (orderSoFar < mm1) {

    for (int ii = 1; ii <= ll; ++ii) {
      _c2[ii - 1] = coeffs[ii-1];
    }

    int l2 = ll;
    double v2 = v1;
    double f2 = f1;
    double a2 = a1;
    f1 = 0.0;
    for (int ii = 1; ii <= nn; ++ii) {
      double b1 = _ee[ii - 1];
      _ee[ii - 1] = (xx[ii-1] - a2) * _ee[ii - 1] - f2 * _cc[ii - 1];
      _cc[ii - 1] = b1;
      f1 += _ee[ii - 1] * _ee[ii - 1];
    }
    f1 = sqrt(f1);
    for (int ii = 1; ii <= nn; ++ii) {
      _ee[ii - 1] /= f1;
    }
    a1 = 0.0;
    for (int ii = 1; ii <= nn; ++ii) {
      c1 += _ee[ii - 1] * yy[ii-1];
    }
    ++(orderSoFar);

    int jj = 0;
    while (jj < orderSoFar) {
      ll = orderSoFar - jj;
      double b2 = _bb[ll - 1];
      d1 = 0.0;
      if (ll > 1) {
        d1 = _bb[ll - 2];
      }
      d1 = d1 - a2 * _bb[ll - 1] - f2 * _aa[ll - 1];
      _bb[ll - 1] = d1 / f1;
      _aa[ll - 1] = b2;
      ++jj;
    }

    for (int ii = 1; ii <= nn; ++ii) {
      _vv[ii - 1] += _ee[ii - 1] * c1;
    }
    for (int ii = 1; ii <= mm1; ++ii) {
      _ff[ii - 1] += _bb[ii - 1] * c1;
      coeffs[ii-1] = _ff[ii - 1];
    }

    double vv = 0.0;
    for (int ii = 1; ii <= nn; ++ii) {
      vv += (_vv[ii - 1] - yy[ii-1]) * (_vv[ii - 1] - yy[ii-1]);
    }

    // Note the division is by the number of degrees of freedom
    vv = sqrt(vv / (double) (nn - ll - 1));

    ll = orderSoFar;

    if (ee != 0.0) {
      // Test for minimal improvement
      if ((fabs(v1 - vv) / vv < ee) || (ee * vv > ee * v1)) {
        ll = l2;
        vv = v2;
        for (int ii = 1; ii <= ll; ++ii) {
          coeffs[ii-1] = _c2[ii - 1];
        }
        sdev = vv;
        break;
      }
      v1 = vv;
    }

  } // while (orderSoFar < mm1)

  // Shift the c[i] down, so c(0) is the constant term

  for (int ii = 1; ii <= ll; ++ii) {
    coeffs[ii - 1] = coeffs[ii];
  }
  coeffs[ll-1] = 0.0;

  // ll is the order of the polynomial fitted
  --(ll);

  return 0;

}

//////////////////////////////////////////////  
// print vector
//

void ForsytheFit::_vectorPrint(string name,
                               vector<double> aa,
                               FILE *out) const
  
{
  
  fprintf(out, "=========== %10s ===========\n", name.c_str());
  for (size_t ii = 0; ii < aa.size(); ii++) {
    fprintf(out, " %8.2g", aa[ii]);
  }
  fprintf(out, "\n");
  fprintf(out, "==================================\n");

}
  
