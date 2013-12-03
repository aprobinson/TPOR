//---------------------------------------------------------------------------//
//! 
//! \file   BrachytherapyAdjointDataGenerator.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy adjoint data generator class definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// TPOR Includes
#include "BrachytherapyAdjointDataGenerator.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Constructor
BrachytherapyAdjointDataGenerator::BrachytherapyAdjointDataGenerator(
		        const boost::shared_ptr<BrachytherapySeedProxy> &seed )
  : d_seed( seed )
{
  // Make sure that a valid seed has been passed
  testPrecondition( seed );
}

// Calculate the adjoint dose
/*! \details The calculated adjoint dose with have units of cGy/source
 */
void BrachytherapyAdjointDataGenerator::calculateAdjointDose( 
			               std::vector<double> &organ_adjoint_data,
				       const std::vector<bool> &organ_mask,
				       const unsigned mesh_x_dim,
				       const unsigned mesh_y_dim,
				       const unsigned mesh_z_dim )
{
  // Make sure that the dimensions passed and the size of the organ_mask
  // are the same
  testPrecondition( organ_mask.size() == mesh_x_dim*mesh_y_dim*mesh_z_dim );

  organ_adjoint_data.resize( organ_mask.size() );

  std::vector<int> seed_position( 3 );

  for( int k = 0; k < mesh_z_dim; ++k )
  {
    seed_position[2] = k;
    
    for( int j = 0; j < mesh_y_dim; ++j )
    {
      seed_position[1] = j;
	
      for( int i = 0; i < mesh_x_dim; ++i )
      {
	seed_position[0] = i;

	organ_adjoint_data[i+j*mesh_x_dim+k*mesh_x_dim*mesh_y_dim] = 
	  calculateAverageDoseToOrgan( seed_position,
				       organ_mask,
				       mesh_x_dim,
				       mesh_y_dim,
				       mesh_z_dim );
      }
    }
  }
}

//! Calculate the adjoint dose in the prostate only
void BrachytherapyAdjointDataGenerator::calculateCondensedAdjointDose( 
				       std::vector<double> &organ_adjoint_data,
				       const std::vector<bool> &organ_mask,
				       const std::vector<bool> &prostate_mask,
				       const unsigned mesh_x_dim,
				       const unsigned mesh_y_dim,
				       const unsigned mesh_z_dim )
{
  // Make sure that the dimensions passed and the size of the organ_mask
  // are the same
  testPrecondition( organ_mask.size() == mesh_x_dim*mesh_y_dim*mesh_z_dim );
  testPrecondition( prostate_mask.size() == organ_mask.size() );

  organ_adjoint_data.resize( organ_mask.size() );

  std::vector<int> seed_position( 3 );

  for( int k = 0; k < mesh_z_dim; ++k )
  {
    seed_position[2] = k;
    
    for( int j = 0; j < mesh_y_dim; ++j )
    {
      seed_position[1] = j;
	
      for( int i = 0; i < mesh_x_dim; ++i )
      {
	seed_position[0] = i;

	if( prostate_mask[i+j*mesh_x_dim+k*mesh_x_dim*mesh_y_dim] )
	{
	  organ_adjoint_data[i+j*mesh_x_dim+k*mesh_x_dim*mesh_y_dim] = 
	    calculateAverageDoseToOrgan( seed_position,
					 organ_mask,
					 mesh_x_dim,
					 mesh_y_dim,
					 mesh_z_dim );
	}
	else
	  organ_adjoint_data[i+j*mesh_x_dim+k*mesh_x_dim*mesh_y_dim] = 0.0;
      }
    }
  }
}

// Calculate the average dose to the organ at a seed location
double BrachytherapyAdjointDataGenerator::calculateAverageDoseToOrgan(
				         const std::vector<int> &seed_position,
					 const std::vector<bool> &organ_mask,
					 const unsigned mesh_x_dim,
					 const unsigned mesh_y_dim,
					 const unsigned mesh_z_dim )
{
  // Make sure that the seed position has only three dimensions
  testPrecondition( seed_position.size() == 3 );
  // Make sure that the dimensions passed and the size of the organ_mask
  // are the same
  testPrecondition( organ_mask.size() == mesh_x_dim*mesh_y_dim*mesh_z_dim );

  double dose = 0.0;
  unsigned samples = 0;

  for( int k = 0; k < mesh_z_dim; ++k )
  {
    for( int j = 0; j < mesh_y_dim; ++j )
    {
      for( int i = 0; i < mesh_x_dim; ++i )
      {
	if( organ_mask[i+j*mesh_x_dim+k*mesh_x_dim*mesh_y_dim] )
	{
	  dose += d_seed->getTotalDose( i - seed_position[0],
					j - seed_position[1],
					k - seed_position[2] );
	  
	  ++samples;
	}
      }
    }
  }

  // cGy/source
  double average_dose = dose/samples;
  
  // Make sure that the average dose calculated is valid
  testPostcondition( average_dose == average_dose ); // Nan test
  testPostcondition( average_dose >= 0.0 );
  testPostcondition( average_dose != std::numeric_limits<double>::infinity() );

  return average_dose;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyAdjointDataGenerator.cpp
//---------------------------------------------------------------------------//
