//---------------------------------------------------------------------------//
//!
//! \file   MBISL125SH125Seed.hpp
//! \author Alex Robinson
//! \brief  Mills Biopharmaceuticals SL-125/SH-125 brachytherapy seed class 
//!         definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "MBISL125SH125Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string MBISL125SH125Seed::seed_name = "MBISL125SH125Seed";

// Set the effective seed length (Leff) (cm)
const double MBISL125SH125Seed::effective_length = 0.3;

// Set the reference geometry function value
const double MBISL125SH125Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					MBISL125SH125Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double MBISL125SH125Seed::dose_rate_constant = 0.953;

// Set the seed radial dose function
const boost::array<double,MBISL125SH125Seed::rdf_points*2> 
MBISL125SH125Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.101, 1.101, 1.101, 1.084, 1.041, 1.000, 0.898, 0.795, 0.610, 0.456, 0.338,
   0.250, 0.183, 0.134, 0.098, 0.072 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> MBISL125SH125Seed::cunningham_fit_coeffs =
  {1.11978980982, 1.50052020998, 0.352885119523, 2.60608715319, 1.17257157464};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,MBISL125SH125Seed::af_radii> 
MBISL125SH125Seed::anisotropy_function_radii = 
  {1.0, 2.0, 3.0, 4.0, 5.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		MBISL125SH125Seed::af_angles*(MBISL125SH125Seed::af_radii+1)>
MBISL125SH125Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.359, 0.429, 0.568, 0.710, 0.823, 0.918, 0.973, 0.985, 0.991, 
   1.000, // r0 values
   0.424, 0.493, 0.610, 0.744, 0.842, 0.926, 0.972, 0.987, 1.000, 
   1.000, // r1 values
   0.471, 0.535, 0.643, 0.759, 0.852, 0.936, 0.980, 0.989, 1.013, 
   1.000, // r2 values
   0.501, 0.563, 0.672, 0.771, 0.863, 0.937, 0.986, 0.993, 1.002, 
   1.000, // r3 values
   0.520, 0.574, 0.670, 0.762, 0.857, 0.921, 0.974, 0.993, 0.993,
   1.000  // r4 values
  };

// Constructor
MBISL125SH125Seed::MBISL125SH125Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType MBISL125SH125Seed::getSeedType() const
{
  return MBISL125SH125Seed::seed_type;
}

// Return the seed name
std::string MBISL125SH125Seed::getSeedName() const
{
  return MBISL125SH125Seed::seed_name;
}

// Return the seed strength
double MBISL125SH125Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double MBISL125SH125Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );

  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					radius,
					theta,
					MBISL125SH125Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			    radius,
			    MBISL125SH125Seed::radial_dose_function.data(),
			    MBISL125SH125Seed::rdf_points,
			    MBISL125SH125Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  MBISL125SH125Seed::anisotropy_function.data(),
			  MBISL125SH125Seed::anisotropy_function_radii.data(),
			  MBISL125SH125Seed::af_angles,
			  MBISL125SH125Seed::af_radii );
						  
  return d_air_kerma_strength*MBISL125SH125Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/MBISL125SH125Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double MBISL125SH125Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end MBISL125SH125Seed.cpp
//---------------------------------------------------------------------------//
