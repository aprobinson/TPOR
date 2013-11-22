//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeed.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed base class declaration
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_HPP
#define BRACHYTHERAPY_SEED_HPP

// Std Lib Includes
#include <string>

// TPOR Includes
#include "BrachytherapySeedType.hpp"

namespace TPOR{

//! Brachytherapy seed base class
class BrachytherapySeed
{

public:

  //! Constructor
  BrachytherapySeed()
  { /* ... */ }

  //! Destructor
  virtual ~BrachytherapySeed()
  { /* ... */ }

  //! Return the seed type
  virtual BrachytherapySeedType getSeedType() const = 0;

  //! Return the seed name
  virtual std::string getSeedName() const = 0;

  //! Return the seed strength (air-kerma strength)
  virtual double getSeedStrength() const = 0;

  //! Return the dose rate at a given point (cGy/hr)
  virtual double getDoseRate( const double x, 
			      const double y, 
			      const double z ) const = 0;

  //! Return the total dose at time = infinity at a given point (cGy)
  virtual double getTotalDose( const double x, 
			       const double y, 
			       const double z ) const = 0;

protected:
  
  // I-125 decay constant (1/h)
  static const double i125_decay_constant;
  
  // Pd-103 decay constant (1/h)
  static const double pd103_decay_constant;

  //! Calculate the radius
  static double calculateRadius( const double x,
				 const double y,
				 const double z );

  //! Calculate the polar angle (radians)
  static double calculatePolarAngle( const double r,
				     const double z );

  //! Convert an angle in radians to an angle in degrees
  static double convertAngleToDegrees( const double theta );
					    
  //! Calculate the angle (rad) subtended by the tips of the seed w.r.t. point
  static double calculateAngleSubtendedBySeed( const double r,
					       const double theta,
					       const double Leff );

  //! Evaluate the geometry function at a given point
  static double evaluateGeometryFunction( const double r,
					  const double z,
					  const double Leff );
					  
  //! Evaluate the radial dose function at a given radius
  static double evaluateRadialDoseFunction( const double r,
					    const double *radial_dose_func,
					    const int number_of_radii,
					    const double *cunningham_coeffs );

  //! Evaluate the 2D anisotropy function at a given point
  static double evaluateAnisotropyFunction( 
					   const double r, 
					   const double theta,
					   const double *anisotropy_func,
					   const double *anisotropy_func_radii,
					   const int number_of_angles,
					   const int number_of_radii );
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeed.hpp
//---------------------------------------------------------------------------//
