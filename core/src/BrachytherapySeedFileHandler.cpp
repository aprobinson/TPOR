//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedFileHandler.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed hdf5 file handler class definition
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapySeedFileHandler.hpp"
#include "BrachytherapySeedHelpers.hpp"

namespace TPOR{

// Constructor
BrachytherapySeedFileHandler::BrachytherapySeedFileHandler( 
					    const std::string &seed_file_name )
  : d_hdf5_file()
{
  d_hdf5_file.openHDF5FileAndReadOnly( seed_file_name );
}

// Destructor
BrachytherapySeedFileHandler::~BrachytherapySeedFileHandler()
{
  d_hdf5_file.closeHDF5File();
}

// Return the mesh dimensions
void BrachytherapySeedFileHandler::getMeshDimensions( 
				       std::vector<unsigned> &mesh_dimensions )
{
  d_hdf5_file.readArrayFromGroupAttribute( mesh_dimensions,
					   "/",
					   "mesh_dimensions" );
}

// Return the mesh element dimensions
void BrachytherapySeedFileHandler::getMeshElementDimensions( 
				      std::vector<double> &element_dimensions )
{
  d_hdf5_file.readArrayFromGroupAttribute( element_dimensions,
					   "/",
					   "mesh_element_dimensions" );
}

// Return the seed position indices
void BrachytherapySeedFileHandler::getSeedPosition( 
					 std::vector<unsigned> &seed_position )
{
  d_hdf5_file.readArrayFromGroupAttribute( seed_position,
					   "/",
					   "seed_position" );
}

// Return the seed mesh for the desired seed
void BrachytherapySeedFileHandler::getSeedDataMesh( 
				  std::vector<double> &seed_data_mesh,
				  const TPOR::BrachytherapySeedType seed_type,
				  const double air_kerma_strength )
{
  std::string dataset_location = "/";
  dataset_location += brachytherapySeedName( seed_type );
  
  d_hdf5_file.readArrayFromDataSet( seed_data_mesh, dataset_location );

  // Scale the seed data mesh by the air_kerma_strength
  scaleSeedDataMesh( seed_data_mesh, air_kerma_strength );
}

// Scale the seed data mesh by a desired air kerma strength
void BrachytherapySeedFileHandler::scaleSeedDataMesh( 
					   std::vector<double> &seed_data_mesh,
					   const double scale_factor )
{
  std::vector<double>::iterator start = seed_data_mesh.begin(),
    end = seed_data_mesh.end();

  while( start != end )
  {
    *start *= scale_factor;

    ++start;
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedFileHandler.cpp
//---------------------------------------------------------------------------//
