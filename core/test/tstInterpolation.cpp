//---------------------------------------------------------------------------//
//!
//! \file   tstInterpolation.cpp
//! \author Alex Robinson
//! \brief  Interpolation function unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

// FACEMC Includes
#include "Interpolation.hpp"

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the linear-linear interpolation function returns the correct
// value.
BOOST_AUTO_TEST_CASE( linlinInterp )
{
  double x0 = 0.0, x1 = 1.0, x = 0.5;
  double y0 = 5.0, y1 = 10.0;

  double y = TPOR::linlinInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_EQUAL( y, 7.5 );

  x = 0.0;
    
  y = TPOR::linlinInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_EQUAL( y, 5.0 );

  x = 1.0;

  y = TPOR::linlinInterp( x0, x1, x, y0, y1 );
  
  BOOST_REQUIRE_EQUAL( y, 10.0 );
}

//---------------------------------------------------------------------------//
// Check that the log-linear interpolation function returns the correct
// value.
BOOST_AUTO_TEST_CASE( loglinInterp )
{
  double x0 = 0.0, x1 = 1.0, x = 0.5;
  double y0 = 0.1, y1 = 10.0;

  double y = TPOR::loglinInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 1.0, 1e-12 );

  x = 0.0;

  y = TPOR::loglinInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 0.1, 1e-12 );

  x = 1.0;

  y = TPOR::loglinInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 10.0, 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the linear-log interpolation function returns the correct
// value.
BOOST_AUTO_TEST_CASE( linlogInterp )
{
  double x0 = 0.1, x1= 10.0, x = 1.0;
  double y0 = 0.0, y1 = 1.0;

  double y = TPOR::linlogInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 0.5, 1e-12 );

  x = 0.1;

  y = TPOR::linlogInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 0.0, 1e-12 );

  x = 10.0;

  y = TPOR::linlogInterp( x0, x1, x, y0, y1 );
  
  BOOST_REQUIRE_CLOSE( y, 1.0, 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the log-log interpolation function returns the correct
// value.
BOOST_AUTO_TEST_CASE( logLogInterp )
{
  double x0 = 0.1, x1 = 10.0, x = 1.0;
  double y0 = 10.0, y1 = 1000.0;

  double y = TPOR::loglogInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 100.0, 1e-12 );

  x = 0.1;

  y = TPOR::loglogInterp( x0, x1, x, y0, y1 );

  BOOST_REQUIRE_CLOSE( y, 10.0, 1e-12 );

  x = 10.0;

  y = TPOR::loglogInterp( x0, x1, x, y0, y1 );
  
  BOOST_REQUIRE_CLOSE( y, 1000.0, 1e-12 );
}

//---------------------------------------------------------------------------//
// end tstInterpolation.cpp
//---------------------------------------------------------------------------//

