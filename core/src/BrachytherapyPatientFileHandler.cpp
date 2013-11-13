//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyPatientFileHandler.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy patient hdf5 file handler class definition
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapyPatientFileHandler.hpp"

namespace TPOR{

// Constructor
BrachytherapyPatientFileHandler::BrachytherapyPatientFileHandler( 
					 const std::string &patient_file_name )
  : d_hdf5_file()
{
  d_hdf5_file.openHDF5FileAndReadOnly( patient_file_name );
}

// Destructor
BrachytherapyPatientFileHandler::~BrachytherapyPatientFileHandler()
{
  d_hdf5_file.closeHDF5File()
}

// Return the patient name
void BrachytherapyPatientFileHandler::getPatientName( 
						    std::string &patient_name )
{
  std::vector<char> patient_name_string;

  d_hdf5_file.readArrayFromGroupAttribute( patient_name_string,
					   "/",
					   "patient_name" );

  patient_name.assign( &patient_name_string[0], patient_name_string.size() );
}

// Return the mesh element dimensions
void BrachytherapyPatientFileHandler::getMeshElementDimensions( 
				  std::vector<double> &mesh_element_dimensions)
{
  d_hdf5_file.readArrayFromGroupAttribute( mesh_element_dimensions,
					   "/",
					   "mesh_element_dimensions" );
}

// Return the organ mask dimensions
void BrachytherapyPatientFileHandler::getOrganMeshDimensions( 
				       std::vector<unsigned> &mesh_dimensions )
{
  d_hdf5_file.readArrayFromGroupAttribute( mesh_dimensions,
					   "/organ_masks",
					   "mesh_dimensions" );
}

// Return the prostate mask
void BrachytherapyPatientFileHandler::getProstateMask( 
					     std::vector<bool> &prostate_mask )
{
  d_hdf5_file.readArrayFromDataSet( prostate_mask,
				    "/organ_masks/prostate_mask" );
}

// Return the prostate mask volume
void BrachytherapyPatientFileHandler::getProstateMaskVolume( 
					       unsigned &prostate_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( prostate_mask_volume,
					     "/organ_masks/prostate_mask",
					     "volume" );
}

// Return the urethra mask
void BrachytherapyPatientFileHandler::getUrethraMask( 
					      std::vector<bool> &urethra_mask )
{
  d_hdf5_file.readArrayFromDataSet( urethra_mask,
				    "/organ_masks/urethra_mask" );
}

// Return the urethra mask volume
void BrachytherapyPatientFileHandler::getUrethraMaskVolume( 
					       unsigned &urethra_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( urethra_mask_volume,
					     "/organ_masks/urethra_mask",
					     "volume" );
}

// Return the margin mask
void BrachytherapyPatientFileHandler::getMarginMask( 
					       std::vector<bool> &margin_mask )
{
  d_hdf5_file.readArrayFromDataSet( margin_mask,
				    "/organ_masks/margin_mask" );
}

// Return the margin mask volume
void BrachytherapyPatientFileHandler::getMarginMaskVolume( 
					       unsigned &margin_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( margin_mask_volume,
					     "/organ_masks/margin_mask",
					     "volume" );
}

// Return the rectum mask
void BrachytherapyPatientFileHandler::getRectumMask( 
					       std::vector<bool> &rectum_mask )
{
  d_hdf5_file.readArrayFromDataSet( rectum_mask,
				    "/organ_masks/rectum_mask" );
}

// Return the rectum mask volume
void BrachytherapyPatientFileHandler::getRectumMaskVolume( 
					       unsigned &rectum_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( rectum_mask_volume,
					     "/organ_masks/rectum_mask",
					     "volume" );
}

// Return the needle template
void BrachytherapyPatientFileHandler::getNeedleTemplate( 
		  std::vector<std::pair<unsigned,unsigned> > &needle_template )
{
  d_hdf5_file.readArrayFromDataSet( needle_template,
				    "/needle_template" );
}

// Return the seed mesh dimensions
void BrachytherapyPatientFileHandler::getSeedMeshDimensions( 
				       std::vector<unsigned> &mesh_dimensions )
{
  d_hdf5_file.readArrayFromGroupAttribute( mesh_dimensions,
					   "/seed_data",
					   "mesh_dimensions" );
}
  
// Return the seed data for the desired seed
void BrachytherapyPatientFileHandler::getSeedData( 
					     std::vector<double> &seed_data,
					     const SeedType desired_seed_type )
{
  std::string dataset_location;
  getPathToSeedData( dataset_location, desired_seed_type );
 
  d_hdf5_file.readArrayFromDataSet( seed_data, dataset_location );
}

// Return the adjoint mesh dimensions
void BrachytherapyPatientFileHandler::getAdjointMeshDimensions( 
				       std::vector<unsigned> &mesh_dimensions )
{
  d_hdf5_file.readArrayFromGroupAttribute( mesh_dimensions,
					   "/adjoint_data",
					   "mesh_dimensions" );
}

// Return the prostate adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getProstateAdjointData( 
				    std::vector<double> &prostate_adjoint_data,
				    const SeedType desired_seed_type )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/prostate_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( prostate_adjoint_data, dataset_location );
}

