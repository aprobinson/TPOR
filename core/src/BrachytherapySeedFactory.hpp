//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedFactory.hpp
//! \author Alex Robinson
//! \brief  Factory class declaration for creating brachytherapy seed objects
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_FACTORY_HPP
#define BRACHYTHERAPY_SEED_FACTORY_HPP

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeed.hpp"

namespace TPOR{

//! Brachytherapy seed factory class
class BrachytherapySeedFactory
{
  
public:
  
  //! Typedef for a brachytherapy seed class pointer
  typedef boost::shared_ptr<BrachytherapySeed> BrachytherapySeedPtr;

  //! Seed construction method
  static BrachytherapySeedPtr createSeed( 
					const BrachytherapySeedType seed_name,
					const double air_kerma_strength,
					const double orientation_angle = 0.0 );
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_FACTORY_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedFactory.hpp
//---------------------------------------------------------------------------//
