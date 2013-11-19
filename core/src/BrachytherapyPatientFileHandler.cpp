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
  d_hdf5_file.openHDF5FileAndAppend( patient_file_name );
}

// Destructor
BrachytherapyPatientFileHandler::~BrachytherapyPatientFileHandler()
{
  d_hdf5_file.closeHDF5File();
}

// Return the patient name
void BrachytherapyPatientFileHandler::getPatientName( 
						    std::string &patient_name )
{
  d_hdf5_file.readArrayFromGroupAttribute( patient_name,
					   "/",
					   "patient_name" );
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
					   "/",
					   "mesh_dimensions" );
}

// Return the prostate mask
void BrachytherapyPatientFileHandler::getProstateMask( 
					     std::vector<bool> &prostate_mask )
{
  std::vector<signed char> tmp_prostate_mask;
  d_hdf5_file.readArrayFromDataSet( tmp_prostate_mask,
				    "/organ_masks/prostate_mask" );

  fillBooleanArray( prostate_mask, tmp_prostate_mask );
}

// Return the prostate mask relative volume
void BrachytherapyPatientFileHandler::getProstateMaskRelativeVolume( 
					 unsigned &prostate_mask_relative_vol )
{
  d_hdf5_file.readValueFromDataSetAttribute( prostate_mask_relative_vol,
					     "/organ_masks/prostate_mask",
					     "relative_volume" );
}

// Return the prostate mask volume (cm^3)
void BrachytherapyPatientFileHandler::getProstateMaskVolume( 
						 double &prostate_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( prostate_mask_volume,
					     "/organ_masks/prostate_mask",
					     "volume" );
}

// Return the urethra mask
void BrachytherapyPatientFileHandler::getUrethraMask( 
					      std::vector<bool> &urethra_mask )
{
  std::vector<signed char> tmp_urethra_mask;
  d_hdf5_file.readArrayFromDataSet( tmp_urethra_mask,
				    "/organ_masks/urethra_mask" );

  fillBooleanArray( urethra_mask, tmp_urethra_mask );
}

// Return the urethra mask relative volume
void BrachytherapyPatientFileHandler::getUrethraMaskRelativeVolume( 
					  unsigned &urethra_mask_relative_vol )
{
  d_hdf5_file.readValueFromDataSetAttribute( urethra_mask_relative_vol,
					     "/organ_masks/urethra_mask",
					     "relative_volume" );
}

// Return the urethra mask volume
void BrachytherapyPatientFileHandler::getUrethraMaskVolume(
						  double &urethra_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( urethra_mask_volume,
					     "/organ_masks/urethra_mask",
					     "volume" );
}

// Return the margin mask
void BrachytherapyPatientFileHandler::getMarginMask( 
					       std::vector<bool> &margin_mask )
{
  std::vector<signed char> tmp_margin_mask;
  d_hdf5_file.readArrayFromDataSet( tmp_margin_mask,
				    "/organ_masks/margin_mask" );

  fillBooleanArray( margin_mask, tmp_margin_mask );
}

// Return the margin mask relative volume
void BrachytherapyPatientFileHandler::getMarginMaskRelativeVolume( 
					   unsigned &margin_mask_relative_vol )
{
  d_hdf5_file.readValueFromDataSetAttribute( margin_mask_relative_vol,
					     "/organ_masks/margin_mask",
					     "relative_volume" );
}

// Return the margin mask volume
void BrachytherapyPatientFileHandler::getMarginMaskVolume(
						   double &margin_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( margin_mask_volume,
					     "/organ_masks/margin_mask",
					     "volume" );
}

// Return the rectum mask
void BrachytherapyPatientFileHandler::getRectumMask( 
					       std::vector<bool> &rectum_mask )
{
  std::vector<signed char> tmp_rectum_mask;
  d_hdf5_file.readArrayFromDataSet( tmp_rectum_mask,
				    "/organ_masks/rectum_mask" );
  
  fillBooleanArray( rectum_mask, tmp_rectum_mask );
}

// Return the rectum mask relative volume
void BrachytherapyPatientFileHandler::getRectumMaskRelativeVolume( 
					   unsigned &rectum_mask_relative_vol )
{
  d_hdf5_file.readValueFromDataSetAttribute( rectum_mask_relative_vol,
					     "/organ_masks/rectum_mask",
					     "relative_volume" );
}

// Return the rectum mask volume
void BrachytherapyPatientFileHandler::getRectumMaskVolume(
						   double &rectum_mask_volume )
{
  d_hdf5_file.readValueFromDataSetAttribute( rectum_mask_volume,
					     "/organ_masks/rectum_mask",
					     "volume" );
}

// Return the needle template
void BrachytherapyPatientFileHandler::getNeedleTemplate( 
					   std::vector<bool> &needle_template )
{
  std::vector<signed char> tmp_needle_template;
  d_hdf5_file.readArrayFromDataSet( tmp_needle_template,
				    "/needle_template" );

  fillBooleanArray( needle_template, tmp_needle_template );
}

// Test if adjoint data has been generated for a specific seed
bool BrachytherapyPatientFileHandler::adjointDataExists( 
					 const BrachytherapySeedType seed_type,
					 const double orientation_angle )
{
  std::string group_location;
  getPathToAdjointData( group_location, seed_type );
  
  return d_hdf5_file.groupExists( group_location );
}

