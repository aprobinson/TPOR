//---------------------------------------------------------------------------//
//!
//! \file   SourceTechSTM1251Seed.hpp
//! \author Alex Robinson
//! \brief  Source Tech STM1251 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "SourceTechSTM1251Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string SourceTechSTM1251Seed::seed_name = "SourceTechSTM1251Seed";

// Set the effective seed length (Leff) (cm)
const double SourceTechSTM1251Seed::effective_length = 0.381;

// Set the reference geometry function value
const double SourceTechSTM1251Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
				     1.0,
				     acos(0.0),
				     SourceTechSTM1251Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double SourceTechSTM1251Seed::dose_rate_constant = 1.018;

// Set the seed radial dose function
const boost::array<double,SourceTechSTM1251Seed::rdf_points*2> 
SourceTechSTM1251Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   0.941, 0.972, 1.013, 1.033, 1.022, 1.000, 0.937, 0.856, 0.691, 0.540, 0.415,
   0.314, 0.236, 0.176, 0.131, 0.0969 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> SourceTechSTM1251Seed::cunningham_fit_coeffs =
  {1.11978980982, 1.50052020998, 0.352885119523, 2.60608715319, 1.17257157464};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,SourceTechSTM1251Seed::af_radii> 
SourceTechSTM1251Seed::anisotropy_function_radii = 
  {0.25, 0.5, 1.0, 2.0, 3.0, 5.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
	  SourceTechSTM1251Seed::af_angles*(SourceTechSTM1251Seed::af_radii+1)>
SourceTechSTM1251Seed::anisotropy_function =
  {0, 2, 5, 7, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.863, 0.865, 0.784, 0.861, 0.778, 0.889, 0.949, 0.979, 0.959, 0.980, 0.989,
   0.994, 1.000, // r0 values
   0.524, 0.489, 0.668, 0.588, 0.562, 0.688, 0.816, 0.898, 0.956, 0.988, 0.973,
   0.994, 1.000, // r1 values
   0.423, 0.616, 0.599, 0.575, 0.579, 0.698, 0.808, 0.888, 0.943, 0.982, 1.005,
   0.989, 1.000, // r2 values
   0.453, 0.701, 0.611, 0.603, 0.617, 0.722, 0.819, 0.891, 0.941, 0.980, 1.002,
   1.015, 1.000, // r3 values
   0.500, 0.702, 0.637, 0.632, 0.649, 0.750, 0.841, 0.903, 0.950, 0.985, 1.011,
   1.018, 1.000, // r4 values
   0.564, 0.706, 0.657, 0.655, 0.672, 0.761, 0.838, 0.901, 0.941, 0.973, 0.995,
   1.003, 1.000, // r5 values
   0.607, 0.720, 0.682, 0.682, 0.700, 0.781, 0.845, 0.912, 0.945, 0.982, 0.998,
   1.011, 1.000  // r6 values
  };

// Constructor
SourceTechSTM1251Seed::SourceTechSTM1251Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType SourceTechSTM1251Seed::getSeedType() const
{
  return SourceTechSTM1251Seed::seed_type;
}

// Return the seed name
std::string SourceTechSTM1251Seed::getSeedName() const
{
  return SourceTechSTM1251Seed::seed_name;
}

// Return the seed strength
double SourceTechSTM1251Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double SourceTechSTM1251Seed::getDoseRate( const double x, 
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
				     SourceTechSTM1251Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			 radius,
			 SourceTechSTM1251Seed::radial_dose_function.data(),
			 SourceTechSTM1251Seed::rdf_points,
			 SourceTechSTM1251Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
		       radius, 
		       theta,
		       SourceTechSTM1251Seed::anisotropy_function.data(),
		       SourceTechSTM1251Seed::anisotropy_function_radii.data(),
		       SourceTechSTM1251Seed::af_angles,
		       SourceTechSTM1251Seed::af_radii );
						  
  return d_air_kerma_strength*SourceTechSTM1251Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/SourceTechSTM1251Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double SourceTechSTM1251Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end SourceTechSTM1251Seed.cpp
//---------------------------------------------------------------------------//
