//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySetCoverSeedPosition.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position for conducting a greedy set cover def.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// TPOR Includes
#include "BrachytherapySetCoverSeedPosition.hpp"

namespace TPOR{

// Constructor
BrachytherapySetCoverSeedPosition::BrachytherapySetCoverSeedPosition(
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
			const boost::shared_ptr<BrachytherapySeedProxy> &seed )
  : BrachytherapySeedPosition( x_index, y_index, z_index, cost, seed ),
    d_dynamic_weight( cost ),
    d_prescribed_dose( prescribed_dose ),
    d_dose_distribution( dose_distribution ),
    d_prostate_mask( prostate_mask ),
    d_mesh_x_dimension( mesh_x_dimension ),
    d_mesh_y_dimension( mesh_y_dimension ),
    d_mesh_z_dimension( mesh_z_dimension )  
{
  // Make sure that the cost is valid
  testPrecondition( cost == cost ); // Nan test
  testPrecondition( cost > 0.0 );
  testPrecondition( cost != std::numeric_limits<double>::infinity() );
  // Make sure that the dose_distribution is valid
  testPrecondition( dose_distribution );
  testPrecondition( dose_distribution->size() > 0 );
  // Make sure that the prostate mask is valid
  testPrecondition( prostate_mask );
  testPrecondition( prostate_mask->size() > 0 );
  // Make sure that the mesh_dimensions are valid
  testPrecondition( x_index < mesh_x_dimension );
  testPrecondition( y_index < mesh_y_dimension );
  testPrecondition( z_index < mesh_z_dimension );
  // Make sure that the seed is valid
  testPrecondition( seed );
}

// Recompute the dynamic weight by computing the set coverage
void BrachytherapySetCoverSeedPosition::updateWeight()
{
  double coverage = 0.0;
  double future_dose;
  
  for( int k = 0; k < d_mesh_z_dimension; ++k )
  {
    for( int j = 0; j < d_mesh_y_dimension; ++j )
    {
      for( int i = 0; i < d_mesh_x_dimension; ++i )
      {
	unsigned index = i + j*d_mesh_x_dimension + 
	  k*d_mesh_x_dimension*d_mesh_y_dimension;

	if( (*d_prostate_mask)[index] && 
	    (*d_dose_distribution)[index] < d_prescribed_dose )
	{
	  future_dose = (*d_dose_distribution)[index] +
	    d_seed->getTotalDose( i - d_x_index, j - d_y_index, k - d_z_index);
	  
	  if( future_dose < d_prescribed_dose )
	    coverage += future_dose - (*d_dose_distribution)[index];
	  else
	    coverage += d_prescribed_dose - (*d_dose_distribution)[index];
	}
      }
    }
  }

  if( coverage == 0.0 ) 
    d_dynamic_weight = std::numeric_limits<double>::infinity();
  else
    d_dynamic_weight = d_weight/coverage;
}

// Multiply the dynamic weight by a multiplier
void BrachytherapySetCoverSeedPosition::multiplyWeight( 
						      const double multiplier )
{
  d_dynamic_weight *= multiplier;
}

// Comparison methods
bool BrachytherapySetCoverSeedPosition::operator < ( 
			     const BrachytherapySetCoverSeedPosition &operand )
{
  return d_dynamic_weight < operand.d_dynamic_weight;
}

bool BrachytherapySetCoverSeedPosition::operator > ( 
			     const BrachytherapySetCoverSeedPosition &operand )
{
  return d_dynamic_weight < operand.d_dynamic_weight;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySetCoverSeedPosition.cpp
//---------------------------------------------------------------------------//
