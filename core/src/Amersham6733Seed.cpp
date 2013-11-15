//---------------------------------------------------------------------------//
//!
//! \file   Amersham6733Seed.cpp
//! \author Alex Robinson
//! \brief  Amersham 6733 brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "Amersham6733Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the effective seed length (Leff) (cm)
const double Amersham6733Seed::effective_length = 0.30;

// Set the reference geometry function value
const double Amersham6733Seed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					  1.0,
					  acos(0.0),
					  Amersham6733Seed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double Amersham6733Seed::dose_rate_constant = 0.980;

// Set the seed radial dose function
const boost::array<double,Amersham6733Seed::rdf_points*2> 
Amersham6733Seed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.050, 1.076, 1.085, 1.069, 1.045, 1.000, 0.912, 0.821, 0.656, 0.495, 0.379,
   0.285, 0.214, 0.155, 0.119, 0.0840 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> Amersham6733Seed::cunningham_fit_coeffs =
  {1.25055495099, 1.63318545844, 0.160978811912, 3.26712150425, 1.11371259824};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,Amersham6733Seed::af_radii> 
Amersham6733Seed::anisotropy_function_radii = 
  {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		   Amersham6733Seed::af_angles*(Amersham6733Seed::af_radii+1)> 
Amersham6733Seed::anisotropy_function =
  {0, 5, 10, 15, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.305, 0.386, 0.507, 0.621, 0.714, 0.848, 0.944, 0.999, 1.029, 1.038, 1.026,
   1.000, // r0 values
   0.397, 0.468, 0.570, 0.663, 0.738, 0.851, 0.933, 0.985, 1.015, 1.033, 1.034,
   1.000, // r1 values
   0.451, 0.510, 0.609, 0.680, 0.743, 0.849, 0.918, 0.969, 0.995, 1.015, 1.014,
   1.000, // r2 values
   0.502, 0.557, 0.634, 0.712, 0.774, 0.873, 0.932, 0.983, 1.012, 1.022, 1.026,
   1.000, // r3 values
   0.533, 0.586, 0.660, 0.717, 0.769, 0.859, 0.921, 0.953, 0.985, 1.001, 1.009,
   1.000, // r4 values
   0.551, 0.595, 0.669, 0.726, 0.779, 0.860, 0.912, 0.965, 1.003, 0.994, 0.999,
   1.000, // r5 values
   0.565, 0.611, 0.685, 0.719, 0.785, 0.880, 0.924, 0.949, 0.982, 1.019, 1.000,
   1.000  // r6 values
  };

// Constructor
Amersham6733Seed::Amersham6733Seed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType Amersham6733Seed::getSeedType() const
{
  return Amersham6733Seed::seed_type;
}

// Return the dose rate at a given point (cGy/hr)
double Amersham6733Seed::getDoseRate( const double x, 
				      const double y, 
				      const double z ) const
{
  double radius = calculateRadius( x, y, z );
  double theta = calculatePolarAngle( radius, z );
  
  // Evaluate the geometry function
  double geometry_function_value = evaluateGeometryFunction( 
					  radius,
					  theta,
					  Amersham6733Seed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			      radius,
			      Amersham6733Seed::radial_dose_function.data(),
			      Amersham6733Seed::rdf_points,
			      Amersham6733Seed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  Amersham6733Seed::anisotropy_function.data(),
			  Amersham6733Seed::anisotropy_function_radii.data(),
			  Amersham6733Seed::af_angles,
			  Amersham6733Seed::af_radii );
						  
  return d_air_kerma_strength*Amersham6733Seed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/Amersham6733Seed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double Amersham6733Seed::getTotalDose( const double x, 
				       const double y, 
				       const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end Amersham6733Seed.cpp
//---------------------------------------------------------------------------//
