//---------------------------------------------------------------------------//
//!
//! \file   Best2301Seed.cpp
//! \author Alex Robinson
//! \brief  Best 2301 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Best2301Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string Best2301Seed::seed_name = "Best2301Seed";

// Set the effective seed length (Leff) (cm)
const double Best2301Seed::effective_length = 0.40;

// Set the reference geometry function value
const double Best2301Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  Best2301Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Best2301Seed::dose_rate_constant = 1.018;

// Set the seed radial dose function
const boost::array<double,Best2301Seed::rdf_points*2> 
Best2301Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.033, 1.029, 1.027, 1.028, 1.030, 1.000, 0.938, 0.866, 0.707, 0.555, 0.427,
   0.320, 0.248, 0.187,	0.142, 0.103 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Best2301Seed::cunningham_fit_coeffs =
  {1.06522357779, 1.42597081747, 0.245016744334, 3.72139410655, 1.06095097163};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Best2301Seed::af_radii> 
Best2301Seed::anisotropy_function_radii = 
  {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   Best2301Seed::af_angles*(Best2301Seed::af_radii+1)> 
Best2301Seed::anisotropy_function =
  {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.367, 0.724, 0.653, 0.785, 0.900, 0.982, 1.014, 1.030, 1.036, 1.010,
   1.000, // r0 values
   0.454, 0.720, 0.671, 0.794, 0.890, 0.954, 0.992, 1.010, 1.026, 1.030,
   1.000, // r1 values
   0.922, 0.726, 0.699, 0.809, 0.885, 0.947, 0.985, 1.009, 1.016, 1.019,
   1.000, // r2 values
   0.902, 0.738, 0.727, 0.814, 0.892, 0.939, 0.991, 1.007, 1.023, 1.017, 
   1.000, // r3 values
   0.894, 0.753, 0.732, 0.825, 0.899, 0.943, 0.997, 1.010, 1.011, 1.010, 
   1.000, // r4 values
   0.893, 0.771, 0.764, 0.852, 0.915, 0.976, 0.989, 1.019, 1.035, 1.020, 
   1.000, // r5 values
   0.858, 0.800, 0.782, 0.821, 0.873, 0.937, 0.961, 1.002, 1.010, 1.005,
   1.000  // r6 values
  };

// Constructor
Best2301Seed::Best2301Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )

{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Best2301Seed::getSeedType() const
{
  return Best2301Seed::seed_type;
}

// Return the seed name
std::string Best2301Seed::getSeedName() const
{
  return Best2301Seed::seed_name;
}

// Return the seed strength
double Best2301Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double Best2301Seed::getDoseRate( const double x, 
				  const double y, 
				  const double z ) const
{
  double radius = calculateRadius( x, y, z );

  // Don't evaluate dose rates inside of the seed
  if( radius < 0.04 )
    radius = 0.04; 
  
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  Best2301Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      Best2301Seed::radial_dose_function.data(),
			      Best2301Seed::rdf_points,
			      Best2301Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  Best2301Seed::anisotropy_function.data(),
			  Best2301Seed::anisotropy_function_radii.data(),
			  Best2301Seed::af_angles,
			  Best2301Seed::af_radii );
						  
  return d_air_kerma_strength*Best2301Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Best2301Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Best2301Seed::getTotalDose( const double x, 
				   const double y, 
				   const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Best2301Seed.cpp
//---------------------------------------------------------------------------//
