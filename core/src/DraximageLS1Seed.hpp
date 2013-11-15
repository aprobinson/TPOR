//---------------------------------------------------------------------------//
//!
//! \file   DraximageLS1Seed.hpp
//! \author Alex Robinson
//! \brief  Draximage BrachySeed LS-1 brachytherapy seed class declaration
//!
//---------------------------------------------------------------------------//

#ifndef DRAXIMAGE_LS1_SEED_HPP
#define DRAXIMAGE_LS1_SEED_HPP

// Boost Includes
#include <boost/array.hpp>

// TPOR Includes
#include "BrachytherapySeed.hpp"

namespace TPOR{

//! Best2301 brachytherapy seed
class DraximageLS1Seed : public BrachytherapySeed
{

public:

  //! Constructor
  DraximageLS1Seed( const double air_kerma_strength );

  //! Destructor
  virtual ~DraximageLS1Seed()
  { /* ... */ }

  //! Return the seed type
  BrachytherapySeedType getSeedType() const;

  //! Return the dose rate at a given point (cGy/hr)
  double getDoseRate( const double x,
		      const double y,
		      const double z ) const;

  //! Return the total dose at time = infinity at a given point (cGy)
  double getTotalDose( const double x,
		       const double y,
		       const double z ) const;

private:

  // The seed type
  static const BrachytherapySeedType seed_type = DRAXIMAGE_LS1_SEED;

  // The effective seed length (Leff)
  static const double effective_length;
  
  // The reference value of the geometry function - G(1.0,pi/2)
  static const double ref_geometry_func_value;
  
  // The dose rate constant
  static const double dose_rate_constant;

  // The radial dose function
  static const int rdf_points = 16;
  static const boost::array<double,rdf_points*2> radial_dose_function;

  // The cunningham fit coefficients
  static const boost::array<double,5> cunningham_fit_coeffs;

  // The 2D anisotropy function
  static const int af_radii = 9;
  static const int af_angles = 10;
  static const boost::array<double,af_radii> anisotropy_function_radii;
  static const boost::array<double,af_angles*(af_radii+1)> anisotropy_function;
  
  double d_air_kerma_strength;
};

} // end TPOR namespace

#endif // end DRAXIMAGE_LS1_SEED_HPP

//---------------------------------------------------------------------------//
// end DraximageLS1Seed.hpp
//---------------------------------------------------------------------------//
