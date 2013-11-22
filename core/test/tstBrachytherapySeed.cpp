//---------------------------------------------------------------------------//
//!
//! \file   tstBrachytherapySeed.cpp
//! \author Alex Robinson
//! \brief  BrachytherapySeed class unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <math.h>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/array.hpp>

// TPOR Includes
#include "BrachytherapySeed.hpp"

//---------------------------------------------------------------------------//
// Table Data.
//---------------------------------------------------------------------------//
const boost::array<double,16*2> radial_dose_function = 
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.020, 1.022, 1.024, 1.030, 1.020, 1.000, 0.935, 0.861, 0.697, 0.553, 
   0.425, 0.332, 0.241, 0.179, 0.134, 0.0979 // function values
  };

const boost::array<double,5> cunningham_fit_coeffs =
  {1.40382547609, 1.80140354363, 0.0638781288323, 4.07433857915, 1.042478373};

const boost::array<double,6> anisotropy_function_radii = 
  {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};

const boost::array<double,12*7> anisotropy_function =
  {0, 5, 10, 15, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.385, 0.413, 0.531, 0.700, 0.788, 0.892, 0.949, 0.977, 0.989, 0.996, 
   1.000, 1.000, // r0 function values
   0.420, 0.472, 0.584, 0.700, 0.789, 0.888, 0.948, 0.973, 0.985, 0.992, 
   0.998, 1.000, // r1 function values
   0.493, 0.546, 0.630, 0.719, 0.793, 0.888, 0.944, 0.967, 0.983, 0.990, 
   0.998, 1.000, // r2 function values
   0.533, 0.586, 0.660, 0.738, 0.805, 0.891, 0.944, 0.967, 0.983, 0.990, 
   0.998, 1.000, // r3 function values
   0.569, 0.613, 0.681, 0.749, 0.810, 0.892, 0.944, 0.967, 0.983, 0.990, 
   0.998, 1.000, // r4 function values
   0.589, 0.631, 0.697, 0.758, 0.814, 0.892, 0.944, 0.967, 0.983, 0.990,
   0.998, 1.000  // r5 function values
  };

//---------------------------------------------------------------------------//
// Testing Structs.
//---------------------------------------------------------------------------//
class TestBrachytherapySeed : public TPOR::BrachytherapySeed
{
public:
  TestBrachytherapySeed()
    : TPOR::BrachytherapySeed()
  { /* ... */ }
  
  virtual ~TestBrachytherapySeed()
  { /* ... */ }

