//---------------------------------------------------------------------------//
//!
//! \file   ImplantSciences3500Seed.cpp
//! \author Alex Robinson
//! \brief  Implant Sciences 3500 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "ImplantSciences3500Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string ImplantSciences3500Seed::seed_name = 
  "ImplantSciences3500Seed";

// Set the effective seed length (Leff) (cm)
const double ImplantSciences3500Seed::effective_length = 0.376;

// Set the reference geometry function value
const double ImplantSciences3500Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					ImplantSciences3500Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double ImplantSciences3500Seed::dose_rate_constant = 1.014;

// Set the seed radial dose function
const boost::array<double,ImplantSciences3500Seed::rdf_points*2> 
ImplantSciences3500Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   0.997, 1.011, 1.021, 1.030, 1.026, 1.000, 0.932, 0.854, 0.681, 0.532, 
   0.407, 0.308, 0.230, 0.171, 0.127, 0.0936 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> ImplantSciences3500Seed::cunningham_fit_coeffs =
  {-1.55228990318, 0.399198553957, 1.74042256916, 2.55875606969, 
   1.03076786853};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,ImplantSciences3500Seed::af_radii> 
ImplantSciences3500Seed::anisotropy_function_radii = 
  {0.25, 0.5, 1.0, 2.0, 5.0, 10.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
      ImplantSciences3500Seed::af_angles*(ImplantSciences3500Seed::af_radii+1)>
ImplantSciences3500Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.494, 0.574, 0.785, 0.899, 0.943, 0.967, 0.986, 0.995, 1.000, 
   1.000, // r0 values
   0.610, 0.513, 0.679, 0.808, 0.892, 0.944, 0.974, 0.990, 0.997, 
   1.000, // r1 values
   0.580, 0.561, 0.705, 0.813, 0.885, 0.933, 0.967, 0.987, 0.997, 
   1.000, // r2 values
   0.652, 0.626, 0.743, 0.830, 0.893, 0.934, 0.967, 0.987, 0.997, 
   1.000, // r3 values
   0.690, 0.700, 0.789, 0.854, 0.905, 0.941, 0.968, 0.986, 0.996, 
   1.000, // r4 values
   0.709, 0.742, 0.815, 0.872, 0.912, 0.947, 0.972, 0.990, 0.997,
   1.000 // r5 values
  };

// Constructor
ImplantSciences3500Seed::ImplantSciences3500Seed( 
					      const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType ImplantSciences3500Seed::getSeedType() const
{
  return ImplantSciences3500Seed::seed_type;
}

// Return the seed name
std::string ImplantSciences3500Seed::getSeedName() const
{
  return ImplantSciences3500Seed::seed_name;
}

// Return the seed strength
double ImplantSciences3500Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double ImplantSciences3500Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );

  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
				   radius,
				   theta,
				   ImplantSciences3500Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
		       radius,
		       ImplantSciences3500Seed::radial_dose_function.data(),
		       ImplantSciences3500Seed::rdf_points,
		       ImplantSciences3500Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
		     radius, 
		     theta,
		     ImplantSciences3500Seed::anisotropy_function.data(),
		     ImplantSciences3500Seed::anisotropy_function_radii.data(),
		     ImplantSciences3500Seed::af_angles,
		     ImplantSciences3500Seed::af_radii );
						  
  return d_air_kerma_strength*ImplantSciences3500Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/ImplantSciences3500Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double ImplantSciences3500Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end ImplantSciences3500Seed.cpp
//---------------------------------------------------------------------------//
