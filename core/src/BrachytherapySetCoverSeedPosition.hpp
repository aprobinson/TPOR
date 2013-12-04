//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySetCoverSeedPosition.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position for conducting a greedy set cover decl.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SET_COVER_SEED_POSITION_HPP
#define BRACHYTHERAPY_SET_COVER_SEED_POSITION_HPP

// Std Lib Includes
#include <vector>
#include <string>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedPosition.hpp"
#include "BrachytherapySeedProxy.hpp"

namespace TPOR{

//! Class that stores a brachytherapy seed, posiiton indices, and weight
class BrachytherapySetCoverSeedPosition : public BrachytherapySeedPosition
{

public:

  //! Constructor
  BrachytherapySetCoverSeedPosition(
		       const int x_index, 
		       const int y_index,
		       const int z_index,
		       const double cost,
		       const double prescribed_dose,
		       const std::vector<double>* dose_distribution,
		       const std::vector<bool>* prostate_mask,
		       const unsigned mesh_x_dimension,
		       const unsigned mesh_y_dimension,
		       const unsigned mesh_z_dimension,
		       const boost::shared_ptr<BrachytherapySeedProxy> &seed );

  //! Destructor
  ~BrachytherapySetCoverSeedPosition()
  { /* ... */ }

  //! Recompute the dynamic weight by computing the set coverage
  void updateWeight();

  //! Multiply the dynamic weight by a multiplier
  void multiplyWeight( const double multiplier );

  //! Comparison methods
  bool operator < ( const BrachytherapySetCoverSeedPosition &operand );
  
  bool operator > ( const BrachytherapySetCoverSeedPosition &operand );

private:

  // Weight of this seed position (set cost/set coverage)
  double d_dynamic_weight;

  // Prescribed dose
  double d_prescribed_dose;
  
  // Dose distribution used to determine seed position set coverage
  const std::vector<double>* d_dose_distribution;
  
  // Prostate mask whose elements represent both the elements of the set cover
  const std::vector<bool>* d_prostate_mask;

  // Dimensions of the dose distribution and prostate mask
  unsigned d_mesh_x_dimension;
  unsigned d_mesh_y_dimension;
  unsigned d_mesh_z_dimension;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SET_COVER_SEED_POSITION_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySetCoverSeedPosition.hpp
//---------------------------------------------------------------------------//
