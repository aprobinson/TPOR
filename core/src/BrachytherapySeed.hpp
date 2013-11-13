//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeed.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed base class declaration
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_HPP
#define BRACHYTHERAPY_SEED_HPP

// TPOR Includes
#include "SeedType.hpp"

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
  virtual SeedType getSeedType() const = 0;

  //! Return the dose rate at a given point
  virtual double getDoseRate( const double x, 
			      const double y, 
			      const double z ) const = 0;

  //! Return the total dose at time = infinity at a given point
  virtual double getTotalDose( const double x, 
			       const double y, 
			       const double z ) const = 0;

protected:

  //! Calculate the radius
  double calculateRadius( const double x,
			  const double y );

  //! Calculate the polar angle (radians)
  double calculatePolarAngleRadians( const double r,
				     const double z );

  //! Calculate the polar angle (degrees)
  double calculatePolarAngleDegrees( const double r,
				     const double z );
					    
  //! Calculate the angle (rad) subtended by the tips of the seed w.r.t. point
  double calculateAngleSubtendedBySeed( const double r,
					const double z,
					const double Leff );

  //! Evaluate the geometry function at a given point
  double evaluateGeometryFunction( const double r,
				   const double z,
				   const double Leff );
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeed.hpp
//---------------------------------------------------------------------------//
