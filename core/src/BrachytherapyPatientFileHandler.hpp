//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyPatientFileHandler.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy patient hdf5 file handler class declaration
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_PATIENT_FILE_HANDLER_HPP
#define BRACHYTHERAPY_PATIENT_FILE_HANDLER_HPP

// Std Lib Includes
#include <utilities>

// TPOR Includes
#include "HDF5FileHandler.hpp"
#include "SeedType.hpp"

namespace TPOR{

//! Brachytherapy patient hdf5 file handler
class BrachytherapyPatientFileHandler
{
  //! Constructor
  BrachytherapyPatientFileHandler( const std::string &patient_file_name );
  
  //! Destructor
  ~BrachytherapyPatientFileHandler();

  //! Return the patient name
  void getPatientName( std::string &patient_name );

  //! Return the mesh element dimensions
  void getMeshElementDimensions( std::vector<double> &mesh_element_dimensions);

  //! Return the organ mask dimensions
  void getOrganMeshDimensions( std::vector<unsigned> &mesh_dimensions );

  //! Return the prostate mask
  void getProstateMask( std::vector<bool> &prostate_mask );

  //! Return the prostate mask volume
  void getProstateMaskVolume( unsigned &prostate_mask_volume );

  //! Return the urethra mask
  void getUrethraMask( std::vector<bool> &urethra_mask );

  //! Return the urethra mask volume
  void getUrethraMaskVolume( unsigned &urethra_mask_volume );

  //! Return the margin mask
  void getMarginMask( std::vector<bool> &margin_mask );

  //! Return the margin mask volume
  void getMarginMaskVolume( unsigned &margin_mask_volume );

  //! Return the rectum mask
  void getRectumMask( std::vector<bool> &rectum_mask );

  //! Return the rectum mask volume
  void getRectumMaskVolume( unsigned &rectum_mask_volume );

  //! Return the needle template
  void getNeedleTemplate( 
		 std::vector<std::pair<unsigned,unsigned> > &needle_template );

  //! Return the seed mesh dimensions
  void getSeedMeshDimensions( std::vector<unsigned> &mesh_dimensions );
  
  //! Return the seed data for the desired seed
  void getSeedData( std::vector<double> &seed_data,
		    const SeedType desired_seed_type );

  //! Return the adjoint mesh dimensions
  void getAdjointMeshDimensions( std::vector<unsigned> &mesh_dimensions );

  //! Return the prostate adjoint data for the desired seed
  void getProstateAdjointData( std::vector<double> &prostate_adjoint_data,
			       const SeedType desired_seed_type );

  //! Return the urethra adjoint data for the desired seed
  void getUrethraAdjointData( std::vector<double> &urethra_adjoint_data,
			      const SeedType desired_seed_type );

  //! Return the margin adjoint data for the desired seed
  void getMarginAdjointData( std::vector<double> &margin_adjoint_data,
			     const SeedType desired_seed_type );

  //! Return the rectum adjoint data for the desired seed
  void getRectumAdjointData( std::vector<double> &rectum_adjoint_data,
			     const SeedType desired_seed_type );

private:

  //! Return the location of the seed data for the desired seed
  void getPathToSeedData( std::string &seed_data_path,
			  const SeedType desired_seed_type );

  //! Return the location of the adjoint data for the desired seed
  void getPathToAdjointData( std::string &seed_data_path,
			     const SeedType desired_seed_type );
  
  // HDF5FileHandler
  HDF5FileHandler d_hdf5_file;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_PATIENT_FILE_HANDLER_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyPatientFileHandler.hpp
//---------------------------------------------------------------------------//
