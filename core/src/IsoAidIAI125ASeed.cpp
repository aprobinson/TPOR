//---------------------------------------------------------------------------//
//!
//! \file   IsoAidIAI125ASeed.cpp
//! \author Alex Robinson
//! \brief  IsoAid IAI-125A brachytherapy seed class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>
#include <math.h>

// TPOR Includes
#include "IsoAidIAI125ASeed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the seed name
const std::string IsoAidIAI125ASeed::seed_name = "IsoAidIAI125ASeed";

// Set the effective seed length (Leff) (cm)
const double IsoAidIAI125ASeed::effective_length = 0.3;

// Set the reference geometry function value
const double IsoAidIAI125ASeed::ref_geometry_func_value = 
  BrachytherapySeed::evaluateGeometryFunction( 
					1.0,
					acos(0.0),
					IsoAidIAI125ASeed::effective_length );

// Set the seed dose rate constant (1/cm^2)
const double IsoAidIAI125ASeed::dose_rate_constant = 0.981;

// Set the seed radial dose function
const boost::array<double,IsoAidIAI125ASeed::rdf_points*2> 
IsoAidIAI125ASeed::radial_dose_function =
  {0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 
   9.0, 10.0, // radii
   1.040, 1.053, 1.066, 1.080, 1.035, 1.000, 0.902, 0.800, 0.611, 0.468, 0.368,
   0.294, 0.227, 0.165, 0.141, 0.090 // function values
  };

// Set the Cunningham radial dose function fit coefficients
const boost::array<double,5> IsoAidIAI125ASeed::cunningham_fit_coeffs =
  {1.67263430812, 2.01929333236, 0.12814402168, 3.16605135455, 1.08168916717};

// Set the 2D anisotropy function radii (cm)
const boost::array<double,IsoAidIAI125ASeed::af_radii> 
IsoAidIAI125ASeed::anisotropy_function_radii = 
  {0.5, 1.0, 2.0, 3.0, 5.0, 7.0};

// Set the 2D anisotropy function (theta = degrees)
const boost::array<double,
		IsoAidIAI125ASeed::af_angles*(IsoAidIAI125ASeed::af_radii+1)>
IsoAidIAI125ASeed::anisotropy_function =
  {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, // theta values
   0.352, 0.411, 0.481, 0.699, 0.848, 0.948, 1.002, 1.029, 1.029, 0.999,  
   1.000, // r0 values
   0.406, 0.465, 0.527, 0.719, 0.846, 0.936, 0.986, 1.024, 1.039, 1.025, 
   1.000, // r1 values
   0.493, 0.545, 0.601, 0.757, 0.862, 0.932, 0.974, 1.008, 1.027, 1.024, 
   1.000, // r2 values
   0.520, 0.584, 0.642, 0.775, 0.862, 0.916, 0.961, 0.993, 1.006, 1.023, 
   1.000, // r3 values
   0.578, 0.658, 0.704, 0.794, 0.869, 0.937, 0.963, 0.990, 1.016, 1.009, 
   1.000, // r4 values
   0.612, 0.701, 0.726, 0.799, 0.879, 0.969, 0.971, 1.001, 1.010, 1.025, 
   1.000  // r5 values
  };

// Constructor
IsoAidIAI125ASeed::IsoAidIAI125ASeed( const double air_kerma_strength )
: BrachytherapySeed(),
  d_air_kerma_strength( air_kerma_strength )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() );  
}

// Return the seed type
BrachytherapySeedType IsoAidIAI125ASeed::getSeedType() const
{
  return IsoAidIAI125ASeed::seed_type;
}

// Return the seed name
std::string IsoAidIAI125ASeed::getSeedName() const
{
  return IsoAidIAI125ASeed::seed_name;
}

// Return the seed strength
double IsoAidIAI125ASeed::getSeedStrength() const
{
  return d_air_kerma_strength;
}

// Return the dose rate at a given point (cGy/hr)
double IsoAidIAI125ASeed::getDoseRate( const double x, 
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
					IsoAidIAI125ASeed::effective_length );

  // Evaluate the radial dose function
  double radial_dose_function_value = 
    BrachytherapySeed::evaluateRadialDoseFunction( 
			    radius,
			    IsoAidIAI125ASeed::radial_dose_function.data(),
			    IsoAidIAI125ASeed::rdf_points,
			    IsoAidIAI125ASeed::cunningham_fit_coeffs.data() );
  
  // Evaluate the 2D anisotropy function
  double anisotropy_function_value = 
    BrachytherapySeed::evaluateAnisotropyFunction( 
			  radius, 
			  theta,
			  IsoAidIAI125ASeed::anisotropy_function.data(),
			  IsoAidIAI125ASeed::anisotropy_function_radii.data(),
			  IsoAidIAI125ASeed::af_angles,
			  IsoAidIAI125ASeed::af_radii );
						  
  return d_air_kerma_strength*IsoAidIAI125ASeed::dose_rate_constant*
    geometry_function_value*radial_dose_function_value*
    anisotropy_function_value/IsoAidIAI125ASeed::ref_geometry_func_value;
}

// Return the total dose at time = infinity at a given point (cGy)
double IsoAidIAI125ASeed::getTotalDose( const double x, 
				      const double y, 
				      const double z ) const
{
  return getDoseRate( x, y, z )/BrachytherapySeed::i125_decay_constant;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end IsoAidIAI125ASeed.cpp
//---------------------------------------------------------------------------//
