//---------------------------------------------------------------------------//
//!
//! \file   Best2335Seed.cpp
//! \author Alex Robinson
//! \brief  Best 2335 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Best2335Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string Best2335Seed::seed_name = "Best2335Seed";

// Set the effective seed length (Leff) (cm)
const double Best2335Seed::effective_length = 0.455;

// Set the reference geometry function value
const double Best2335Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  Best2335Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Best2335Seed::dose_rate_constant = 0.685;

// Set the seed radial dose function
const boost::array<double,Best2335Seed::rdf_points*2> 
Best2335Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   0.826, 1.066, 1.236, 1.307, 1.128, 1.000, 0.742, 0.533, 0.296, 0.158, 
   0.0920, 0.0529, 0.0309, 0.0180, 0.0105, 0.0062 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Best2335Seed::cunningham_fit_coeffs =
  {5.31875967529, 6.12369837261, 0.0774736930699, 1.83681612571, 
   1.11047876885};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Best2335Seed::af_radii> 
Best2335Seed::anisotropy_function_radii = 
  {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   Best2335Seed::af_angles*(Best2335Seed::af_radii+1)> 
Best2335Seed::anisotropy_function =
  {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, // theta values
   0.797, 0.801, 0.790, 0.675, 0.608, 0.675, 0.681, 0.725, 0.762, 0.792, 0.885,
   0.915, 0.932, 0.941, 1.000, // r0 values
   0.690, 0.696, 0.690, 0.613, 0.591, 0.639, 0.660, 0.693, 0.736, 0.807, 0.880,
   0.929, 0.960, 0.975, 1.000, // r1 values
   0.674, 0.683, 0.673, 0.608, 0.596, 0.637, 0.679, 0.705, 0.750, 0.846, 0.885,
   0.944, 0.972, 0.986, 1.000, // r2 values
   0.672, 0.669, 0.675, 0.604, 0.601, 0.659, 0.694, 0.721, 0.747, 0.847, 0.887,
   0.936, 0.965, 0.985, 1.000, // r3 values
   0.663, 0.666, 0.665, 0.626, 0.616, 0.653, 0.694, 0.703, 0.741, 0.866, 0.909,
   0.965, 0.975, 0.999, 1.000, // r4 values
   0.675, 0.679, 0.690, 0.620, 0.647, 0.706, 0.717, 0.730, 0.775, 0.876, 0.907,
   1.001, 1.014, 1.017, 1.000, // r5 values
   0.630, 0.645, 0.644, 0.581, 0.595, 0.651, 0.672, 0.687, 0.720, 0.804, 0.835,
   0.912, 0.916, 0.915, 1.000  // r6 values
  };

// Constructor
Best2335Seed::Best2335Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )

{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Best2335Seed::getSeedType() const
{
  return Best2335Seed::seed_type;
}

// Return the seed name
std::string Best2335Seed::getSeedName() const
{
  return Best2335Seed::seed_name;
}

// Return the seed strength
double Best2335Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double Best2335Seed::getDoseRate( const double x, 
				  const double y, 
				  const double z ) const
{
  double radius = calculateRadius( x, y, z );

  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  Best2335Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      Best2335Seed::radial_dose_function.data(),
			      Best2335Seed::rdf_points,
			      Best2335Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  Best2335Seed::anisotropy_function.data(),
			  Best2335Seed::anisotropy_function_radii.data(),
			  Best2335Seed::af_angles,
			  Best2335Seed::af_radii );
						  
  return d_air_kerma_strength*Best2335Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Best2335Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Best2335Seed::getTotalDose( const double x, 
				   const double y, 
				   const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::pd103_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Best2335Seed.cpp
//---------------------------------------------------------------------------//
