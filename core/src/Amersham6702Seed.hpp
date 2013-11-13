//---------------------------------------------------------------------------//
//!
//! \file   Amersham6702Seed.hpp
//! \author Alex Robinson
//! \brief  Amersham 6702 brachytherapy seed class declaration
//!
//---------------------------------------------------------------------------//

#ifndef AMERSHAM_6702_SEED_HPP
#define AMERSHAM_6702_SEED_HPP

// TPOR Includes
#include "BrachytherapySeed.hpp"

namespace TPOR{

//! Amersham 6702 brachytherapy seed
class Amersham6702Seed : public BrachytherapySeed
{

public:

  //! Constructor
  Amersham6702Seed( const double air_kerma_strength );

  //! Destructor
  virtual ~Amersham6702Seed()
  { /* ... */ }
  
  //! Return the seed type
  SeedType getSeedType() const;

  //! Return the dose rate at a given point
  double getDoseRate( const double x, 
		      const double y, 
		      const double z ) const;

  //! Return the total dose at time = infinity at a given point
  double getTotalDose( const double x, 
		       const double y, 
		       const double z ) const;

protected:
  
  //! Evaluate the radial dose function
  static double evaluateRadialDoseFunction( const double r );

  //! Evaluate the 2D anisotropy function
  static double evaluateAnisotropyFunction( const double r, 
					    const double theta );

private:

  static const SeedType seed_type = AMERSHAM_6702_SEED;
  static const double decay_constant;
  static const double effective_length;
  static const double dose_rate_constant;
  static const double radial_dose_function[2][16];
  static const double cunningham_radial_dose_fit_coeffs[5];
  static const double two_dim_anisotropy_function_radii[6];
  static const double two_dim_anisotropy_function[7][11];
  
  double d_air_kerma_strength;
};

} // end TPOR namespace

#endif // end AMERSHAM_6702_SEED_HPP

//---------------------------------------------------------------------------//
// end Amersham6702Seed.hpp
//---------------------------------------------------------------------------//
