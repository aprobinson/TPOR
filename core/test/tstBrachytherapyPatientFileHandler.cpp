//---------------------------------------------------------------------------//
//!
//! \file   tstBrachytherapyPatientFileHandler.cpp
//! \author Alex Robinson
//! \brief  BrachytherapyPatientFileHandler class unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <algorithm>

// Boost Includes
#define BOOST_TEST_MODULE BrachytherapyPatientFileHandler
#include <boost/test/unit_test.hpp>

// TPOR Includes
#include "BrachytherapyPatientFileHandler.hpp"

//---------------------------------------------------------------------------//
// Testing Structs.
//---------------------------------------------------------------------------//
struct MockPatientFileGenerator{
  MockPatientFileGenerator()
  {
    std::string patient_name( "John Doe" );

    std::vector<double> mesh_element_dimensions( 3 );
    mesh_element_dimensions[0] = 0.1;
    mesh_element_dimensions[1] = 0.1;
    mesh_element_dimensions[2] = 0.5;

    std::vector<unsigned> mesh_dimensions( 3 );
    mesh_dimensions[0] = 2u;
    mesh_dimensions[1] = 3u;
    mesh_dimensions[2] = 4u;
    
    std::vector<char> needle_template( 2*3 );
    needle_template[0] = 1;
    
    std::vector<char> prostate_mask( 2*3*4 );
    prostate_mask[0] = 1;
    
    std::vector<char> urethra_mask( 2*3*4 );
    urethra_mask[0] = 1;
    
    std::vector<char> margin_mask( 2*3*4 );
    margin_mask[0] = 1;
    
    std::vector<char> rectum_mask( 2*3*4 );
    rectum_mask[0] = 1;

    std::vector<double> prostate_adjoint( 2*3*4 );
    std::fill( prostate_adjoint.begin(), prostate_adjoint.end(), 1.0 );

    std::vector<double> urethra_adjoint( 2*3*4 );
    std::fill( urethra_adjoint.begin(), urethra_adjoint.end(), 1.0 );

    std::vector<double> margin_adjoint( 2*3*4 );
    std::fill( margin_adjoint.begin(), margin_adjoint.end(), 1.0 );

    std::vector<double> rectum_adjoint( 2*3*4 );
    std::fill( rectum_adjoint.begin(), rectum_adjoint.end(), 1.0 );
    
    TPOR::HDF5FileHandler file_handle;
    file_handle.openHDF5FileAndOverwrite( "John_Doe.h5" );
    
    file_handle.writeArrayToGroupAttribute( patient_name,
					    "/",
					    "patient_name" );
    file_handle.writeArrayToGroupAttribute( mesh_element_dimensions,
					    "/",
					    "mesh_element_dimensions" );
    file_handle.writeArrayToGroupAttribute( mesh_dimensions,
					    "/",
					    "mesh_dimensions" );
    file_handle.writeArrayToDataSet( needle_template, "/needle_template" );
    file_handle.writeArrayToDataSet( prostate_mask, 
				     "/organ_masks/prostate_mask" );
    file_handle.writeArrayToDataSet( urethra_mask,
				     "/organ_masks/urethra_mask" );
    file_handle.writeArrayToDataSet( margin_mask,
				     "/organ_masks/margin_mask" );
    file_handle.writeArrayToDataSet( rectum_mask,
				     "/organ_masks/rectum_mask" );

    file_handle.writeValueToDataSetAttribute( 1u,
					      "/organ_masks/prostate_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 1.0,
					      "/organ_masks/prostate_mask",
					      "volume" );
    file_handle.writeValueToDataSetAttribute( 1u,
					      "/organ_masks/urethra_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 1.0,
					      "/organ_masks/urethra_mask",
					      "volume" );
    file_handle.writeValueToDataSetAttribute( 1u,
					      "/organ_masks/margin_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 1.0,
					      "/organ_masks/margin_mask",
					      "volume" );
    file_handle.writeValueToDataSetAttribute( 1u,
					      "/organ_masks/rectum_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 1.0,
					      "/organ_masks/rectum_mask",
					      "volume" );
    
    std::string adjoint_data_path = "/adjoint_data/Amersham6711Seed";
    
    file_handle.writeArrayToDataSet( prostate_adjoint, adjoint_data_path +
				    "/prostate_adjoint_data" );
    file_handle.writeArrayToDataSet( urethra_adjoint, adjoint_data_path +
				    "/urethra_adjoint_data" );
    file_handle.writeArrayToDataSet( margin_adjoint, adjoint_data_path +
				    "/margin_adjoint_data" );
    file_handle.writeArrayToDataSet( rectum_adjoint, adjoint_data_path +
				     "/rectum_adjoint_data" );
    file_handle.closeHDF5File();
  }
  
