//---------------------------------------------------------------------------//
//!
//! \file   Amersham6733Seed.hpp
//! \author Alex Robinson
//! \brief  Amersham 6733 brachytherapy seed class declaration
//!
//---------------------------------------------------------------------------//

#ifndef AMERSHAM_6733_SEED_HPP
#define AMERSHAM_6733_SEED_HPP

// Boost Includes
#include <boost/array.hpp>

// TPOR Includes
#include "BrachytherapySeed.hpp"

namespace TPOR{

//! Amersham 6733 brachytherapy seed
class Amersham6733Seed : public BrachytherapySeed
{

public:

  //! Constructor
  Amersham6733Seed( const double air_kerma_strength );

  //! Destructor
  virtual ~Amersham6733Seed()
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
  static const BrachytherapySeedType seed_type = AMERSHAM_6733_SEED;

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
  static const int af_radii = 7;
  static const int af_angles = 12;
  static const boost::array<double,af_radii> anisotropy_function_radii;
  static const boost::array<double,af_angles*(af_radii+1)> anisotropy_function;
  
  double d_air_kerma_strength;
};

} // end TPOR namespace

#endif // end AMERSHAM_6733_SEED_HPP

//---------------------------------------------------------------------------//
// end Amersham6733Seed.hpp
//---------------------------------------------------------------------------//
