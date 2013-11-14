//---------------------------------------------------------------------------//
//!
//! \file   Interpolation.cpp
//! \author Alex Robinson
//! \brief  Interpolation function definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <math.h>
#include <limits>

// TPOR Includes
#include "Interpolation.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Linear-linear interpolation function
double linlinInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 )
{
  // Make sure the independent variables are valid
  testPrecondition( x0 < x1 );
  testPrecondition( x0 != -std::numeric_limits<double>::infinity() );
  testPrecondition( x1 != std::numeric_limits<double>::infinity() );
  testPrecondition( x >= x0 );
  testPrecondition( x <= x1 );
  // Make sure the dependent variables are valid
  testPrecondition( y0 != -std::numeric_limits<double>::infinity() );
  testPrecondition( y1 != std::numeric_limits<double>::infinity() );
  
  return y0 + (y1-y0)/(x1-x0)*(x-x0);
}

// Log-linear interpolation function
double loglinInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 )
{
  // Make sure the independent variables are valid
  testPrecondition( x0 < x1 );
  testPrecondition( x0 != -std::numeric_limits<double>::infinity() );
  testPrecondition( x1 != std::numeric_limits<double>::infinity() );
  testPrecondition( x >= x0 );
  testPrecondition( x <= x1 );
  // Make sure the dependent variables are valid
  testPrecondition( y0 > 0.0 );
  testPrecondition( y1 != std::numeric_limits<double>::infinity() );
  
  return y0*pow((y1/y0), (x-x0)/(x1-x0));
}

// Linear-log interpolation function
double linlogInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 )
{
  // Make sure the independent variables are valid
  testPrecondition( x0 < x1 );
  testPrecondition( x0 > 0.0 );
  testPrecondition( x1 != std::numeric_limits<double>::infinity() );
  testPrecondition( x >= x0 );
  testPrecondition( x <= x1 );
  // Make sure the dependent variables are valid
  testPrecondition( y0 != -std::numeric_limits<double>::infinity() );
  testPrecondition( y1 != std::numeric_limits<double>::infinity() );

  return y0 + (y1-y0)*log(x/x0)/log(x1/x0);
}

// Log-log interpolation function
double loglogInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 )
{
  // Make sure the independent variables are valid
  testPrecondition( x0 < x1 );
  testPrecondition( x0 > 0.0 );
  testPrecondition( x1 != std::numeric_limits<double>::infinity() );
  testPrecondition( x >= x0 );
  testPrecondition( x <= x1 );
  // Make sure the dependent variables are valid
  testPrecondition( y0 > 0.0 );
  testPrecondition( y1 != std::numeric_limits<double>::infinity() );
  
  return y0*pow((y1/y0), log(x/x0)/log(x1/x0));
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Interpolation.cpp
//---------------------------------------------------------------------------//
