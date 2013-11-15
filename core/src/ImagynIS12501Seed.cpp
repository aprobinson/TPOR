//---------------------------------------------------------------------------//
//!
//! \file   ImagynIS12501Seed.cpp
//! \author Alex Robinson
//! \brief  Imagyn IS-12501 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "ImagynIS12501Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the effective seed length (Leff) (cm)
const double ImagynIS12501Seed::effective_length = 0.34;

// Set the reference geometry function value
const double ImagynIS12501Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					 1.0,
					 acos(0.0),
					 ImagynIS12501Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double ImagynIS12501Seed::dose_rate_constant = 0.940;

// Set the seed radial dose function
const boost::array<double,ImagynIS12501Seed::rdf_points*2> 
ImagynIS12501Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.022, 1.058, 1.093, 1.080, 1.048, 1.000, 0.907, 0.808, 0.618, 0.463, 0.348,
   0.253, 0.193, 0.149, 0.100, 0.075 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> ImagynIS12501Seed::cunningham_fit_coeffs =
  {1.61155438266, 2.00973368538, 0.126766232781, 3.17079295308, 1.08642482398};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,ImagynIS12501Seed::af_radii> 
ImagynIS12501Seed::anisotropy_function_radii = 
  {1.0, 2.0, 3.0, 5.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		  ImagynIS12501Seed::af_angles*(ImagynIS12501Seed::af_radii+1)>
ImagynIS12501Seed::anisotropy_function =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.241, 0.327, 0.479, 0.634, 0.768, 0.867, 0.946, 0.986, 0.998, 
   1.000, // r0 values
   0.337, 0.399, 0.532, 0.663, 0.775, 0.870, 0.944, 0.985, 0.994,
   1.000, // r1 values
   0.362, 0.440, 0.563, 0.681, 0.786, 0.878, 0.944, 0.987, 1.004, 
   1.000, // r2 values
   0.424, 0.486, 0.584, 0.706, 0.806, 0.875, 0.943, 0.974, 0.981, 
   1.000, // r3 values
   0.454, 0.510, 0.581, 0.700, 0.776, 0.849, 0.913, 0.955, 0.956,
   1.000  // r4 values
  };

// Constructor
ImagynIS12501Seed::ImagynIS12501Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType ImagynIS12501Seed::getSeedType() const
{
  return ImagynIS12501Seed::seed_type;
}

// Return the dose rate at a given point (cGy/hr)
double ImagynIS12501Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					 radius,
					 theta,
					 ImagynIS12501Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			     radius,
			     ImagynIS12501Seed::radial_dose_function.data(),
			     ImagynIS12501Seed::rdf_points,
			     ImagynIS12501Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  ImagynIS12501Seed::anisotropy_function.data(),
			  ImagynIS12501Seed::anisotropy_function_radii.data(),
			  ImagynIS12501Seed::af_angles,
			  ImagynIS12501Seed::af_radii );
						  
  return d_air_kerma_strength*ImagynIS12501Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/ImagynIS12501Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double ImagynIS12501Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end ImagynIS12501Seed.cpp
//---------------------------------------------------------------------------//
