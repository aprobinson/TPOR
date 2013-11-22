//---------------------------------------------------------------------------//
//!
//! \file   TheragenicsAgX100Seed.cpp
//! \author Alex Robinson
//! \brief  Theragenics AgX100 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "TheragenicsAgX100Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string TheragenicsAgX100Seed::seed_name = "TheragenicsAgX100Seed";

// Set the effective seed length (Leff) (cm)
const double TheragenicsAgX100Seed::effective_length = 0.35;

// Set the reference geometry function value
const double TheragenicsAgX100Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
				     1.0,
				     acos(0.0),
				     TheragenicsAgX100Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double TheragenicsAgX100Seed::dose_rate_constant = 0.943;

// Set the seed radial dose function
const boost::array<double,TheragenicsAgX100Seed::rdf_points*2> 
TheragenicsAgX100Seed::radial_dose_function =
  {0.1, 0.15, 0.2, 0.25, 0.3, 0.5, 0.75, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 6, 
   7, 8, 9, 10, // radii
   1.066, 1.086, 1.096, 1.098, 1.097, 1.076, 1.042, 1.000, 0.908, 0.813, 0.720,
   0.633, 0.553, 0.482, 0.418, 0.361, 0.269, 0.199, 0.147, 0.108, 
   0.079 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> TheragenicsAgX100Seed::cunningham_fit_coeffs =
  {1.16141862676, 1.54434887695, 0.234373865415, 2.96834405993, 1.14538191576};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,TheragenicsAgX100Seed::af_radii> 
TheragenicsAgX100Seed::anisotropy_function_radii = 
  {0.25, 0.5, 1.0, 2.0, 3.0, 5.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
	  TheragenicsAgX100Seed::af_angles*(TheragenicsAgX100Seed::af_radii+1)>
TheragenicsAgX100Seed::anisotropy_function =
  { 0, 1, 2, 3, 5, 7, 10, 12, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70,
    75, 80, 85, 90, // theta values
    0.205, 0.209, 0.211, 0.214, 0.220, 0.248, 0.410, 0.484, 0.617, 0.790, 
    0.902, 0.974, 1.015, 1.016, 0.963, 0.950, 0.964, 0.974, 0.983, 0.990, 
    0.995, 0.997, 0.999, 1.000, // r0 values
    0.217, 0.215, 0.215, 0.218, 0.314, 0.342, 0.404, 0.459, 0.538, 0.652,
    0.742, 0.813, 0.870, 0.916, 0.954, 0.986, 1.012, 1.028, 1.028, 0.987, 
    0.991, 0.995, 0.999, 1.000, // r1 values
    0.294, 0.295, 0.313, 0.377, 0.400, 0.428, 0.490, 0.536, 0.600, 0.688, 
    0.760, 0.817, 0.864, 0.904, 0.936, 0.963, 0.987, 1.007, 1.024, 1.036, 
    1.035, 1.002, 1.000, 1.000, // r2 values
    0.400, 0.413, 0.469, 0.483, 0.493, 0.522, 0.577, 0.613, 0.665, 0.736, 
    0.793, 0.841, 0.878, 0.912, 0.937, 0.961, 0.981, 0.999, 1.012, 1.025, 
    1.032, 1.031, 1.004, 1.000, // r3 values
    0.462, 0.491, 0.534, 0.534, 0.548, 0.572, 0.624, 0.654, 0.699, 0.761, 
    0.812, 0.852, 0.887, 0.915, 0.939, 0.961, 0.978, 0.994, 1.006, 1.017, 
    1.026, 1.028, 1.017, 1.000, // r4 values
    0.543, 0.575, 0.598, 0.598, 0.610, 0.634, 0.674, 0.698, 0.736, 0.788, 
    0.831, 0.865, 0.896, 0.920, 0.942, 0.961, 0.976, 0.989, 1.000, 1.011, 
    1.018, 1.022, 1.016, 1.000, // r5 values
    0.586, 0.616, 0.640, 0.636, 0.648, 0.663, 0.702, 0.723, 0.757, 0.803,
    0.845, 0.874, 0.901, 0.923, 0.942, 0.961, 0.974, 0.988, 0.998, 1.009,
    1.015, 1.017, 1.015, 1.000  // r6 values
  };

// Constructor
TheragenicsAgX100Seed::TheragenicsAgX100Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType TheragenicsAgX100Seed::getSeedType() const
{
  return TheragenicsAgX100Seed::seed_type;
}

// Return the seed name
std::string TheragenicsAgX100Seed::getSeedName() const
{
  return TheragenicsAgX100Seed::seed_name;
}

// Return the seed strength
double TheragenicsAgX100Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double TheragenicsAgX100Seed::getDoseRate( const double x, 
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
				     TheragenicsAgX100Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			 radius,
			 TheragenicsAgX100Seed::radial_dose_function.data(),
			 TheragenicsAgX100Seed::rdf_points,
			 TheragenicsAgX100Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
		       radius, 
		       theta,
		       TheragenicsAgX100Seed::anisotropy_function.data(),
		       TheragenicsAgX100Seed::anisotropy_function_radii.data(),
		       TheragenicsAgX100Seed::af_angles,
		       TheragenicsAgX100Seed::af_radii );
						  
  return d_air_kerma_strength*TheragenicsAgX100Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/TheragenicsAgX100Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double TheragenicsAgX100Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end TheragenicsAgX100Seed.cpp
//---------------------------------------------------------------------------//
