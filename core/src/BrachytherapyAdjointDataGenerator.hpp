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

// TPOR Includes
#include "BrachytherapySeedFactory.hpp"

namespace TPOR{

//! Adjoint data generator class 
class BrachytherapyAdjointDataGenerator
{

public:

  //! Constructor
  BrachytherapyAdjointDataGenerator( 
		    const BrachytherapySeedFactory::BrachytherapySeedPtr &seed,
		    const double mesh_element_x_dim,
		    const double mesh_element_y_dim,
		    const double mesh_element_z_dim );

  //! Destructor
  ~BrachytherapyAdjointDataGenerator()
  { /* ... */ }

  //! Calculate the adjoint dose
  void calculateAdjointDose( std::vector<double> &organ_adjoint_data,
			     const std::vector<bool> &organ_mask,
			     const unsigned mesh_x_dim,
			     const unsigned mesh_y_dim,
			     const unsigned mesh_z_dim );

private:

  //! Calculate the average dose to the organ at a seed location
  double calculateAverageDoseToOrgan( const std::vector<double> &seed_position,
				      const std::vector<bool> &organ_mask,
				      const unsigned mesh_x_dim,
				      const unsigned mesh_y_dim,
				      const unsigned mesh_z_dim );

  // Brachytherapy seed
  BrachytherapySeedFactory::BrachytherapySeedPtr d_seed;

  // Mesh element dimensions
  double d_mesh_element_x_dim;
  double d_mesh_element_y_dim;
  double d_mesh_element_z_dim;
};

} // end TPOR namespace

#endif // end BrachytherapyADJOINT_DATA_GENERATOR_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyAdjointDataGenerator.hpp
//---------------------------------------------------------------------------//
