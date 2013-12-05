//---------------------------------------------------------------------------//
//!
//! \file   BebigI25S06Seed.cpp
//! \author Alex Robinson
//! \brief  Bebig I25.S06 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "BebigI25S06Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string BebigI25S06Seed::seed_name = "BebigI25S06Seed";

// Set the effective seed length (Leff) (cm)
const double BebigI25S06Seed::effective_length = 0.35;

// Set the reference geometry function value
const double BebigI25S06Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  BebigI25S06Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double BebigI25S06Seed::dose_rate_constant = 1.012;

// Set the seed radial dose function
const boost::array<double,BebigI25S06Seed::rdf_points*2> 
BebigI25S06Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.010, 1.018, 1.030, 1.030, 1.020, 1.000, 0.937, 0.857, 0.689, 0.538, 0.409,
   0.313, 0.232, 0.176, 0.134, 0.0957 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> BebigI25S06Seed::cunningham_fit_coeffs =
  {1.31143756876, 1.69270649862, 0.136917284438, 3.81788953408, 1.04242571563};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,BebigI25S06Seed::af_radii> 
BebigI25S06Seed::anisotropy_function_radii = 
  {0.25, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   BebigI25S06Seed::af_angles*(BebigI25S06Seed::af_radii+1)> 
BebigI25S06Seed::anisotropy_function =
  {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.302, 0.352, 0.440, 0.746, 0.886, 0.943, 0.969, 0.984, 0.994, 0.998,
   1.000, // r0 values
   0.429, 0.436, 0.476, 0.686, 0.820, 0.897, 0.946, 0.974, 0.989, 0.998, 
   1.000, // r1 values
   0.512, 0.509, 0.557, 0.721, 0.828, 0.898, 0.942, 0.970, 0.988, 0.998, 
   1.000, // r2 values
   0.579, 0.576, 0.622, 0.757, 0.846, 0.907, 0.947, 0.974, 0.990, 1.000, 
   1.000, // r3 values
   0.610, 0.610, 0.651, 0.771, 0.857, 0.908, 0.944, 0.967, 0.984, 0.994, 
   1.000, // r4 values
   0.631, 0.635, 0.672, 0.785, 0.862, 0.913, 0.947, 0.966, 0.985, 1.000, 
   1.000, // r5 values
   0.649, 0.651, 0.689, 0.790, 0.867, 0.918, 0.949, 0.967, 0.987, 0.993,
   1.000, // r6 values
   0.684, 0.689, 0.721, 0.807, 0.874, 0.912, 0.946, 0.976, 0.994, 0.999,
   1.000 // r7 values
  };

// Constructor
BebigI25S06Seed::BebigI25S06Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType BebigI25S06Seed::getSeedType() const
{
  return BebigI25S06Seed::seed_type;
}

// Return the seed name
std::string BebigI25S06Seed::getSeedName() const
{
  return BebigI25S06Seed::seed_name;
}

// Return the seed strength
double BebigI25S06Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double BebigI25S06Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );
  
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  BebigI25S06Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      BebigI25S06Seed::radial_dose_function.data(),
			      BebigI25S06Seed::rdf_points,
			      BebigI25S06Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  BebigI25S06Seed::anisotropy_function.data(),
			  BebigI25S06Seed::anisotropy_function_radii.data(),
			  BebigI25S06Seed::af_angles,
			  BebigI25S06Seed::af_radii );
						  
  return d_air_kerma_strength*BebigI25S06Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/BebigI25S06Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double BebigI25S06Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BebigI25S06Seed.cpp
//---------------------------------------------------------------------------//