  ~MockPatientFileGenerator()
  { /* ... */ }
};

//---------------------------------------------------------------------------//
// Global Testing Fixture.
//---------------------------------------------------------------------------//
BOOST_GLOBAL_FIXTURE( MockPatientFileGenerator );

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the patient name can be retrieved.
BOOST_AUTO_TEST_CASE( getPatientName )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::string patient_name;

  patient_file.getPatientName( patient_name );
  
  BOOST_CHECK_EQUAL( patient_name, "John Doe" );
}

//---------------------------------------------------------------------------//
// Check that the mesh element dimensions can be retrieved.
BOOST_AUTO_TEST_CASE( getMeshElementDimensions )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> mesh_element_dimensions;

  patient_file.getMeshElementDimensions( mesh_element_dimensions );

  BOOST_CHECK_EQUAL( mesh_element_dimensions[0], 0.1 );
  BOOST_CHECK_EQUAL( mesh_element_dimensions[1], 0.1 );
  BOOST_CHECK_EQUAL( mesh_element_dimensions[2], 0.5 );
}

//---------------------------------------------------------------------------//
// Check that the mesh dimensions can be retrieved.
BOOST_AUTO_TEST_CASE( getOrganMeshDimensions )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<unsigned> mesh_dimensions;

  patient_file.getOrganMeshDimensions( mesh_dimensions );

  BOOST_CHECK_EQUAL( mesh_dimensions[0], 2 );
  BOOST_CHECK_EQUAL( mesh_dimensions[1], 3 );
  BOOST_CHECK_EQUAL( mesh_dimensions[2], 4 );
}

//---------------------------------------------------------------------------//
// Check that the prostate mask can be retrieved.
BOOST_AUTO_TEST_CASE( getProstateMask )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<bool> prostate_mask;

  patient_file.getProstateMask( prostate_mask );

  BOOST_CHECK_EQUAL( prostate_mask[0], true );
  BOOST_CHECK_EQUAL( prostate_mask[1], false );
  BOOST_CHECK_EQUAL( prostate_mask[2], false );
}

//---------------------------------------------------------------------------//
// Check that the prostate mask relative volume can be retrieved.
BOOST_AUTO_TEST_CASE( getProstateMaskRelativeVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  unsigned prostate_mask_relative_vol;

  patient_file.getProstateMaskRelativeVolume( prostate_mask_relative_vol );

  BOOST_CHECK_EQUAL( prostate_mask_relative_vol, 1 );
}

//---------------------------------------------------------------------------//
// Check that the prostate mask volume can be retrieved.
BOOST_AUTO_TEST_CASE( getProstateMaskVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  double prostate_mask_volume;

  patient_file.getProstateMaskVolume( prostate_mask_volume );

  BOOST_CHECK_EQUAL( prostate_mask_volume, 1.0 );
}

//---------------------------------------------------------------------------//
// Check that the urethra mask can be retrieved.
BOOST_AUTO_TEST_CASE( getUrethraMask )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<bool> urethra_mask;

  patient_file.getUrethraMask( urethra_mask );

  BOOST_CHECK_EQUAL( urethra_mask[0], true );
  BOOST_CHECK_EQUAL( urethra_mask[1], false );
  BOOST_CHECK_EQUAL( urethra_mask[2], false );
}

//---------------------------------------------------------------------------//
// Check that the urethra mask relative volume can be retrieved.
BOOST_AUTO_TEST_CASE( getUrethraMaskRelativeVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  unsigned urethra_mask_relative_vol;

  patient_file.getUrethraMaskRelativeVolume( urethra_mask_relative_vol );

  BOOST_CHECK_EQUAL( urethra_mask_relative_vol, 1 );
}

//---------------------------------------------------------------------------//
// Check that the urethra mask volume can be retrieved.
BOOST_AUTO_TEST_CASE( getUrethraMaskVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  double urethra_mask_volume;

  patient_file.getUrethraMaskVolume( urethra_mask_volume );

  BOOST_CHECK_EQUAL( urethra_mask_volume, 1.0 );
}

//---------------------------------------------------------------------------//
// Check that the margin mask can be retrieved.
BOOST_AUTO_TEST_CASE( getMarginMask )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<bool> margin_mask;

  patient_file.getMarginMask( margin_mask );

  BOOST_CHECK_EQUAL( margin_mask[0], true );
  BOOST_CHECK_EQUAL( margin_mask[1], false );
  BOOST_CHECK_EQUAL( margin_mask[2], false );
}

//---------------------------------------------------------------------------//
// Check that the margin mask relative volume can be retrieved.
BOOST_AUTO_TEST_CASE( getMarginMaskRelativeVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  unsigned margin_mask_relative_vol;

  patient_file.getMarginMaskRelativeVolume( margin_mask_relative_vol );

  BOOST_CHECK_EQUAL( margin_mask_relative_vol, 1 );
}

