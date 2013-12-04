//---------------------------------------------------------------------------//
//!
//! \file   BrachyDynamicWeightSeedPosition.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position with dynamics weight class def.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// TPOR Includes
#include "BrachytherapyDynamicWeightSeedPosition.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Constructor
BrachytherapyDynamicWeightSeedPosition::BrachytherapyDynamicWeightSeedPosition(
		        const int x_index, 
			const int y_index,
			const int z_index,
			const double weight,
			const double* weight_multiplier,
		        const boost::shared_ptr<BrachytherapySeedProxy> &seed )
  : BrachytherapySeedPosition( x_index, y_index, z_index, weight, seed ),
    d_dynamic_weight( weight ),
    d_weight_multiplier( weight_multiplier )
{
  // Make sure that the weight is valid
  testPrecondition( weight == weight ); // Nan test
  testPrecondition( weight > 0.0 );
  testPrecondition( weight != std::numeric_limits<double>::infinity() );
  // Make sure that the seed is valid
  testPrecondition( seed );
}

// Multiply the base weight by a value
void BrachytherapyDynamicWeightSeedPosition::updateWeight()
{
  d_dynamic_weight = d_weight*(*d_weight_multiplier);
}

// Multiply the dynamic weight by a multiplier
void BrachytherapyDynamicWeightSeedPosition::multiplyWeight( 
						      const double multiplier )
{
  d_dynamic_weight *= multiplier;
}

// Less-than operator for BrachytherapySeedPositions
bool BrachytherapyDynamicWeightSeedPosition::operator < ( 
			const BrachytherapyDynamicWeightSeedPosition &operand )
{
  return d_dynamic_weight < operand.d_dynamic_weight;
}

// Greater-than operator for BrachytherapySeedPositions
bool BrachytherapyDynamicWeightSeedPosition::operator > ( 
			const BrachytherapyDynamicWeightSeedPosition &operand )
{
  return d_dynamic_weight > operand.d_dynamic_weight;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyDynamicWeightSeedPosition.cpp
//---------------------------------------------------------------------------//
