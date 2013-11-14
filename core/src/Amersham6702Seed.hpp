//---------------------------------------------------------------------------//
//!
//! \file   Amersham6702Seed.hpp
//! \author Alex Robinson
//! \brief  Amersham 6702 brachytherapy seed class declaration
//!
//---------------------------------------------------------------------------//

#ifndef AMERSHAM_6702_SEED_HPP
#define AMERSHAM_6702_SEED_HPP

// Boost Includes
#include <boost/array.hpp>

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

  static const BrachytherapySeedType seed_type = AMERSHAM_6702_SEED;
  static const double decay_constant;
  static const double effective_length;
  static const double ref_geometry_func_value;
  static const double dose_rate_constant;
  static const boost::array<double,16*2> radial_dose_function;
  static const boost::array<double,5> cunningham_fit_coeffs;
  static const boost::array<double,6> anisotropy_function_radii;
  static const boost::array<double,12*7> anisotropy_function;
  
  double d_air_kerma_strength;
};

} // end TPOR namespace

#endif // end AMERSHAM_6702_SEED_HPP

//---------------------------------------------------------------------------//
// end Amersham6702Seed.hpp
//---------------------------------------------------------------------------//
