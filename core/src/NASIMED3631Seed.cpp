//---------------------------------------------------------------------------//
//!
//! \file   NASIMED3631Seed.cpp
//! \author Alex Robinson
//! \brief  NASI MED 3631 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "NASIMED3631Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the effective seed length (Leff) (cm)
const double NASIMED3631Seed::effective_length = 0.42;

// Set the reference geometry function value
const double NASIMED3631Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  NASIMED3631Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double NASIMED3631Seed::dose_rate_constant = 1.036;

// Set the seed radial dose function
const boost::array<double,NASIMED3631Seed::rdf_points*2> 
NASIMED3631Seed::radial_dose_function =
  {0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, // radii
   0.998, 1.025, 1.019, 1.000, 0.954, 0.836, 0.676, 0.523, 0.395, 0.293, 
   0.211 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> NASIMED3631Seed::cunningham_fit_coeffs =
  {3.09951745046, 3.5989866341, 0.000686277516937, 4.18282107799, 
   1.01704022417};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,NASIMED3631Seed::af_radii> 
NASIMED3631Seed::anisotropy_function_radii = 
  {0.25, 0.5, 1.0, 2.0, 5.0, 10.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   NASIMED3631Seed::af_angles*(NASIMED3631Seed::af_radii+1)> 
NASIMED3631Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   1.038, 0.984, 0.916, 0.928, 0.941, 0.962, 0.975, 0.991, 0.999, 
   1.000, // r0 values
   0.690, 0.700, 0.761, 0.854, 0.909, 0.949, 0.975, 0.989, 0.999, 
   1.000, // r1 values
   0.702, 0.662, 0.747, 0.846, 0.906, 0.949, 0.975, 0.992, 1.003, 
   1.000, // r2 values
   0.667, 0.676, 0.764, 0.852, 0.909, 0.950, 0.975, 0.990, 0.996, 
   1.000, // r3 values
   0.718, 0.728, 0.794, 0.871, 0.918, 0.958, 0.983, 0.993, 0.998, 
   1.000, // r4 values
   0.771, 0.758, 0.815, 0.878, 0.914, 0.954, 0.972, 0.989, 0.999,
   1.000  // r5 values
  };

// Constructor
NASIMED3631Seed::NASIMED3631Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )

{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType NASIMED3631Seed::getSeedType() const
{
  return NASIMED3631Seed::seed_type;
}

// Return the dose rate at a given point (cGy/hr)
double NASIMED3631Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  NASIMED3631Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      NASIMED3631Seed::radial_dose_function.data(),
			      NASIMED3631Seed::rdf_points,
			      NASIMED3631Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  NASIMED3631Seed::anisotropy_function.data(),
			  NASIMED3631Seed::anisotropy_function_radii.data(),
			  NASIMED3631Seed::af_angles,
			  NASIMED3631Seed::af_radii );
						  
  return d_air_kerma_strength*NASIMED3631Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/NASIMED3631Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double NASIMED3631Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end NASIMED3631Seed.cpp
//---------------------------------------------------------------------------//
