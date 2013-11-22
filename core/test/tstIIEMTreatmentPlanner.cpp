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
#include <boost/test/unit_test.hpp>

// TPOR Includes
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

    std::vector<unsigned> mesh_dimensions( 3 );
    mesh_dimensions[0] = 2u;
    mesh_dimensions[1] = 3u;
    mesh_dimensions[2] = 4u;
    
    std::vector<signed char> needle_template( 2*3 );
    needle_template[0] = 1;
    
    std::vector<signed char> prostate_mask( 2*3*4 );
    prostate_mask[0] = 1;
    
    std::vector<signed char> urethra_mask( 2*3*4 );
    urethra_mask[0] = 1;
    
    std::vector<signed char> margin_mask( 2*3*4 );
    margin_mask[0] = 1;
    
    std::vector<signed char> rectum_mask( 2*3*4 );
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
// Check that the IIEMTreatmentPlanner can be successfully created
BOOST_AUTO_TEST_CASE( constructor )
{
  TPOR::BrachytherapySeedFactory::BrachytherapySeedPtr
    seed = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::AMERSHAM_6711_SEED,
						      1.5 );
						      
  TPOR::IIEMTreatmentPlanner treatement_planner( "John_Doe.h5",
						 seed,
						 145.0 );
}

//---------------------------------------------------------------------------//
// end tstIIEMTreatmentPlanner.cpp
//---------------------------------------------------------------------------//