  using TPOR::BrachytherapySeed::calculateRadius;
  using TPOR::BrachytherapySeed::calculatePolarAngle;
  using TPOR::BrachytherapySeed::convertAngleToDegrees;
  using TPOR::BrachytherapySeed::calculateAngleSubtendedBySeed;
  using TPOR::BrachytherapySeed::evaluateGeometryFunction;
  using TPOR::BrachytherapySeed::evaluateRadialDoseFunction;
  using TPOR::BrachytherapySeed::evaluateAnisotropyFunction;
};

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that a radius can be calculated from an (x,y,z) coordinate
BOOST_AUTO_TEST_CASE( calculateRadius )
{
  double x = 1.0, y = 1.0, z = 1.0;
  
  double r = TestBrachytherapySeed::calculateRadius( x, y, z );

  BOOST_CHECK_CLOSE( r, sqrt(x*x+y*y+z*z), 1e-12 );

  x = -1.0;
  y = -1.0;
  z = 1.0;
  
  r = TestBrachytherapySeed::calculateRadius( x, y, z );

  BOOST_CHECK_CLOSE( r, sqrt(x*x+y*y+z*z), 1e-12 );

  x = 1.0;
  y = 1.0;
  z = -1.0;

  r = TestBrachytherapySeed::calculateRadius( x, y, z );

  BOOST_CHECK_CLOSE( r, sqrt(x*x+y*y+z*z), 1e-12 );

  x = -1.0;
  y = -1.0;
  z = -1.0;

  r = TestBrachytherapySeed::calculateRadius( x, y, z );

  BOOST_CHECK_CLOSE( r, sqrt(x*x+y*y+z*z), 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the polar angle in radians can be calculated from an (r,z)
// coordinate
BOOST_AUTO_TEST_CASE( calculatePolarAngle )
{
  double r = 1.0, z = 0.0;

  double theta = TestBrachytherapySeed::calculatePolarAngle( r, z );

  BOOST_CHECK_CLOSE( theta, acos(-1.0)/2, 1e-12 );

  r = 1.0;
  z = sqrt(2.0)/2;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );

  BOOST_CHECK_CLOSE( theta, acos(-1.0)/4, 1e-12 );

  r = 1.0;
  z = 1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );

  BOOST_CHECK_CLOSE( theta, 0.0, 1e-12 );

  r = 1.0;
  z = -1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );

  BOOST_CHECK_CLOSE( theta, 0.0, 1e-12 );

  r = 1.0;
  z = -sqrt(2.0)/2;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );

  BOOST_CHECK_CLOSE( theta, acos(-1.0)/4, 1e-12 );

  r = 0.0;
  z = 0.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );

  BOOST_CHECK_CLOSE( theta, acos(0.0), 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the polar angle in radians can be converted to degrees
BOOST_AUTO_TEST_CASE( convertAngleToDegrees )
{
  double r = 1.0, z = 0.0;

  double theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  double theta_degrees = TestBrachytherapySeed::convertAngleToDegrees( theta );
  
  BOOST_CHECK_CLOSE( theta_degrees, 90.0, 1e-12 );

  r = 1.0;
  z = sqrt(2.0)/2;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  theta_degrees = TestBrachytherapySeed::convertAngleToDegrees( theta );

  BOOST_CHECK_CLOSE( theta_degrees, 45.0, 1e-12 );

  r = 1.0;
  z = 1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  theta_degrees = TestBrachytherapySeed::convertAngleToDegrees( theta );

  BOOST_CHECK_CLOSE( theta_degrees, 0.0, 1e-12 );

  r = 1.0;
  z = -1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  theta_degrees = TestBrachytherapySeed::convertAngleToDegrees( theta );

  BOOST_CHECK_CLOSE( theta_degrees, 0.0, 1e-12 );

  r = 1.0;
  z = -sqrt(2.0)/2;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  theta_degrees = TestBrachytherapySeed::convertAngleToDegrees( theta );

  BOOST_CHECK_CLOSE( theta_degrees, 45.0, 1e-12 );

  r = 0.0;
  z = 0.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  theta_degrees = TestBrachytherapySeed::convertAngleToDegrees( theta );

  BOOST_CHECK_CLOSE( theta_degrees, 90.0, 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the angle in radians subtended by the tips of the seed can
// be calculated
BOOST_AUTO_TEST_CASE( calculateAngleSubtendedBySeed )
{
  double seed_length = 0.3;
  double r = 1.0, z = 0.0;

  double theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  double beta = TestBrachytherapySeed::calculateAngleSubtendedBySeed( 
								 r,
								 theta,
								 seed_length );
  BOOST_CHECK_CLOSE( beta, 0.29777989521899, 1e-9 );

  r = sqrt(2.0);
  z = 1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  beta = TestBrachytherapySeed::calculateAngleSubtendedBySeed( r,
							       theta,
							       seed_length );

  BOOST_CHECK_CLOSE( beta, 0.15055867288369, 1e-9 );

  r = 1.0;
  z = 1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  beta = TestBrachytherapySeed::calculateAngleSubtendedBySeed( r,
							       theta,
							       seed_length );

  BOOST_CHECK_CLOSE( beta, 0.0, 1e-9 );

  r = 1.0;
  z = -1.0;
  
  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  beta = TestBrachytherapySeed::calculateAngleSubtendedBySeed( r,
							       theta,
							       seed_length );

  BOOST_CHECK_CLOSE( beta, 0.0, 1e-9 );

  r = sqrt(2.0);
  z = -1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  beta = TestBrachytherapySeed::calculateAngleSubtendedBySeed( r,
							       theta,
							       seed_length );

  BOOST_CHECK_CLOSE( beta, 0.15055867288369, 1e-9 );

  r = 0.0;
  z = 0.0;
  
  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  beta = TestBrachytherapySeed::calculateAngleSubtendedBySeed( r,
							       theta,
							       seed_length );

  BOOST_CHECK_CLOSE( beta, acos(-1.0), 1e-9 );
}

//---------------------------------------------------------------------------//
// Check that the geometry function can be evaluated.
BOOST_AUTO_TEST_CASE( evaluateGemoetryFunction )
{
  double seed_length = 0.3;
  double r = 1.0, z = 0.0;
  
  double theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  double geometry_function_val = 
    TestBrachytherapySeed::evaluateGeometryFunction( r, theta, seed_length );

  BOOST_CHECK_CLOSE( geometry_function_val, 0.9925996507299667, 1e-9 );

  r = sqrt(2.0);
  z = 1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  geometry_function_val = 
    TestBrachytherapySeed::evaluateGeometryFunction( r, theta, seed_length );

  BOOST_CHECK_CLOSE( geometry_function_val, 0.5018622429456333, 1e-9 );

  r = 1.0;
  z = 1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  geometry_function_val = 
    TestBrachytherapySeed::evaluateGeometryFunction( r, theta, seed_length );

  BOOST_CHECK_CLOSE( geometry_function_val, 1.0230179028132993, 1e-9 );

  r = 1.0;
  z = -1.0;
  
  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  geometry_function_val = 
    TestBrachytherapySeed::evaluateGeometryFunction( r, theta, seed_length );

  BOOST_CHECK_CLOSE( geometry_function_val, 1.0230179028132993, 1e-9 );

  r = sqrt(2.0);
  z = -1.0;

  theta = TestBrachytherapySeed::calculatePolarAngle( r, z );
  geometry_function_val = 
    TestBrachytherapySeed::evaluateGeometryFunction( r, theta, seed_length );

  BOOST_CHECK_CLOSE( geometry_function_val, 0.5018622429456333, 1e-9 );
}

//---------------------------------------------------------------------------//
// Check that the radial dose function can be evaluated
BOOST_AUTO_TEST_CASE( evaluateRadialDoseFunction )
{
  double r = 1.0;

  double radial_dose_function_value = 
    TestBrachytherapySeed::evaluateRadialDoseFunction( 
					        r,
						radial_dose_function.data(),
						radial_dose_function.size()/2,
						cunningham_fit_coeffs.data() );

  BOOST_CHECK_EQUAL( radial_dose_function_value, 1.000 );
  
  r = 0.05;
  
  radial_dose_function_value = 
    TestBrachytherapySeed::evaluateRadialDoseFunction( 
					        r,
						radial_dose_function.data(),
						radial_dose_function.size()/2,
						cunningham_fit_coeffs.data() );

  BOOST_CHECK_EQUAL( radial_dose_function_value, 1.020 );

  r = 9.5;

  radial_dose_function_value = 
    TestBrachytherapySeed::evaluateRadialDoseFunction( 
					        r,
						radial_dose_function.data(),
						radial_dose_function.size()/2,
						cunningham_fit_coeffs.data() );

  BOOST_CHECK_CLOSE( radial_dose_function_value, 0.11453645707808498, 1e-9 );

  r = 20;

  radial_dose_function_value  =
    TestBrachytherapySeed::evaluateRadialDoseFunction( 
					        r,
						radial_dose_function.data(),
						radial_dose_function.size()/2,
						cunningham_fit_coeffs.data() );

  BOOST_CHECK_CLOSE( radial_dose_function_value, 0.0018512944161126182, 1e-6 );
}

//---------------------------------------------------------------------------//
// Check that the anisotropy function can be evaluated
BOOST_AUTO_TEST_CASE( evaluateAnisotropyFunction )
{
  double r = 1.0, theta = 10.0*acos(-1.0)/180.0;

  double anisotropy_function_value = 
    TestBrachytherapySeed::evaluateAnisotropyFunction(
					    r,
					    theta,
					    anisotropy_function.data(),
					    anisotropy_function_radii.data(),
					    anisotropy_function.size()/7,
					    anisotropy_function_radii.size() );

  BOOST_CHECK_EQUAL( anisotropy_function_value, 0.584 );

  r = 0.25, theta = 2.5*acos(-1.0)/180.0;

  anisotropy_function_value = 
    TestBrachytherapySeed::evaluateAnisotropyFunction(
					    r,
					    theta,
					    anisotropy_function.data(),
					    anisotropy_function_radii.data(),
					    anisotropy_function.size()/7,
					    anisotropy_function_radii.size() );

  BOOST_CHECK_CLOSE( anisotropy_function_value, 0.399, 1e-9 );
  
  r = 4.5, theta = 65*acos(-1.0)/180;

  anisotropy_function_value = 
    TestBrachytherapySeed::evaluateAnisotropyFunction(
					    r,
					    theta,
					    anisotropy_function.data(),
					    anisotropy_function_radii.data(),
					    anisotropy_function.size()/7,
					    anisotropy_function_radii.size() );

  BOOST_CHECK_CLOSE( anisotropy_function_value, 0.9864999999999999, 1e-9 );

  r = 6.0, theta = 40*acos(-1.0)/180;

  anisotropy_function_value = 
    TestBrachytherapySeed::evaluateAnisotropyFunction(
					    r,
					    theta,
					    anisotropy_function.data(),
					    anisotropy_function_radii.data(),
					    anisotropy_function.size()/7,
					    anisotropy_function_radii.size() );

  BOOST_CHECK_CLOSE( anisotropy_function_value, 0.944, 1e-9 );
}

//---------------------------------------------------------------------------//
// end tstBrachytherapySeed.cpp
//---------------------------------------------------------------------------//
