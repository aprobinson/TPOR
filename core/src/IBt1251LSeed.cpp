//---------------------------------------------------------------------------//
//!
//! \file   IBt1251LSeed.cpp
//! \author Alex Robinson
//! \brief  IBt 1251L brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "IBt1251LSeed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string IBt1251LSeed::seed_name = "IBt1251LSeed";

// Set the effective seed length (Leff) (cm)
const double IBt1251LSeed::effective_length = 0.435;

// Set the reference geometry function value
const double IBt1251LSeed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					IBt1251LSeed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double IBt1251LSeed::dose_rate_constant = 1.038;

// Set the seed radial dose function
const boost::array<double,IBt1251LSeed::rdf_points*2> 
IBt1251LSeed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   0.757, 0.841, 0.963, 1.021, 1.024, 1.000, 0.937, 0.859, 0.700, 0.554, 0.425,
   0.323, 0.240, 0.180,	0.138, 0.101 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> IBt1251LSeed::cunningham_fit_coeffs =
  {0.882104245806, 1.23951436352, 0.325475781109, 3.47272496361, 
   1.09836345569};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,IBt1251LSeed::af_radii> 
IBt1251LSeed::anisotropy_function_radii = 
  {0.5, 1.0, 2.0, 3.0, 5.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		IBt1251LSeed::af_angles*(IBt1251LSeed::af_radii+1)>
IBt1251LSeed::anisotropy_function =
  {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.476, 0.645, 0.725, 0.810, 0.867, 0.923, 0.966, 0.991, 0.998, 1.002, 
   1.000, // r0 values
   0.544, 0.626, 0.699, 0.783, 0.849, 0.900, 0.946, 0.979, 0.988, 0.996, 
   1.000, // r1 values
   0.653, 0.656, 0.709, 0.789, 0.849, 0.910, 0.946, 0.971, 0.991, 0.997, 
   1.000, // r2 values
   0.680, 0.713, 0.736, 0.810, 0.859, 0.911, 0.949, 0.976, 0.996, 0.995, 
   1.000, // r3 values
   0.703, 0.718, 0.751, 0.817, 0.854, 0.911, 0.954, 0.968, 0.988, 0.988,
   1.000  // r4 values
  };

// Constructor
IBt1251LSeed::IBt1251LSeed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType IBt1251LSeed::getSeedType() const
{
  return IBt1251LSeed::seed_type;
}

// Return the seed name
std::string IBt1251LSeed::getSeedName() const
{
  return IBt1251LSeed::seed_name;
}

// Return the seed strength
double IBt1251LSeed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double IBt1251LSeed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );

  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					radius,
					theta,
					IBt1251LSeed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			    radius,
			    IBt1251LSeed::radial_dose_function.data(),
			    IBt1251LSeed::rdf_points,
			    IBt1251LSeed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  IBt1251LSeed::anisotropy_function.data(),
			  IBt1251LSeed::anisotropy_function_radii.data(),
			  IBt1251LSeed::af_angles,
			  IBt1251LSeed::af_radii );
						  
  return d_air_kerma_strength*IBt1251LSeed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/IBt1251LSeed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double IBt1251LSeed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end IBt1251LSeed.cpp
//---------------------------------------------------------------------------//