// Return the prostate adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getProstateAdjointData( 
				 std::vector<double> &prostate_adjoint_data,
				 const BrachytherapySeedType desired_seed_type,
				 const double desired_seed_strength,
				 const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/prostate_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( prostate_adjoint_data, dataset_location );

  // Scale the adjoint data by the desired seed strength
  scaleAdjointData( prostate_adjoint_data, desired_seed_strength );
}

// Set the prostate adjoint data for the desired seed
void BrachytherapyPatientFileHandler::setProstateAdjointData(
			      const std::vector<double> &prostate_adjoint_data,
			      const BrachytherapySeedType seed_type,
			      const double seed_strength,
			      const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, seed_type );
  dataset_location += "/prostate_adjoint_data";
  
  // Normalize the adjoint data
  std::vector<double> normalized_adjoint_data = prostate_adjoint_data;
  scaleAdjointData( normalized_adjoint_data, 1.0/seed_strength );
  
  d_hdf5_file.writeArrayToDataSet( normalized_adjoint_data, dataset_location );
}

// Return the urethra adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getUrethraAdjointData( 
				 std::vector<double> &urethra_adjoint_data,
				 const BrachytherapySeedType desired_seed_type,
				 const double desired_seed_strength,
				 const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/urethra_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( urethra_adjoint_data, dataset_location );

  // Scale the adjoint data by the desired seed strength
  scaleAdjointData( urethra_adjoint_data, desired_seed_strength );
}

// Set the urethra adjoint data for the desired seed
void BrachytherapyPatientFileHandler::setUrethraAdjointData( 
			       const std::vector<double> &urethra_adjoint_data,
			       const BrachytherapySeedType seed_type,
			       const double seed_strength,
			       const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, seed_type );
  dataset_location += "/urethra_adjoint_data";

  
  // Normalize the adjoint data
  std::vector<double> normalized_adjoint_data = urethra_adjoint_data;
  scaleAdjointData( normalized_adjoint_data, 1.0/seed_strength );

  d_hdf5_file.writeArrayToDataSet( normalized_adjoint_data, dataset_location );
}

// Return the margin adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getMarginAdjointData( 
				 std::vector<double> &margin_adjoint_data,
				 const BrachytherapySeedType desired_seed_type,
				 const double desired_seed_strength,
				 const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/margin_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( margin_adjoint_data, dataset_location );

  // Scale the adjoint data by the desired seed strength
  scaleAdjointData( margin_adjoint_data, desired_seed_strength );
}

// Set the margin adjoint data for the desired seed
void BrachytherapyPatientFileHandler::setMarginAdjointData( 
			        const std::vector<double> &margin_adjoint_data,
				const BrachytherapySeedType seed_type,
				const double seed_strength,
				const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, seed_type );
  dataset_location += "/margin_adjoint_data";

  
  // Normalize the adjoint data
  std::vector<double> normalized_adjoint_data = margin_adjoint_data;
  scaleAdjointData( normalized_adjoint_data, 1.0/seed_strength );

  d_hdf5_file.writeArrayToDataSet( normalized_adjoint_data, dataset_location );
}

// Return the rectum adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getRectumAdjointData( 
				 std::vector<double> &rectum_adjoint_data,
				 const BrachytherapySeedType desired_seed_type,
				 const double desired_seed_strength,
				 const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, desired_seed_type );
  dataset_location += "/rectum_adjoint_data";

  d_hdf5_file.readArrayFromDataSet( rectum_adjoint_data, dataset_location );

  // Scale the adjoint data by the desired seed strength
  scaleAdjointData( rectum_adjoint_data, desired_seed_strength );
}

//! Set the rectum adjoint data for the desired seed
void BrachytherapyPatientFileHandler::setRectumAdjointData( 
			        const std::vector<double> &rectum_adjoint_data,
				const BrachytherapySeedType seed_type,
				const double seed_strength,
				const double orientation_angle )
{
  std::string dataset_location;
  getPathToAdjointData( dataset_location, seed_type );
  dataset_location += "/rectum_adjoint_data";

  
  // Normalize the adjoint data
  std::vector<double> normalized_adjoint_data = rectum_adjoint_data;
  scaleAdjointData( normalized_adjoint_data, 1.0/seed_strength );

  d_hdf5_file.writeArrayToDataSet( normalized_adjoint_data, dataset_location );
}

// Return the location of the adjoint data for the desired seed
void BrachytherapyPatientFileHandler::getPathToAdjointData( 
				std::string &seed_data_path,
				const BrachytherapySeedType desired_seed_type )
{
  seed_data_path = "/adjoint_data/" +
    brachytherapySeedTypeToString( desired_seed_type );
}

// Fill a boolean array using an array of signed chars
void BrachytherapyPatientFileHandler::fillBooleanArray( 
			        std::vector<bool> &bool_array,
			        const std::vector<signed char> &schar_array )
{
  bool_array.resize( schar_array.size() );
  
  for( unsigned i = 0; i < schar_array.size(); ++i )
  {
    if( schar_array[i] == 0 )
      bool_array[i] = false;
    else 
      bool_array[i] = true;
  }
}

// Scale adjoint data by a factor
void BrachytherapyPatientFileHandler::scaleAdjointData( 
					     std::vector<double> &adjoint_data,
					     const double scale_factor )
{
  std::vector<double>::iterator start = adjoint_data.begin(),
    end = adjoint_data.end();

  while( start != end )
  {
    *start *= scale_factor;
      
    ++start;
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyPatientFileHandler.cpp
//---------------------------------------------------------------------------//
