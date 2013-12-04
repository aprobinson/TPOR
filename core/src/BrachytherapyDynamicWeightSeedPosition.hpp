//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyDynamicWeightSeedPosition.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position with dynamic weight class decl.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_DW_SEED_POSITION_HPP
#define BRACHYTHERAPY_DW_SEED_POSITION_HPP

// Std Lib Includes
#include <vector>
#include <string>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedPosition.hpp"
#include "BrachytherapySeedProxy.hpp"

namespace TPOR{

//! Class that stores a brachytherapy seed, position indices, and weight
class BrachytherapyDynamicWeightSeedPosition : public BrachytherapySeedPosition
{

public: 

  //! Constructor
  BrachytherapyDynamicWeightSeedPosition( 
		       const int x_index, 
		       const int y_index,
		       const int z_index,
		       const double weight,
		       const double* weight_multiplier,
		       const boost::shared_ptr<BrachytherapySeedProxy> &seed );

  //! Destructor
  ~BrachytherapyDynamicWeightSeedPosition()
  { /* ... */ }

  //! Multiply the base weight by the weight multiplier (dynamic weight)
  void updateWeight();

  //! Multiply the dynamic weight by a multiplier
  void multiplyWeight( const double multiplier );

  //! Comparison methods
  bool operator < ( const BrachytherapyDynamicWeightSeedPosition &operand );
  
  bool operator > ( const BrachytherapyDynamicWeightSeedPosition &operand );
		    
private:

  // Dynamics Position weight
  double d_dynamic_weight;

  // Weight Multiplier
  const double* d_weight_multiplier;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_DW_SEED_POSITION_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyDynamicWeightSeedPosition.hpp
//---------------------------------------------------------------------------//