//---------------------------------------------------------------------------//
// Check that the margin mask volume can be retrieved.
BOOST_AUTO_TEST_CASE( getMarginMaskVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  double margin_mask_volume;
  
  patient_file.getMarginMaskVolume( margin_mask_volume );

  BOOST_CHECK_EQUAL( margin_mask_volume, 1.0 );
}

//---------------------------------------------------------------------------//
// Check that the rectum mask can be retrieved.
BOOST_AUTO_TEST_CASE( getRectumMask )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<bool> rectum_mask;

  patient_file.getRectumMask( rectum_mask );

  BOOST_CHECK_EQUAL( rectum_mask[0], true );
  BOOST_CHECK_EQUAL( rectum_mask[1], false );
  BOOST_CHECK_EQUAL( rectum_mask[2], false );
}

//---------------------------------------------------------------------------//
// Check that the rectum mask relative volume can be retrieved.
BOOST_AUTO_TEST_CASE( getRectumMaskRelativeVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  unsigned rectum_mask_relative_vol;

  patient_file.getRectumMaskRelativeVolume( rectum_mask_relative_vol );

  BOOST_CHECK_EQUAL( rectum_mask_relative_vol, 1 );
}

//---------------------------------------------------------------------------//
// Check that the rectum mask volume can be retrieved.
BOOST_AUTO_TEST_CASE( getRectumMaskVolume )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  double rectum_mask_volume;

  patient_file.getRectumMaskVolume( rectum_mask_volume );

  BOOST_CHECK_EQUAL( rectum_mask_volume, 1.0 );
}

//---------------------------------------------------------------------------//
// Check that the needle template can be retrieved.
BOOST_AUTO_TEST_CASE( getNeedleTemplate )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<bool> needle_template;

  patient_file.getNeedleTemplate( needle_template );

  BOOST_CHECK_EQUAL( needle_template[0], true );
  BOOST_CHECK_EQUAL( needle_template[1], false );
  BOOST_CHECK_EQUAL( needle_template[2], false );
}

//---------------------------------------------------------------------------//
// Check if the adjoint data for a seed type exists.
BOOST_AUTO_TEST_CASE( adjointDataExists )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  
  BOOST_CHECK( patient_file.adjointDataExists( "Amersham6711Seed" ) );
  BOOST_CHECK( !patient_file.adjointDataExists( "Amersham6702Seed" ) );
}

//---------------------------------------------------------------------------//
// Check that the prostate adjoint data can be retrieved.
BOOST_AUTO_TEST_CASE( getProstateAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> prostate_adjoint_data;

  patient_file.getProstateAdjointData( prostate_adjoint_data,
				       "Amersham6711Seed",
				       1.5 );

  BOOST_CHECK_EQUAL( prostate_adjoint_data[0], 1.5 );
  BOOST_CHECK_EQUAL( prostate_adjoint_data[1], 1.5 );
  BOOST_CHECK_EQUAL( prostate_adjoint_data[2], 1.5 );
}

//---------------------------------------------------------------------------//
// Check that prostate adjoint data can be cached.
BOOST_AUTO_TEST_CASE( setProstateAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> prostate_adjoint_data( 2*3*4 );
  std::fill( prostate_adjoint_data.begin(),
	     prostate_adjoint_data.end(),
	     0.5 );

  patient_file.setProstateAdjointData( prostate_adjoint_data,
  				       "Amersham6702Seed",
  				       0.5 );
  
  std::vector<double> stored_prostate_adjoint_data;
  patient_file.getProstateAdjointData( stored_prostate_adjoint_data,
				       "Amersham6702Seed",
				       1.5 );

  BOOST_CHECK_EQUAL( prostate_adjoint_data[0]*3, 
		     stored_prostate_adjoint_data[0] );
  BOOST_CHECK_EQUAL( prostate_adjoint_data[1]*3, 
		     stored_prostate_adjoint_data[1] );
  BOOST_CHECK_EQUAL( prostate_adjoint_data[2]*3, 
		     stored_prostate_adjoint_data[2] );
}

//---------------------------------------------------------------------------//
// Check that the urethra adjoint data can be retrieved.
BOOST_AUTO_TEST_CASE( getUrethraAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> urethra_adjoint_data;

  patient_file.getUrethraAdjointData( urethra_adjoint_data,
				      "Amersham6711Seed",
				      1.5 );

  BOOST_CHECK_EQUAL( urethra_adjoint_data[0], 1.5 );
  BOOST_CHECK_EQUAL( urethra_adjoint_data[1], 1.5 );
  BOOST_CHECK_EQUAL( urethra_adjoint_data[2], 1.5 );
}

