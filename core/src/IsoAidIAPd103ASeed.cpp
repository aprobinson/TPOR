//---------------------------------------------------------------------------//
//!
//! \file   IsoAidIAPd103ASeed.cpp
//! \author Alex Robinson
//! \brief  IsoAid IAPd-103A brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "IsoAidIAPd103ASeed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string IsoAidIAPd103ASeed::seed_name = "IsoAidIAPd103ASeed";

// Set the effective seed length (Leff) (cm)
const double IsoAidIAPd103ASeed::effective_length = 0.34;

// Set the reference geometry function value
const double IsoAidIAPd103ASeed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					IsoAidIAPd103ASeed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double IsoAidIAPd103ASeed::dose_rate_constant = 0.709;

// Set the seed radial dose function
const boost::array<double,IsoAidIAPd103ASeed::rdf_points*2> 
IsoAidIAPd103ASeed::radial_dose_function =
  {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.9, 1.0, 1.5, 2.0, 2.5, 3.0, 
   3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 
   10.0, // radii
   0.915, 1.234, 1.296, 1.290, 1.260, 1.213, 1.160, 1.134, 1.106, 1.053, 1.000,
   0.768, 0.576, 0.429, 0.318, 0.233, 0.173, 0.127, 0.092, 0.069, 0.050, 0.037,
   0.028, 0.020, 0.015, 0.011, 0.008, 0.006, 0.005 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> IsoAidIAPd103ASeed::cunningham_fit_coeffs =
  {1.92054230614, 2.62709542099, 0.677840206129, 1.17180295085, 1.46864411521};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,IsoAidIAPd103ASeed::af_radii> 
IsoAidIAPd103ASeed::anisotropy_function_radii = 
  {0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		IsoAidIAPd103ASeed::af_angles*(IsoAidIAPd103ASeed::af_radii+1)>
IsoAidIAPd103ASeed::anisotropy_function =
  {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 
   90, // theta values
   0.319, 0.333, 0.349, 0.436, 0.520, 0.807, 0.852, 0.880, 0.902, 0.941, 1.021,
   1.020, 1.016, 1.009, 1.003, 0.997, 0.992, 0.990, 1.000, // r0 values
   0.307, 0.310, 0.321, 0.350, 0.482, 0.549, 0.705, 0.752, 0.807, 0.906, 0.948,
   0.964, 0.976, 0.984, 0.990, 0.994, 0.997, 0.999, 1.000, // r1 values
   0.320, 0.324, 0.335, 0.362, 0.482, 0.539, 0.641, 0.724, 0.796, 0.895, 0.926,
   0.946, 0.963, 0.975, 0.986, 0.994, 0.999, 1.000, 1.000, // r2 values
   0.337, 0.340, 0.352, 0.381, 0.490, 0.544, 0.627, 0.724, 0.794, 0.882, 0.912,
   0.936, 0.951, 0.967, 0.979, 0.988, 0.993, 0.999, 1.000, // r3 values
   0.349, 0.356, 0.367, 0.395, 0.500, 0.553, 0.626, 0.716, 0.792, 0.873, 0.903,
   0.930, 0.946, 0.966, 0.976, 0.986, 0.975, 0.989, 1.000, // r4 values
   0.365, 0.365, 0.371, 0.413, 0.522, 0.568, 0.641, 0.727, 0.802, 0.890, 0.921,
   0.940, 0.957, 0.992, 0.992, 0.997, 1.013, 1.033, 1.000, // r5 values
   0.392, 0.384, 0.390, 0.425, 0.521, 0.579, 0.647, 0.741, 0.805, 0.882, 0.918,
   0.937, 0.936, 0.990, 1.018, 1.019, 1.009, 1.005, 1.000, // r6 values
   0.379, 0.397, 0.399, 0.430, 0.518, 0.577, 0.650, 0.740, 0.782, 0.858, 0.887,
   0.888, 0.956, 0.975, 0.953, 0.961, 0.985, 0.973, 1.000  // r7 values
  };

// Constructor
IsoAidIAPd103ASeed::IsoAidIAPd103ASeed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType IsoAidIAPd103ASeed::getSeedType() const
{
  return IsoAidIAPd103ASeed::seed_type;
}

// Return the seed name
std::string IsoAidIAPd103ASeed::getSeedName() const
{
  return IsoAidIAPd103ASeed::seed_name;
}

// Return the seed strength
double IsoAidIAPd103ASeed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double IsoAidIAPd103ASeed::getDoseRate( const double x, 
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
					IsoAidIAPd103ASeed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			    radius,
			    IsoAidIAPd103ASeed::radial_dose_function.data(),
			    IsoAidIAPd103ASeed::rdf_points,
			    IsoAidIAPd103ASeed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  IsoAidIAPd103ASeed::anisotropy_function.data(),
			  IsoAidIAPd103ASeed::anisotropy_function_radii.data(),
			  IsoAidIAPd103ASeed::af_angles,
			  IsoAidIAPd103ASeed::af_radii );
						  
  return d_air_kerma_strength*IsoAidIAPd103ASeed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/IsoAidIAPd103ASeed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double IsoAidIAPd103ASeed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::pd103_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end IsoAidIAPd103ASeed.cpp
//---------------------------------------------------------------------------//
