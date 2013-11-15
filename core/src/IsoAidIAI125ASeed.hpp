//---------------------------------------------------------------------------//
//!
//! \file   IsoAidIAI125ASeed.hpp
//! \author Alex Robinson
//! \brief  IsoAid IAI-125A brachytherapy seed class declaration
//!
//---------------------------------------------------------------------------//

#ifndef ISOAID_IAI_125A_SEED_HPP
#define ISOAID_IAI_125A_SEED_HPP

// Boost Includes
#include <boost/array.hpp>

// TPOR Includes
#include "BrachytherapySeed.hpp"

namespace TPOR{

//! Best2301 brachytherapy seed
class IsoAidIAI125ASeed : public BrachytherapySeed
{

public:

  //! Constructor
  IsoAidIAI125ASeed( const double air_kerma_strength );

  //! Destructor
  virtual ~IsoAidIAI125ASeed()
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
  static const BrachytherapySeedType seed_type = ISOAID_IAI_125A_SEED;

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
  static const int af_radii = 6;
  static const int af_angles = 11;
  static const boost::array<double,af_radii> anisotropy_function_radii;
  static const boost::array<double,af_angles*(af_radii+1)> anisotropy_function;
  
  double d_air_kerma_strength;
};

} // end TPOR namespace

#endif // end ISOAID_IAI_125A_SEED_HPP

//---------------------------------------------------------------------------//
// end IsoAidIAI125ASeed.hpp
//---------------------------------------------------------------------------//
