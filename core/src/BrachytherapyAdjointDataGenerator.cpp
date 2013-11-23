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
		    const BrachytherapySeedFactory::BrachytherapySeedPtr &seed,
		    const double mesh_element_x_dim,
		    const double mesh_element_y_dim,
		    const double mesh_element_z_dim )
  : d_seed( seed ),
    d_mesh_element_x_dim( mesh_element_x_dim ),
    d_mesh_element_y_dim( mesh_element_y_dim ),
    d_mesh_element_z_dim( mesh_element_z_dim )
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

  std::vector<double> seed_position( 3 );

  for( unsigned k = 0; k < mesh_z_dim; ++k )
  {
    seed_position[2] = k*d_mesh_element_z_dim + d_mesh_element_z_dim/2;
    
    for( unsigned j = 0; j < mesh_y_dim; ++j )
    {
      seed_position[1] = j*d_mesh_element_y_dim + d_mesh_element_y_dim/2;
	
      for( unsigned i = 0; i < mesh_x_dim; ++i )
      {
	seed_position[0] = i*d_mesh_element_x_dim + d_mesh_element_x_dim/2;

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

// Calculate the average dose to the organ at a seed location
double BrachytherapyAdjointDataGenerator::calculateAverageDoseToOrgan(
				      const std::vector<double> &seed_position,
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
  double samples = 0;

  std::vector<double> mesh_element_center( 3 );

  for( unsigned k = 0; k < mesh_z_dim; ++k )
  {
    mesh_element_center[2] = k*d_mesh_element_z_dim + d_mesh_element_z_dim/2;
    
    for( unsigned j = 0; j < mesh_y_dim; ++j )
    {
      mesh_element_center[1] = j*d_mesh_element_y_dim + d_mesh_element_y_dim/2;
	
      for( unsigned i = 0; i < mesh_x_dim; ++i )
      {
	mesh_element_center[0] = i*d_mesh_element_x_dim+d_mesh_element_x_dim/2;
	
	if( organ_mask[i+j*mesh_x_dim+k*mesh_x_dim*mesh_y_dim] )
	{
	  dose += 
	    d_seed->getTotalDose( mesh_element_center[0]-seed_position[0],
				  mesh_element_center[1]-seed_position[1],
				  mesh_element_center[2]-seed_position[2] );

	  ++samples;
	}
      }
    }
  }

  // cGy/source
  double average_dose = dose/samples;
  
  // Make sure that the average dose calculated is valid
  testPostcondition( average_dose >= 0.0 );
  testPostcondition( average_dose != std::numeric_limits<double>::infinity() );
  testPostcondition( average_dose == average_dose ); // Nan test

  return average_dose;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyAdjointDataGenerator.cpp
//---------------------------------------------------------------------------//
