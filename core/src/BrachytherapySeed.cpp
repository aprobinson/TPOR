//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeed.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed base class definition
//!
//---------------------------------------------------------------------------//
 
// Std Lib Includes
#include <math.h>
#include <limits>
#include <iostream>

// TPOR Includes
#include "BrachytherapySeed.hpp"
#include "BinarySearch.hpp"
#include "Interpolation.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Set the I-125 decay constant (1/h) - half-life = 59.4 days
const double BrachytherapySeed::i125_decay_constant = log(2)/(59.4*24);

// Set the Pd-103 decay constant (1/h) - half-life = 16.991 days
const double BrachytherapySeed::pd103_decay_constant = log(2)/(16.991*24);

// Calculate the radius
double BrachytherapySeed::calculateRadius( const double x,
					   const double y,
					   const double z)
{
  // Make sure that the coordinate is valid
  testPrecondition( x < std::numeric_limits<double>::infinity() );
  testPrecondition( x == x ); // Nan test
  testPrecondition( y < std::numeric_limits<double>::infinity() );
  testPrecondition( y == y ); // Nan test
  testPrecondition( z < std::numeric_limits<double>::infinity() );
  testPrecondition( z == z ); // Nan test
  
  return sqrt( x*x + y*y + z*z );
}

// Calculate the polar angle (radians)
double BrachytherapySeed::calculatePolarAngle( const double r, const double z )
{ 
  // Make sure that the radius is valid
  testPrecondition( r >= 0.0 );
  testPrecondition( r < std::numeric_limits<double>::infinity() );
  testPrecondition( r == r ); // Nan test
  // Make sure that the z value is valid
  testPrecondition( z < std::numeric_limits<double>::infinity() );
  testPrecondition( z == z ); // Nan test
  
  double theta;

  // Due to assumed symmetry, the absolute value of z should be used.
  // This also assures that theta is always in [0,pi/2]
  if( r != 0.0 )
    theta = acos(fabs(z)/r);
  else
    theta = acos(0.0); // pi/2
  
  return theta;
}

// Convert an angle in radians to an angle in degrees
double BrachytherapySeed::convertAngleToDegrees( const double theta )
{
  // Make sure theta is in radians
  testPrecondition( theta >= 0.0 );
  testPrecondition( theta <= acos(0.0) );
  
  return theta*180/acos(-1.0);
}

// Calculate the angle (rad) subtended by the tips of the seed w.r.t. point
/*! \details theta should be in radians
 */
double BrachytherapySeed::calculateAngleSubtendedBySeed( const double r,
							 const double theta,
							 const double Leff )
{
  // Make sure that the radius is valid
  testPrecondition( r >= 0.0 );
  testPrecondition( r < std::numeric_limits<double>::infinity() );
  testPrecondition( r == r ); // Nan test
  // Make sure theta is in radians
  testPrecondition( theta >= 0.0 );
  testPrecondition( theta <= acos(0.0) );
  // Make sure that the effective length is valid
  testPrecondition( Leff > 0.0 );
  
  double z_point = r*cos(theta);
  double r_perp_point = r*sin(theta);
  
  double z_seed_max = Leff/2;
  double z_seed_min = -z_seed_max;

  // Compute the dot product of the two vectors formed from the point to the
  // ends of the seed (v_min is from point to bottom of seed)
  // cos(beta) = v_min*v_max/(||v_min||*||v_max||)
  //           = (rp^2 + (z_seed_min-z)*(z_seed_max-z))/
  //             sqrt((rp^2 + z_seed_min^2)*(rp^2 + z_seed_max^2))

  double r_perp_squared = r_perp_point*r_perp_point;
  double z_min_diff = z_seed_min - z_point;
  double z_max_diff = z_seed_max - z_point;
  
  double numerator = r_perp_squared + z_min_diff*z_max_diff;
  double denominator = sqrt( (r_perp_squared + z_min_diff*z_min_diff)*
			     (r_perp_squared + z_max_diff*z_max_diff) );

  return acos( numerator/denominator );
}

//! Evaluate the geometry function at a given point
/*! \details theta must be in radians. r must be > 0 to avoid the 
 * discontinuity. If theta = 0.0, r must be > Leff to avoid the discontinuity.
 */
double BrachytherapySeed::evaluateGeometryFunction( const double r,
						    const double theta,
						    const double Leff )
{
  // Make sure that the radius is valid
  testPrecondition( r > 0.0 );
  testPrecondition( r < std::numeric_limits<double>::infinity() );
  testPrecondition( r == r ); // Nan test
  // Make sure theta is in radians
  testPrecondition( theta >= 0.0 );
  testPrecondition( theta <= acos(0.0) );
  // Make sure that the effective length is valid
  testPrecondition( Leff > 0.0 );
  // Check if the special case requirement has been violated
  testPrecondition( (theta == 0.0) ? (r > Leff) : true )

  double geometry_function_value;

  if( theta != 0.0 )
  {
    double beta = calculateAngleSubtendedBySeed( r, theta, Leff );
    
    geometry_function_value = beta/(Leff*r*sin(theta));
  }
  else
    geometry_function_value = 1.0/(r*r-Leff*Leff/4.0);

  // The geometry function must return a positive value
  testPostcondition( geometry_function_value >= 0.0 );
  testPostcondition( geometry_function_value < 
		     std::numeric_limits<double>::infinity() );

  return geometry_function_value;  
}

