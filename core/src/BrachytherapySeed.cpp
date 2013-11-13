//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeed.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed base class definition
//!
//---------------------------------------------------------------------------//
 
// Std Lib Includes
#include <math.h>

// TPOR Includes
#include "BrachytherapySeed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Calculate the radius
/*! \details The seed is assumed to be aligned with the z-axis. 
 */ 
double BrachytherapySeed::calculateRadius( const double x,
					   const double y )
{
  return sqrt( x*x + y*y );
}

// Calculate the polar angle (radians)
double BrachytherapySeed::calculatePolarAngleRadians( const double r,
						      const double z )
{ 
  double theta;
  
  if( z != 0.0 )
    theta = atan(r/z);
  else
    theta = acos(-1.0)/2.0; // pi/2

  // Due to assumed symmetry, the absolute value of theta should be returned
  return abs(theta)
}

// Calculate the polar angle (degrees)
double BrachytherapySeed::calculatePolarAngleDegrees( const double r,
						      const double z )
{
  double theta = calculatePolarAngleRadians( r, z );
  return theta*180/acos(-1.0)
}

// Calculate the angle (rad) subtended by the tips of the seed w.r.t. point
double BrachytherapySeed::calculateAngleSubtendedBySeed( const double r,
							 const double z,
							 const double Leff )
{
  double z_seed_max = Leff/2;
  double z_seed_min = -z_seed_max;

  // Compute the dot product of the two vectors formed from the ends of the
  // seed to the point (v_min is from point to bottom of seed)
  // cos(beta) = v_min*v_max/(||v_min||*||v_max||)
  //           = (r^2 + (z_seed_min-z_point)*(z_seed_max-z_point))/
  //             sqrt((r^2 + z_seed_min^2)*(r^2 + z_seed_max^2))

  double square_radius = r*r;
  double z_min_diff = z_seed_min - z;
  double z_max_diff = z_seed_max - z;

  double numerator = square_radius + z_min_diff*z_max_diff;
  double denominator = sqrt( (square_radius + z_min_diff*z_min_diff)*
			     (square_radius + z_max_diff*z_max_diff) );

  return acos( numerator/denominator );
}

//! Evaluate the geometry function at a given point
double BrachytherapySeed::evaluateGeometryFunction( const double r,
						    const double z,
						    const double Leff )
{
  double geometry_function_value;

  double theta = calculatePolarAngleRadians( r, z );
  
  if( theta != 0.0 )
  {
    double beta = calculateAngleSubtendedBySeed( r, z, Leff );
    
    geometry_function_value = beta/(Leff*r*sin(theta));
  }
  else
    geometry_function_value = 1.0/(r*r-Leff*Leff/4.0);

  return geometry_function_value;  
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeed.cpp
//---------------------------------------------------------------------------//

