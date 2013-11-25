//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedProxy.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed proxy class definition
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapySeedProxy.hpp"
#include "BrachytherapySeedFileHandler.hpp"
#include "BrachytherapySeedHelpers.hpp"

namespace TPOR{

// Constructor
BrachytherapySeedProxy::BrachytherapySeedProxy( 
					 const std::string seed_file_name,
					 const BrachytherapySeedType seed_type,
					 const double air_kerma_strength )
  : d_dose_distribution_mesh(),
    d_mesh_x_dim(),
    d_mesh_y_dim(),
    d_mesh_z_dim(),
    d_seed_x_index(),
    d_seed_y_index(),
    d_seed_z_index(),
    d_seed_type( seed_type ),
    d_seed_name(),
    d_air_kerma_strength( air_kerma_strength )
{
  // Open the seed file
  BrachytherapySeedFileHandler seed_file( seed_file_name );

  // Get the dose distribution for this seed
  seed_file.getSeedDataMesh( d_dose_distribution_mesh,
			     seed_type,
			     air_kerma_strength );

  // Get the seed mesh dimensions
  std::vector<unsigned> mesh_dimensions;
  seed_file.getMeshDimensions( mesh_dimensions );
  
  d_mesh_x_dim = mesh_dimensions[0];
  d_mesh_y_dim = mesh_dimensions[1];
  d_mesh_z_dim = mesh_dimensions[2];
  
  // Get the seed position indices
  std::vector<unsigned> seed_position;
  seed_file.getSeedPosition( seed_position );

  d_seed_x_index = (int)seed_position[0];
  d_seed_y_index = (int)seed_position[1];
  d_seed_z_index = (int)seed_position[2];

  // Get the seed name
  d_seed_name = brachytherapySeedName( seed_type );
}

// Return the seed type
BrachytherapySeedType BrachytherapySeedProxy::getSeedType() const
{
  return d_seed_type;
}

// Return the seed name
std::string BrachytherapySeedProxy::getSeedName() const
{
  return d_seed_name;
}
  
// Return the seed strength (air-kerma strength)
double BrachytherapySeedProxy::getSeedStrength() const
{
  return d_air_kerma_strength;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedProxy.cpp
//---------------------------------------------------------------------------//
