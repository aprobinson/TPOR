//---------------------------------------------------------------------------//
//!
//! \file   Amersham6702Seed.cpp
//! \author Alex Robinson
//! \brief  Amersham 6702 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Amersham6702Seed.hpp"
#include "ContractException.hpp"
#include "BinarySearch.hpp"
#include "Interpolation.hpp"

namespace TPOR{

// Set the seed decay constant (1/h) - I125 half-life = 59.4 days
const double Amersham6702Seed::decay_constant = log(2)/(59.4*24);

// Set the effective seed length (Leff) (cm)
const double Amersham6702Seed::effective_length = 0.30;

// Set the seed dose rate constant (1/cm^2)
const double Amersham6702Seed::dose_rate_constant = 1.036;

// Set the seed radial dose function
const double Amersham6702Seed::radial_dose_function[][16] =
  {{0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
    9.0, 10.0},
   {1.020, 1.022, 1.024, 1.030, 1.020, 1.000, 0.935, 0.861, 0.697, 0.553, 
    0.425, 0.332, 0.241, 0.179, 0.134, 0.0979}};

// Set the Cunningham radial dose function fit coefficients
const double Amersham6702Seed::radial_dose_fit_coeffs[] =
  {1.40382547609, 1.80140354363, 0.0638781288323, 4.07433857915, 1.042478373};

// Set the 2D anisotropy function radii (cm)
const double Amersham6702Seed::two_dim_anisotropy_function_radii[] = 
  {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};

// Set the 2D anisotropy function (theta = degrees)
const double two_dim_anisotropy_function[][11] =
  {{0, 5, 10, 15, 20, 30, 40, 50, 60, 70, 80},
   {0.385, 0.413, 0.531, 0.700, 0.788, 0.892, 0.949, 0.977, 0.989, 0.996, 
    1.000},
   {0.420, 0.472, 0.584, 0.700, 0.789, 0.888, 0.948, 0.973, 0.985, 0.992, 
    0.998},
   {0.493, 0.546, 0.630, 0.719, 0.793, 0.888, 0.944, 0.967, 0.983, 0.990, 
    0.998},
   {0.533, 0.586, 0.660, 0.738, 0.805, 0.891, 0.944, 0.967, 0.983, 0.990, 
    0.998},
   {0.569, 0.613, 0.681, 0.749, 0.810, 0.892, 0.944, 0.967, 0.983, 0.990, 
    0.998},
   {0.589, 0.631, 0.697, 0.758, 0.814, 0.892, 0.944, 0.967, 0.983, 0.990,
    0.998}};

// Constructor
Amersham6702Seed::Amersham6702Seed( const double air_kerma_strength )
: d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
SeedType Amersham6702Seed::getSeedType() const
{
  return Amersham6702Seed::seed_type;
}

// Return the dose rate at a given point
double Amersham6702Seed::getDoseRate( const double x, 
				      const double y, 
				      const double z ) const
{
  double radius = calculateRadius( x, y );
  double theta = calculatePolarAngleDegrees( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  z,
					  Amersham6702Seed::effective_length );

  // Evaluate the geometry function at the reference point
  double geometry_function_ref_value = evaluateGeometryFunction(
					  1.0,
					  0.0,
					  Amersham6702Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    Amersham6702Seed::evaluateRadialDoseFunction( radius );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    Amersham6703Seed::evaluateAnisotropyFunction( radius, theta );

  return d_air_kerma_strength*Amersham6702Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/geometry_function_ref_value;
}

// Return the total dose at time = infinity at a given point
double Amersham6702Seed::getTotalDose( const double x, 
				       const double y, 
				       const double z ) const
{
  double dose_rate = getDoseRate( x, y, z );
  
  
}

// Evaluate the radial dose function
double Amersham6702Seed::evaluateRadialDoseFunction( const double r ) const
{
  
}

// Evaluate the 2D anisotropy function
/*! \details theta must be in units of degrees
 */
double Amersham6702Seed::evaluateAnisotropyFunction( const double r, 
						     const double theta ) const
{

}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Amersham6702Seed.cpp
//---------------------------------------------------------------------------//

