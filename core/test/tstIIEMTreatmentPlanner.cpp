//---------------------------------------------------------------------------//
//!
//! \file   tstIIEMTreatmentPlanner.cpp
//! \author Alex Robinson
//! \brief  Unit tests for the IIEMTreatmentPlanner class
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#define BOOST_TEST_MODULE IIEM Treatment Planner
#include <boost/test/included/unit_test.hpp>

// SPRNG Includes
#include "HDF5FileHandler.hpp"
#include "IIEMTreatmentPlanner.hpp"

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

    std::vector<double> mesh_dimensions( 3 );
    mesh_dimensions[0] = 5;
    mesh_dimensions[1] = 5;
    mesh_dimensions[2] = 5;

    std::vector<char> needle_template( 5*5 );
    fill_needle_template( needle_template );
    
    std::vector<char> prostate_mask( 5*5*5 );
    fill_prostate_mask( prostate_mask );
    
    std::vector<char> urethra_mask( 5*5*5 );
    
    std::vector<char> margin_mask( 5*5*5 );
        
    std::vector<char> rectum_mask( 5*5*5 );
    
    TPOR::HDF5FileHandler file_handle;
    file_handle.openHDF5FileAndOverwrite( "John_Dose.h5" );
    
    file_handle.writeArrayToGroupAttribute( patient_name,
					    "/",
					    "patient_name" );
    file_handle.writeArrayToGroupAttribute( mesh_element_dimensions,
					    "/",
					    "mesh_element_dimensions" );
    file_handle.writeArrayToGroupAttribute( mesh_dimensions,
					    "/",
					    "mesh_element_dimensions" );
    file_handle.writeArrayToDataSet( needle_template, "/needle_template" );
    file_handle.writeArrayToDataSet( prostate_mask, 
				     "/organ_masks/prostate_mask" );
    file_handle.writeArrayToDataSet( urethra_mask,
				     "/organ_masks/urethra_mask" );
    file_handle.writeArrayToDataSet( margin_mask,
				     "/organ_masks/margin_mask" );
    file_handle.writeArrayToDataSet( rectum_mask,
				     "/organ_masks/rectum_mask" );

    file_handle.writeValueToDataSetAttribute( 5,
					      "/organ_masks/prostate_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 5,
					      "/organ_masks/urethra_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 5,
					      "/organ_masks/margin_mask",
					      "relative_volume" );
    file_handle.writeValueToDataSetAttribute( 5,
					      "/organ_masks/margin_mask",
					      "relative_volume" );
    file_handle.closeHDF5File();
  }

  void fill_needle_template( std::vector<char> &needle_template )
  {
    for( int j = 0; j < 5; j += 2 )
      for( int i = 0; i < 5; i += 2 )
	needle_template[i +j*5] = 1;
  }
  
  void fill_prostate_mask( std::vector<char> &prostate_template )
  {
    for( int k = 1; k < 4; ++k )
      for( int j = 1; j < 4; ++j )
	for( int i = 1; i < 4; ++i )
	  prostate_template[i+j*5+k*5*5] = 1;
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
// Check that the IIEMTreatmentPlanner can be successfully created
BOOST_AUTO_TEST_CASE( constructor )
{
  TPOR::IIEMTreatmentPlanner treatement_planner( "John_Doe.h5",
						 TPOR::AMERSHAM_6711_SEED,
						 1.5,
						 145.0 );
}

//---------------------------------------------------------------------------//
// end tstIIEMTreatmentPlanner.cpp
//---------------------------------------------------------------------------//
