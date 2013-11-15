//---------------------------------------------------------------------------//
//!
//! \file   Theragenics200Seed.cpp
//! \author Alex Robinson
//! \brief  Theragenics 200 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Theragenics200Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the effective seed length (Leff) (cm)
const double Theragenics200Seed::effective_length = 0.423;

// Set the reference geometry function value
const double Theragenics200Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					Theragenics200Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Theragenics200Seed::dose_rate_constant = 0.686;

// Set the seed radial dose function
const boost::array<double,Theragenics200Seed::rdf_points*2> 
Theragenics200Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.3, 0.4, 0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 
   5.0, 6.0, 7.0, 10.0, // radii
   0.911, 1.21, 1.37, 1.38, 1.36, 1.30, 1.15, 1.000, 0.749, 0.555, 0.410, 
   0.302, 0.223, 0.163, 0.0887, 0.0482, 0.0262, 0.00615 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Theragenics200Seed::cunningham_fit_coeffs =
  {7.98564887935, 8.89711127592, 0.00133256288073, 1.80353486555, 
   1.25321610687};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Theragenics200Seed::af_radii> 
Theragenics200Seed::anisotropy_function_radii = 
  {0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0, 7.5};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		Theragenics200Seed::af_angles*(Theragenics200Seed::af_radii+1)>
Theragenics200Seed::anisotropy_function =
  {0, 1, 2, 3, 5, 7, 10, 12, 15, 20, 25, 30, 40, 50, 60, 70, 75, 80, 85, 
   90, // theta values
   0.619, 0.617, 0.618, 0.620, 0.617, 0.579, 0.284, 0.191, 0.289, 0.496, 0.655,
   0.775, 0.917, 0.945, 0.976, 0.981, 0.947, 0.992, 1.007, 1.000, // r0 values
   0.694, 0.689, 0.674, 0.642, 0.600, 0.553, 0.496, 0.466, 0.446, 0.442, 0.497,
   0.586, 0.734, 0.837, 0.906, 0.929, 0.938, 0.955, 0.973, 1.000, // r1 values
   0.601, 0.597, 0.574, 0.577, 0.540, 0.519, 0.495, 0.486, 0.482, 0.486, 0.524,
   0.585, 0.726, 0.831, 0.907, 0.954, 0.961, 0.959, 0.960, 1.000, // r2 values
   0.541, 0.549, 0.534, 0.538, 0.510, 0.498, 0.487, 0.487, 0.490, 0.501, 0.537,
   0.593, 0.727, 0.834, 0.912, 0.964, 0.978, 0.972, 0.982, 1.000, // r3 values
   0.526, 0.492, 0.514, 0.506, 0.499, 0.498, 0.504, 0.512, 0.523, 0.547, 0.582,
   0.633, 0.750, 0.853, 0.931, 0.989, 1.006, 1.017, 0.998, 1.000, // r4 values
   0.504, 0.505, 0.517, 0.509, 0.508, 0.509, 0.519, 0.529, 0.540, 0.568, 0.603,
   0.654, 0.766, 0.869, 0.942, 1.001, 1.021, 1.035, 1.030, 1.000, // r5 values
   0.497, 0.513, 0.524, 0.519, 0.514, 0.521, 0.530, 0.544, 0.556, 0.585, 0.621,
   0.667, 0.778, 0.881, 0.960, 1.008, 1.029, 1.046, 1.041, 1.000, // r6 values
   0.513, 0.533, 0.538, 0.532, 0.531, 0.532, 0.544, 0.555, 0.567, 0.605, 0.640,
   0.683, 0.784, 0.886, 0.964, 1.004, 1.024, 1.037, 1.036, 1.000, // r7 values
   0.547, 0.580, 0.568, 0.570, 0.571, 0.568, 0.590, 0.614, 0.614, 0.642, 0.684,
   0.719, 0.820, 0.912, 0.974, 1.011, 1.033, 1.043, 1.043, 1.000 // r8 values
  };

// Constructor
Theragenics200Seed::Theragenics200Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Theragenics200Seed::getSeedType() const
{
  return Theragenics200Seed::seed_type;
}

// Return the dose rate at a given point (cGy/hr)
double Theragenics200Seed::getDoseRate( const double x, 
				     const double y, 
				     const double z ) const
{
  double radius = calculateRadius( x, y, z );
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					radius,
					theta,
					Theragenics200Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			    radius,
			    Theragenics200Seed::radial_dose_function.data(),
			    Theragenics200Seed::rdf_points,
			    Theragenics200Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  Theragenics200Seed::anisotropy_function.data(),
			  Theragenics200Seed::anisotropy_function_radii.data(),
			  Theragenics200Seed::af_angles,
			  Theragenics200Seed::af_radii );
						  
  return d_air_kerma_strength*Theragenics200Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Theragenics200Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Theragenics200Seed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::pd103_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Theragenics200Seed.cpp
//---------------------------------------------------------------------------//
