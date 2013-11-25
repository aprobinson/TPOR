//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyAdjointDataGenerator.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy adjoint data generator class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_ADJOINT_DATA_GENERATOR_HPP
#define BRACHYTHERAPY_ADJOINT_DATA_GENERATOR_HPP

// Std Lib Includes
#include <vector>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedProxy.hpp"

namespace TPOR{

//! Adjoint data generator class
class BrachytherapyAdjointDataGenerator
{

public:

  //! Constructor
  BrachytherapyAdjointDataGenerator( 
		       const boost::shared_ptr<BrachytherapySeedProxy> &seed );
  
  //! Destructor
  ~BrachytherapyAdjointDataGenerator()
  { /* ... */ }

  //! Calculate the adjoint dose
  void calculateAdjointDose( std::vector<double> &organ_adjoint_data,
			     const std::vector<bool> &organ_mask,
			     const unsigned mesh_x_dim,
			     const unsigned mesh_y_dim,
			     const unsigned mesh_z_dim );

  //! Calculate the adjoint dose in the prostate only
  void calculateCondensedAdjointDose( std::vector<double> &organ_adjoint_data,
				      const std::vector<bool> &organ_mask,
				      const std::vector<bool> &prostate_mask,
				      const unsigned mesh_x_dim,
				      const unsigned mesh_y_dim,
				      const unsigned mesh_z_dim );

private:

  //! Calculate the average dose to the organ at a seed location
  double calculateAverageDoseToOrgan( 
				    const std::vector<int> &seed_position,
				    const std::vector<bool> &organ_mask,
				    const unsigned mesh_x_dim,
				    const unsigned mesh_y_dim,
				    const unsigned mesh_z_dim );

  // Brachytherapy seed
  boost::shared_ptr<BrachytherapySeedProxy> d_seed;
};

} // end TPOR namespace

#endif // end BrachytherapyADJOINT_DATA_GENERATOR_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyAdjointDataGenerator.hpp
//---------------------------------------------------------------------------//
