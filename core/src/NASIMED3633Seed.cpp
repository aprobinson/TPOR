//---------------------------------------------------------------------------//
//!
//! \file   NASIMED3633Seed.cpp
//! \author Alex Robinson
//! \brief  NASI MED 3633 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "NASIMED3633Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string NASIMED3633Seed::seed_name = "NASIMED3633Seed";

// Set the effective seed length (Leff) (cm)
const double NASIMED3633Seed::effective_length = 0.42;

// Set the reference geometry function value
const double NASIMED3633Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  NASIMED3633Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double NASIMED3633Seed::dose_rate_constant = 0.688;

// Set the seed radial dose function
const boost::array<double,NASIMED3633Seed::rdf_points*2> 
NASIMED3633Seed::radial_dose_function =
  {0.25, 0.3, 0.4, 0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 
   7.0, // radii
   1.331, 1.322, 1.286, 1.243, 1.125, 1.000, 0.770, 0.583, 0.438, 0.325, 
   0.241, 0.177, 0.098, 0.053, 0.028 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> NASIMED3633Seed::cunningham_fit_coeffs =
  {1.53497160903, 2.21551489129, 0.814756253265, 0.984982781594, 
   1.56831411678};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,NASIMED3633Seed::af_radii> 
NASIMED3633Seed::anisotropy_function_radii = 
  {0.25, 0.5, 1.0, 2.0, 5.0, 10.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   NASIMED3633Seed::af_angles*(NASIMED3633Seed::af_radii+1)> 
NASIMED3633Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   1.024, 0.888, 0.850, 0.892, 0.931, 0.952, 0.971, 0.995, 1.003,
   1.000, // r0 values
   0.667, 0.581, 0.627, 0.748, 0.838, 0.897, 0.942, 0.976, 0.994, 
   1.000, // r1 values
   0.566, 0.536, 0.603, 0.729, 0.821, 0.890, 0.942, 0.974, 0.997,  
   1.000, // r2 values
   0.589, 0.536, 0.614, 0.734, 0.824, 0.891, 0.940, 0.973, 0.994, 
   1.000, // r3 values
   0.609, 0.569, 0.652, 0.756, 0.837, 0.901, 0.948, 0.980, 1.000, 
   1.000, // r4 values
   0.733, 0.641, 0.716, 0.786, 0.853, 0.905, 0.939, 0.974, 0.986,
   1.000  // r5 values
  };

// Constructor
NASIMED3633Seed::NASIMED3633Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )

{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType NASIMED3633Seed::getSeedType() const
{
  return NASIMED3633Seed::seed_type;
}

// Return the seed name
std::string NASIMED3633Seed::getSeedName() const
{
  return NASIMED3633Seed::seed_name;
}

// Return the seed strength
double NASIMED3633Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double NASIMED3633Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );

  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  NASIMED3633Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      NASIMED3633Seed::radial_dose_function.data(),
			      NASIMED3633Seed::rdf_points,
			      NASIMED3633Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  NASIMED3633Seed::anisotropy_function.data(),
			  NASIMED3633Seed::anisotropy_function_radii.data(),
			  NASIMED3633Seed::af_angles,
			  NASIMED3633Seed::af_radii );
						  
  return d_air_kerma_strength*NASIMED3633Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/NASIMED3633Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double NASIMED3633Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::pd103_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end NASIMED3633Seed.cpp
//---------------------------------------------------------------------------//
