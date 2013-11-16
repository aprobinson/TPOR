//---------------------------------------------------------------------------//
//!
//! \file   Nucletron130002Seed.hpp
//! \author Alex Robinson
//! \brief  Source Tech STM1251 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Nucletron130002Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the effective seed length (Leff) (cm)
const double Nucletron130002Seed::effective_length = 0.3;

// Set the reference geometry function value
const double Nucletron130002Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
				     1.0,
				     acos(0.0),
				     Nucletron130002Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Nucletron130002Seed::dose_rate_constant = 0.954;

// Set the seed radial dose function
const boost::array<double,Nucletron130002Seed::rdf_points*2> 
Nucletron130002Seed::radial_dose_function =
  {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.5, 2, 2.5, 3, 3.5, 4, 
   4.5, 5, 6, 7, 8, 9, 10, // radii
   1.042, 1.082, 1.087, 1.085, 1.078, 1.066, 1.052, 1.035, 1.019, 1.000, 0.907,
   0.808, 0.713, 0.627, 0.548, 0.477, 0.414, 0.357, 0.265, 0.196, 0.144, 0.106,
   0.078 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Nucletron130002Seed::cunningham_fit_coeffs =
  {1.13995941028, 1.52147092047, 0.254384417907, 2.85860627707, 1.16168136199};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Nucletron130002Seed::af_radii> 
Nucletron130002Seed::anisotropy_function_radii = 
  {0.3, 0.5, 0.7, 1, 1.5, 2, 3, 4, 6, 8};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
	  Nucletron130002Seed::af_angles*(Nucletron130002Seed::af_radii+1)>
Nucletron130002Seed::anisotropy_function =
  {0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 7.5, 10.5, 12.5, 15.5, 17.5, 20.5, 25.5, 30.5,
   35.5, 40.5, 50.5, 60.5, 70.5, 80.5, 90, // theta values
   0.197, 0.200, 0.201, 0.202, 0.213, 0.240, 0.353, 0.410, 0.469, 0.565, 0.623,
   0.700, 0.803, 0.880, 0.937, 0.970, 0.988, 0.966, 0.987, 0.996, 
   1.000, // r0 values
   0.205, 0.211, 0.218, 0.246, 0.309, 0.351, 0.371, 0.425, 0.473, 0.543, 0.588,
   0.646, 0.728, 0.797, 0.854, 0.902, 0.973, 1.003, 0.981, 0.995, 
   1.000, // r1 values
   0.248, 0.251, 0.268, 0.333, 0.373, 0.384, 0.401, 0.460, 0.502, 0.567, 0.602,
   0.659, 0.729, 0.790, 0.841, 0.886, 0.955, 1.002, 1.013, 0.995, 
   1.000, // r2 values
   0.291, 0.297, 0.354, 0.406, 0.420, 0.425, 0.442, 0.499, 0.540, 0.595, 0.631,
   0.679, 0.747, 0.799, 0.844, 0.886, 0.952, 0.998, 1.017, 0.994, 
   1.000, // r3 values
   0.344, 0.382, 0.441, 0.463, 0.467, 0.471, 0.495, 0.546, 0.582, 0.633, 0.663,
   0.706, 0.766, 0.815, 0.853, 0.889, 0.951, 0.991, 1.016, 1.013, 
   1.000, // r4 values
   0.388, 0.441, 0.483, 0.499, 0.508, 0.511, 0.535, 0.580, 0.617, 0.661, 0.689,
   0.728, 0.783, 0.828, 0.866, 0.898, 0.956, 0.991, 1.019, 1.019, 
   1.000, // r5 values
   0.464, 0.519, 0.552, 0.557, 0.559, 0.562, 0.581, 0.631, 0.657, 0.695, 0.723,
   0.754, 0.799, 0.844, 0.879, 0.907, 0.955, 0.993, 1.014, 1.018, 
   1.000, // r6 values
   0.515, 0.553, 0.580, 0.591, 0.594, 0.599, 0.616, 0.654, 0.680, 0.715, 0.739,
   0.769, 0.814, 0.852, 0.882, 0.908, 0.955, 0.991, 1.008, 1.019, 
   1.000, // r7 values
   0.569, 0.620, 0.635, 0.639, 0.641, 0.644, 0.668, 0.699, 0.716, 0.745, 0.763,
   0.792, 0.836, 0.867, 0.892, 0.918, 0.958, 0.986, 1.011, 1.018, 
   1.000, // r8 values
   0.615, 0.648, 0.655, 0.658, 0.667, 0.671, 0.685, 0.719, 0.731, 0.762, 0.788,
   0.810, 0.840, 0.871, 0.903, 0.926, 0.959, 0.987, 1.006, 1.014,
   1.000 // r9 values
  };

// Constructor
Nucletron130002Seed::Nucletron130002Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Nucletron130002Seed::getSeedType() const
{
  return Nucletron130002Seed::seed_type;
}

// Return the dose rate at a given point (cGy/hr)
double Nucletron130002Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
				     radius,
				     theta,
				     Nucletron130002Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			 radius,
			 Nucletron130002Seed::radial_dose_function.data(),
			 Nucletron130002Seed::rdf_points,
			 Nucletron130002Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
		       radius, 
		       theta,
		       Nucletron130002Seed::anisotropy_function.data(),
		       Nucletron130002Seed::anisotropy_function_radii.data(),
		       Nucletron130002Seed::af_angles,
		       Nucletron130002Seed::af_radii );
						  
  return d_air_kerma_strength*Nucletron130002Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Nucletron130002Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Nucletron130002Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Nucletron130002Seed.cpp
//---------------------------------------------------------------------------//