// Evaluate the radial dose function at a given radius
double BrachytherapySeed::evaluateRadialDoseFunction( 
					 const double r,
					 const double *radial_dose_func,
					 const int number_of_radii,
					 const double *cunningham_coeffs )
{
  // Make sure that the radius is valid
  testPrecondition( r >= 0.0 );
  testPrecondition( r < std::numeric_limits<double>::infinity() );
  testPrecondition( r == r ); // Nan test
  
  double dose_function_value;
  
  // Return the minimum function value
  if( r < radial_dose_func[0] )
    dose_function_value = radial_dose_func[number_of_radii];
  
  // Use linear-linear interpolation inside the table
  else if( r <= radial_dose_func[number_of_radii-1] )
  {
    int index = binarySearch( radial_dose_func,
			      radial_dose_func+number_of_radii-1,
			      r );
    
    dose_function_value = linlinInterp( 
				   radial_dose_func[index],
				   radial_dose_func[index+1],
				   r,
				   radial_dose_func[number_of_radii+index],
				   radial_dose_func[number_of_radii+index+1] );
  }

  // Extrapolate using a fitted modified Cunningham eqution
  else 
  {
    double exponent1 = cunningham_coeffs[0]*(r - cunningham_coeffs[3]);
    double exponent2 = cunningham_coeffs[1]*(r - cunningham_coeffs[3]);
    
    dose_function_value = cunningham_coeffs[4]*
      (cunningham_coeffs[2] + exp(exponent1))/
      (cunningham_coeffs[2] + exp(exponent1) + exp(exponent2));
  }

  return dose_function_value;
}

// Evaluate the 2D anisotropy function at a given point
/*! \details theta must be in radians - it will be converted to degrees
 */
double BrachytherapySeed::evaluateAnisotropyFunction( 
					  const double r, 
					  const double theta,
					  const double *anisotropy_func,
					  const double *anisotropy_func_radii,
					  const int number_of_angles,
					  const int number_of_radii )
{
  // Make sure that the radius is valid
  testPrecondition( r >= 0.0 );
  testPrecondition( r < std::numeric_limits<double>::infinity() );
  testPrecondition( r == r ); // Nan test
  // Make sure theta is in radians
  testPrecondition( theta >= 0.0 );
  testPrecondition( theta <= acos(0.0) );
  
  double anisotropy_function_value;

  double theta_degrees = BrachytherapySeed::convertAngleToDegrees( theta );
  
  int theta_index = binarySearch( anisotropy_func,
				  anisotropy_func+number_of_angles-1,
				  theta_degrees );
  
  // Return the minimum function value
  if( r < anisotropy_func_radii[0] )
  {
    anisotropy_function_value = linlinInterp(
			     anisotropy_func[theta_index],
			     anisotropy_func[theta_index+1],
			     theta_degrees,
			     anisotropy_func[number_of_angles+theta_index],
			     anisotropy_func[number_of_angles+theta_index+1] );
  }

  // Use linear-linear interpolation inside the table
  else if( r <= anisotropy_func_radii[number_of_radii-1] )
  {
    int r_index = binarySearch( anisotropy_func_radii,
				anisotropy_func_radii+number_of_radii-1,
				r );

    double lower_anisotropy_function_value = 
      linlinInterp( 
		   anisotropy_func_radii[r_index],
		   anisotropy_func_radii[r_index+1],
		   r,
		   anisotropy_func[(r_index+1)*number_of_angles+theta_index],
		   anisotropy_func[(r_index+2)*number_of_angles+theta_index] );

    double upper_anisotropy_function_value =
      linlinInterp( 
		 anisotropy_func_radii[r_index],
		 anisotropy_func_radii[r_index+1],
		 r,
		 anisotropy_func[(r_index+1)*number_of_angles+theta_index+1],
		 anisotropy_func[(r_index+2)*number_of_angles+theta_index+1] );

    anisotropy_function_value = 
      linlinInterp( anisotropy_func[theta_index],
		    anisotropy_func[theta_index+1],
		    theta_degrees,
		    lower_anisotropy_function_value,
		    upper_anisotropy_function_value );
  }

  // Return the maximum function value
  else
  {
    anisotropy_function_value = linlinInterp(
	     anisotropy_func[theta_index],
	     anisotropy_func[theta_index+1],
	     theta_degrees,
	     anisotropy_func[number_of_radii*number_of_angles+theta_index],
	     anisotropy_func[number_of_radii*number_of_angles+theta_index+1] );
  }

  return anisotropy_function_value;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeed.cpp
//---------------------------------------------------------------------------//

