//---------------------------------------------------------------------------//
//!
//! \file   DraximageLS1Seed.cpp
//! \author Alex Robinson
//! \brief  Draximage BrachySeed LS-1 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "DraximageLS1Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string DraximageLS1Seed::seed_name = "DraximageLS1Seed";

// Set the effective seed length (Leff) (cm)
const double DraximageLS1Seed::effective_length = 0.41;

// Set the reference geometry function value
const double DraximageLS1Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					DraximageLS1Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double DraximageLS1Seed::dose_rate_constant = 0.972;

// Set the seed radial dose function
const boost::array<double,DraximageLS1Seed::rdf_points*2> 
DraximageLS1Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   0.182, 0.323, 0.741, 0.964, 1.004, 1.000, 0.937, 0.853, 0.680, 0.527, 0.400,
   0.300, 0.223, 0.166, 0.122, 0.0900 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> DraximageLS1Seed::cunningham_fit_coeffs =
  {1.47020089678, 1.86839707578, 0.109010835208, 3.90473364387, 1.01117554615};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,DraximageLS1Seed::af_radii> 
DraximageLS1Seed::anisotropy_function_radii = 
  {0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 5.0, 10.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		DraximageLS1Seed::af_angles*(DraximageLS1Seed::af_radii+1)>
DraximageLS1Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   3.459, 3.312, 2.755, 2.130, 1.675, 1.380, 1.194, 1.085, 1.024, 
   1.000, // r0 values
   1.261, 1.246, 1.219, 1.178, 1.125, 1.073, 1.032, 1.007, 0.999, 
   1.000, // r1 values
   0.979, 0.977, 0.988, 0.994, 0.999, 0.998, 0.996, 0.998, 1.001, 
   1.000, // r2 values
   0.872, 0.877, 0.901, 0.925, 0.950, 0.967, 0.981, 0.994, 1.001, 
   1.000, // r3 values
   0.799, 0.808, 0.841, 0.877, 0.912, 0.945, 0.970, 0.990, 0.999, 
   1.000, // r4 values
   0.775, 0.787, 0.821, 0.861, 0.902, 0.938, 0.968, 0.989, 0.999, 
   1.000, // r5 values
   0.765, 0.775, 0.811, 0.854, 0.898, 0.934, 0.967, 0.988, 0.998, 
   1.000, // r6 values
   0.766, 0.778, 0.816, 0.864, 0.909, 0.940, 0.968, 0.991, 1.004, 
   1.000, // r7 values
   0.781, 0.786, 0.822, 0.873, 0.899, 0.935, 0.964, 0.993, 0.982, 
   1.000  // r8 values
  };

// Constructor
DraximageLS1Seed::DraximageLS1Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType DraximageLS1Seed::getSeedType() const
{
  return DraximageLS1Seed::seed_type;
}

// Return the seed name
std::string DraximageLS1Seed::getSeedName() const
{
  return DraximageLS1Seed::seed_name;
}

// Return the seed strength
double DraximageLS1Seed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double DraximageLS1Seed::getDoseRate( const double x, 
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
					DraximageLS1Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			    radius,
			    DraximageLS1Seed::radial_dose_function.data(),
			    DraximageLS1Seed::rdf_points,
			    DraximageLS1Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  DraximageLS1Seed::anisotropy_function.data(),
			  DraximageLS1Seed::anisotropy_function_radii.data(),
			  DraximageLS1Seed::af_angles,
			  DraximageLS1Seed::af_radii );
						  
  return d_air_kerma_strength*DraximageLS1Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/DraximageLS1Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double DraximageLS1Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end DraximageLS1Seed.cpp
//---------------------------------------------------------------------------//
