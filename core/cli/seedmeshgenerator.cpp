//---------------------------------------------------------------------------//
//!
//! \file   seedmeshgenerator.cpp
//! \author Alex Robinson
//! \brief  C++ command-line interface for generating seed data meshes.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdlib.h>

// TPOR Includes
#include "BrachytherapySeedFactory.hpp"
#include "BrachytherapySeedHelpers.hpp"
#include "HDF5FileHandler.hpp"

//! C++ command-line interface for create seed data meshes
int main()
{
  // Set the size of the seed data mesh
  unsigned mesh_x_dim = 151;
  unsigned mesh_y_dim = 151;
  unsigned mesh_z_dim = 31;

  // Set the seed center position
  unsigned seed_x_index = 75;
  unsigned seed_y_index = 75;
  unsigned seed_z_index = 15;

  // Set the size of the mesh elements (cm)
  double element_x_dim = 0.1;
  double element_y_dim = 0.1;
  double element_z_dim = 0.5;

  // Create the vector that will store the seed data mesh
  std::vector<double> seed_mesh( mesh_x_dim*mesh_y_dim*mesh_z_dim );

  // Create the HDF5 file that will store the seed data meshes
  TPOR::HDF5FileHandler hdf5_file;
  hdf5_file.openHDF5FileAndOverwrite( "BrachytherapySeeds.h5" );
  
  // Create a root level attribute for the mesh dimensions
  std::vector<unsigned> mesh_dims( 3 );
  mesh_dims[0] = mesh_x_dim;
  mesh_dims[1] = mesh_y_dim;
  mesh_dims[2] = mesh_z_dim;

  hdf5_file.writeArrayToGroupAttribute( mesh_dims, "/", "mesh_dimensions" );

  // Create a root level attribute for the seed center position
  std::vector<unsigned> seed_position( 3 );
  seed_position[0] = seed_x_index;
  seed_position[1] = seed_y_index;
  seed_position[2] = seed_z_index;

  hdf5_file.writeArrayToGroupAttribute( seed_position, "/", "seed_position" );
  
  // Create a root level attribute for the mesh element dimensions
  std::vector<double> element_dims( 3 );
  element_dims[0] = element_x_dim;
  element_dims[1] = element_y_dim;
  element_dims[2] = element_z_dim;

  hdf5_file.writeArrayToGroupAttribute( element_dims, 
					"/", 
					"mesh_element_dimensions" );

  // The brachytherapy seed
  TPOR::BrachytherapySeedFactory::BrachytherapySeedPtr seed;

  // Generate a data mesh for every seed type (cGy)
  for( unsigned seed_id = TPOR::SEED_min; seed_id <= TPOR::SEED_max; ++seed_id)
  {
    TPOR::BrachytherapySeedType seed_type = 
      TPOR::unsignedToBrachytherapySeedType( seed_id );
    
    seed = TPOR::BrachytherapySeedFactory::createSeed( seed_type, 1.0 );
    
    std::cout << "generating data mesh for " << seed->getSeedName() << "..."
  	      << std::endl;
    for( int k = 0; k < mesh_z_dim; ++k )
    {
      double z_distance = (k - (int)seed_z_index)*element_z_dim;
      
      for( int j = 0; j < mesh_y_dim; ++j )
      {
  	double y_distance = (j - (int)seed_y_index)*element_y_dim;
	
  	for( int i = 0; i < mesh_x_dim; ++i )
  	{
  	  double x_distance = (i - (int)seed_x_index)*element_x_dim;
	  
  	  unsigned index = i + j*mesh_x_dim + k*mesh_x_dim*mesh_y_dim;
	  
  	  seed_mesh[index] = seed->getTotalDose( x_distance,
  						 y_distance,
  						 z_distance );
  	}
      }
    }

    // Store this data mesh
    hdf5_file.writeArrayToDataSet( seed_mesh, "/" + seed->getSeedName() );

    // Clear the seed mesh for good measure
    std::fill( seed_mesh.begin(), seed_mesh.end(), 0.0 );
  }
}

//---------------------------------------------------------------------------//
// end seedmeshgenerator.cpp
//---------------------------------------------------------------------------//