//---------------------------------------------------------------------------//
// Check that the urethra adjoint data can be cached.
BOOST_AUTO_TEST_CASE( setUrethraAdjointData )
{
TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> urethra_adjoint_data( 2*3*4 );
  std::fill( urethra_adjoint_data.begin(),
	     urethra_adjoint_data.end(),
	     0.5 );

  patient_file.setUrethraAdjointData( urethra_adjoint_data,
				      "Amersham6702Seed",
				      0.5 );
  
  std::vector<double> stored_urethra_adjoint_data;
  patient_file.getUrethraAdjointData( stored_urethra_adjoint_data,
				      "Amersham6702Seed",
				      1.5 );

  BOOST_CHECK_EQUAL( urethra_adjoint_data[0]*3, 
		     stored_urethra_adjoint_data[0] );
  BOOST_CHECK_EQUAL( urethra_adjoint_data[1]*3, 
		     stored_urethra_adjoint_data[1] );
  BOOST_CHECK_EQUAL( urethra_adjoint_data[2]*3, 
		     stored_urethra_adjoint_data[2] );
}

//---------------------------------------------------------------------------//
// Check that the margin adjoint data can be retrieved.
BOOST_AUTO_TEST_CASE( getMarginAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> margin_adjoint_data;

  patient_file.getMarginAdjointData( margin_adjoint_data,
				     "Amersham6711Seed",
				     1.5 );

  BOOST_CHECK_EQUAL( margin_adjoint_data[0], 1.5 );
  BOOST_CHECK_EQUAL( margin_adjoint_data[1], 1.5 );
  BOOST_CHECK_EQUAL( margin_adjoint_data[2], 1.5 );
}

//---------------------------------------------------------------------------//
// Check that the margin adjoint data can be retrieved.
BOOST_AUTO_TEST_CASE( setMarginAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> margin_adjoint_data( 2*3*4 );
  std::fill( margin_adjoint_data.begin(),
	     margin_adjoint_data.end(),
	     0.5 );

  patient_file.setMarginAdjointData( margin_adjoint_data,
				     "Amersham6702Seed",
				     0.5 );
  
  std::vector<double> stored_margin_adjoint_data;
  patient_file.getUrethraAdjointData( stored_margin_adjoint_data,
				      "Amersham6702Seed",
				      1.5 );

  BOOST_CHECK_EQUAL( margin_adjoint_data[0]*3, 
		     stored_margin_adjoint_data[0] );
  BOOST_CHECK_EQUAL( margin_adjoint_data[1]*3, 
		     stored_margin_adjoint_data[1] );
  BOOST_CHECK_EQUAL( margin_adjoint_data[2]*3, 
		     stored_margin_adjoint_data[2] );
}

//---------------------------------------------------------------------------//
// Check that the rectum adjoint data can be retrieved.
BOOST_AUTO_TEST_CASE( getRectumAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> rectum_adjoint_data;

  patient_file.getRectumAdjointData( rectum_adjoint_data,
				     "Amersham6711Seed",
				     1.5 );

  BOOST_CHECK_EQUAL( rectum_adjoint_data[0], 1.5 );
  BOOST_CHECK_EQUAL( rectum_adjoint_data[1], 1.5 );
  BOOST_CHECK_EQUAL( rectum_adjoint_data[2], 1.5 );
}

//---------------------------------------------------------------------------//
// Check that the rectum adjoint data can be retrieved.
BOOST_AUTO_TEST_CASE( setRectumAdjointData )
{
  TPOR::BrachytherapyPatientFileHandler patient_file( "John_Doe.h5" );
  std::vector<double> rectum_adjoint_data( 2*3*4 );
  std::fill( rectum_adjoint_data.begin(),
	     rectum_adjoint_data.end(),
	     0.5 );

  patient_file.setRectumAdjointData( rectum_adjoint_data,
				     "Amersham6702Seed",
				     0.5 );
  
  std::vector<double> stored_rectum_adjoint_data;
  patient_file.getUrethraAdjointData( stored_rectum_adjoint_data,
				      "Amersham6702Seed",
				      1.5 );

  BOOST_CHECK_EQUAL( rectum_adjoint_data[0]*3, 
		     stored_rectum_adjoint_data[0] );
  BOOST_CHECK_EQUAL( rectum_adjoint_data[1]*3, 
		     stored_rectum_adjoint_data[1] );
  BOOST_CHECK_EQUAL( rectum_adjoint_data[2]*3, 
		     stored_rectum_adjoint_data[2] );
}

//---------------------------------------------------------------------------//
// end tstBrachytherapyPatientFileHandler.cpp
//---------------------------------------------------------------------------//
