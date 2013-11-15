//---------------------------------------------------------------------------//
//!
//! \file   Amersham6711Seed.cpp
//! \author Alex Robinson
//! \brief  Amersham 6711 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Amersham6711Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the effective seed length (Leff) (cm)
const double Amersham6711Seed::effective_length = 0.30;

// Set the reference geometry function value
const double Amersham6711Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  Amersham6711Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Amersham6711Seed::dose_rate_constant = 0.965;

// Set the seed radial dose function
const boost::array<double,Amersham6711Seed::rdf_points*2> 
Amersham6711Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.055, 1.078, 1.082, 1.071, 1.042, 1.000, 0.908, 0.814, 0.632, 0.496, 0.364,
   0.270, 0.199, 0.148, 0.109, 0.0803 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Amersham6711Seed::cunningham_fit_coeffs =
  {1.46765182623, 1.87106159031, 0.0942126722129, 3.31408710528, 
   1.10557239977};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Amersham6711Seed::af_radii> 
Amersham6711Seed::anisotropy_function_radii = 
  {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   Amersham6711Seed::af_angles*(Amersham6711Seed::af_radii+1)> 
Amersham6711Seed::anisotropy_function =
  {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.333, 0.400, 0.519, 0.716, 0.846, 0.926, 0.972, 0.991, 0.996, 1.000, 
   1.000, // r0 values
   0.370, 0.429, 0.537, 0.705, 0.834, 0.925, 0.972, 0.991, 0.996, 1.000, 
   1.000, // r1 values
   0.442, 0.497, 0.580, 0.727, 0.842, 0.926, 0.970, 0.987, 0.996, 1.000,
   1.000, // r2 values
   0.488, 0.535, 0.609, 0.743, 0.846, 0.926, 0.969, 0.987, 0.995, 0.999, 
   1.000, // r3 values
   0.520, 0.561, 0.630, 0.752, 0.848, 0.928, 0.969, 0.987, 0.995, 0.999, 
   1.000, // r4 values
   0.550, 0.587, 0.645, 0.760, 0.852, 0.928, 0.969, 0.987, 0.995, 0.999,
   1.000 // r5 values
  };

// Constructor
Amersham6711Seed::Amersham6711Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Amersham6711Seed::getSeedType() const
{
  return Amersham6711Seed::seed_type;
}

// Return the dose rate at a given point (cGy/hr)
double Amersham6711Seed::getDoseRate( const double x, 
				      const double y, 
				      const double z ) const
{
  double radius = calculateRadius( x, y, z );
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  Amersham6711Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      Amersham6711Seed::radial_dose_function.data(),
			      Amersham6711Seed::rdf_points,
			      Amersham6711Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  Amersham6711Seed::anisotropy_function.data(),
			  Amersham6711Seed::anisotropy_function_radii.data(),
			  Amersham6711Seed::af_angles,
			  Amersham6711Seed::af_radii );
						  
  return d_air_kerma_strength*Amersham6711Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Amersham6711Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Amersham6711Seed::getTotalDose( const double x, 
				       const double y, 
				       const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Amersham6711Seed.cpp
//---------------------------------------------------------------------------//
