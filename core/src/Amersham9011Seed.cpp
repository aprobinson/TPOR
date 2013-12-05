//---------------------------------------------------------------------------//
//!
//! \file   Amersham9011Seed.cpp
//! \author Alex Robinson
//! \brief  Amersham 9011 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Amersham9011Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string Amersham9011Seed::seed_name = "Amersham9011Seed";

// Set the effective seed length (Leff) (cm)
const double Amersham9011Seed::effective_length = 0.28;

// Set the reference geometry function value
const double Amersham9011Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  Amersham9011Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Amersham9011Seed::dose_rate_constant = 0.923;

// Set the seed radial dose function
const boost::array<double,Amersham9011Seed::rdf_points*2> 
Amersham9011Seed::radial_dose_function =
  {0.2, 0.3, 0.5, 0.7, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 
   6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0, 12.0, // radii
   1.079, 1.084, 1.072, 1.047, 1.000, 0.908, 0.811, 0.717, 0.629, 0.549, 0.477,
   0.413, 0.357, 0.308, 0.265, 0.228, 0.196, 0.168, 0.144, 0.124, 0.106, 0.091,
   0.078, 0.042 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Amersham9011Seed::cunningham_fit_coeffs =
  {0.947235431599, 1.31060345517, 0.477491677635, 2.59741515131, 
   1.17358949117};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Amersham9011Seed::af_radii> 
Amersham9011Seed::anisotropy_function_radii = 
  {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   Amersham9011Seed::af_angles*(Amersham9011Seed::af_radii+1)> 
Amersham9011Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.221, 0.403, 0.655, 0.800, 0.885, 0.939, 0.974, 0.993, 0.996, 
   1.000, // r0 values
   0.291, 0.465, 0.676, 0.803, 0.883, 0.935, 0.971, 0.994, 1.002,  
   1.000, // r1 values
   0.390, 0.542, 0.714, 0.822, 0.893, 0.939, 0.972, 0.993, 1.004, 
   1.000, // r2 values
   0.446, 0.587, 0.739, 0.835, 0.899, 0.943, 0.973, 0.993, 1.003, 
   1.000, // r3 values
   0.500, 0.617, 0.755, 0.844, 0.904, 0.945, 0.974, 0.992, 1.002, 
   1.000, // r4 values
   0.526, 0.639, 0.768, 0.852, 0.908, 0.947, 0.974, 0.992, 1.002,
   1.000  // r5 values
  };

// Constructor
Amersham9011Seed::Amersham9011Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Amersham9011Seed::getSeedType() const
{
  return Amersham9011Seed::seed_type;
}

// Return the seed name
std::string Amersham9011Seed::getSeedName() const
{
  return Amersham9011Seed::seed_name;
}

// Return the seed strength
double Amersham9011Seed::getSeedStrength() const
{
  return Amersham9011Seed::d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double Amersham9011Seed::getDoseRate( const double x, 
				      const double y, 
				      const double z ) const
{
  double radius = calculateRadius( x, y, z );

  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  Amersham9011Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      Amersham9011Seed::radial_dose_function.data(),
			      Amersham9011Seed::rdf_points,
			      Amersham9011Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  Amersham9011Seed::anisotropy_function.data(),
			  Amersham9011Seed::anisotropy_function_radii.data(),
			  Amersham9011Seed::af_angles,
			  Amersham9011Seed::af_radii );
						  
  return d_air_kerma_strength*Amersham9011Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Amersham9011Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Amersham9011Seed::getTotalDose( const double x, 
				       const double y, 
				       const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Amersham9011Seed.cpp
//---------------------------------------------------------------------------//