// Return the urethra adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getUrethraAdjointData( 
				     std::vector<double> &urethra_adjoint_data,
				     const SeedType desired_seed_type )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/urethra_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( urethra_adjoint_data, dataset_location );
}

// Return the margin adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getMarginAdjointData( 
				      std::vector<double> &margin_adjoint_data,
				      const SeedType desired_seed_type )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/margin_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( margin_adjoint_data, dataset_location );
}

// Return the rectum adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getRectumAdjointData( 
				      std::vector<double> &rectum_adjoint_data,
				      const SeedType desired_seed_type )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/rectum_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( rectum_adjoint_data, dataset_location );
}

// Return the location of the seed data for the desired seed
void BrachytherapyPatientFileHandler::getPathToSeedData( 
					     std::string &seed_data_path,
					     const SeedType desired_seed_type )
{
  switch( desired_seed_type )
  {
  case AMERSHAM_6702_SEED:
    seed_data_path = "/seed_data/amersham_6702";
    break;
  case AMERSHAM_6711_SEED:
    seed_data_path = "/seed_data/amersham_6711";
    break;
  case BEST_2301_SEED:
    seed_data_path = "/seed_data/best_2301";
    break;
  case NASI_MED_3631_SEED:
    seed_data_path = "/seed_data/nasi_med_3631";
    break;
  case NASI_MED_3633_SEED:
    seed_data_path = "/seed_data/nasi_med_3633";
  case BEBIG_I25_S06_SEED:
    seed_data_path = "/seed_data/bebig_i25_s06";
    break;
  case IMAGYN_IS_12501_SEED:
    seed_data_path = "/seed_data/imagyn_is_12051";
    break;
  case THERAGENICS_200_SEED:
    seed_data_path = "/seed_data/theragenics_200";
    break;
  }
}

// Return the location of the adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getPathToAdjointData( 
					     std::string &seed_data_path,
					     const SeedType desired_seed_type )
{
  switch( desired_seed_type )
  {
  case AMERSHAM_6702_SEED:
    seed_data_path = "/adjoint_data/amersham_6702";
    break;
  case AMERSHAM_6711_SEED:
    seed_data_path = "/adjoint_data/amersham_6711";
    break;
  case BEST_2301_SEED:
    seed_data_path = "/adjoint_data/best_2301";
    break;
  case NASI_MED_3631_SEED:
    seed_data_path = "/adjoint_data/nasi_med_3631";
    break;
  case NASI_MED_3633_SEED:
    seed_data_path = "/adjoint_data/nasi_med_3633";
  case BEBIG_I25_S06_SEED:
    seed_data_path = "/adjoint_data/bebig_i25_s06";
    break;
  case IMAGYN_IS_12501_SEED:
    seed_data_path = "/adjoint_data/imagyn_is_12051";
    break;
  case THERAGENICS_200_SEED:
    seed_data_path = "/adjoint_data/theragenics_200";
    break;
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyPatientFileHandler.cpp
//---------------------------------------------------------------------------//
