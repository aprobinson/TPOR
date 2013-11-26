//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedPosition_def.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position class template definitions.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_POSITION_DEF_HPP
#define BRACHYTHERAPY_SEED_POSITION_DEF_HPP

namespace TPOR{

// Map the dose from the seed at this position
template<typename EqualOp>
void BrachytherapySeedPosition::mapSeedDoseDistribution( 
					std::vector<double> &dose_mesh,
					const unsigned mesh_x_dimension,
					const unsigned mesh_y_dimension,
					const unsigned mesh_z_dimension ) const
{
  // Make sure that the mesh_dimensions are valid
  testPrecondition( d_x_index < mesh_x_dimension );
  testPrecondition( d_y_index < mesh_y_dimension );
  testPrecondition( d_z_index < mesh_z_dimension );

  for( int k = 0; k < mesh_z_dimension; ++k )
  {
    for( int j = 0; j < mesh_y_dimension; ++j )
    {
      for( int i = 0; i < mesh_x_dimension; ++i )
      {
	unsigned index = i + j*mesh_x_dimension + 
	  k*mesh_x_dimension*mesh_y_dimension;

	double dose = d_seed->getTotalDose( i - d_x_index,
					    j - d_y_index,
					    k - d_z_index );
	
	EqualOp::set( dose_mesh[index], dose );
      }
    }
  }
}

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_POSITION_DEF_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedPosition_def.hpp
//---------------------------------------------------------------------------//
