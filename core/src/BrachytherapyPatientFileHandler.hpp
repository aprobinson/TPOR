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
#include <utility>

// TPOR Includes
#include "HDF5FileHandler.hpp"
#include "BrachytherapySeedType.hpp"

namespace TPOR{

//! Brachytherapy patient hdf5 file handler
class BrachytherapyPatientFileHandler
{
  
public:
  
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

  //! Return the prostate mask relative volume (num elements)
  void getProstateMaskRelativeVolume( unsigned &prostate_mask_relative_vol );

  //! Return the prostate mask volume (cm^3)
  void getProstateMaskVolume( double &prostate_mask_volume );

  //! Return the urethra mask
  void getUrethraMask( std::vector<bool> &urethra_mask );

  //! Return the urethra mask relative volume (num elements)
  void getUrethraMaskRelativeVolume( unsigned &urethra_mask_relative_vol );

  //! Return the urethra mask volume (cm^3)
  void getUrethraMaskVolume( double &urethra_mask_volume );

  //! Return the margin mask
  void getMarginMask( std::vector<bool> &margin_mask );

  //! Return the margin mask relative volume (num elements)
  void getMarginMaskRelativeVolume( unsigned &margin_mask_relative_vol );

  //! Return the margin mask volume (cm^3)
  void getMarginMaskVolume( double &margin_mask_volume );

  //! Return the rectum mask
  void getRectumMask( std::vector<bool> &rectum_mask );

  //! Return the rectum mask relative volume (num elements)
  void getRectumMaskRelativeVolume( unsigned &rectum_mask_volume );

  //! Return the rectum mask volume (cm^3)
  void getRectumMaskVolume( double &rectum_volume );

  //! Return the needle template
  void getNeedleTemplate( std::vector<bool> &needle_template );

  //! Test if adjoint data has been generated for a specific seed
  bool adjointDataExists( const std::string &seed_name );

  //! Return the prostate adjoint data for the desired seed
  void getProstateAdjointData( std::vector<double> &prostate_adjoint_data,
			       const std::string &desired_seed_name,
			       const double desired_seed_strength );
			       
  //! Set the prostate adjoint data for the desired seed
  void setProstateAdjointData(const std::vector<double> &prostate_adjoint_data,
			      const std::string &seed_name,
			      const double seed_strength );

  //! Return the urethra adjoint data for the desired seed
  void getUrethraAdjointData( std::vector<double> &urethra_adjoint_data,
			      const std::string &desired_seed_name,
			      const double desired_seed_strength );

  //! Set the urethra adjoint data for the desired seed
  void setUrethraAdjointData( const std::vector<double> &urethra_adjoint_data,
			      const std::string &seed_name,
			      const double seed_strength );

  //! Return the margin adjoint data for the desired seed
  void getMarginAdjointData( std::vector<double> &margin_adjoint_data,
			     const std::string &desired_seed_name,
			     const double desired_seed_strength );

  //! Set the margin adjoint data for the desired seed
  void setMarginAdjointData( const std::vector<double> &margin_adjoint_data,
			     const std::string &seed_name,
			     const double seed_strength );

  //! Return the rectum adjoint data for the desired seed
  void getRectumAdjointData( std::vector<double> &rectum_adjoint_data,
			     const std::string &desired_seed_name,
			     const double desired_seed_strength );

  //! Set the rectum adjoint data for the desired seed
  void setRectumAdjointData( const std::vector<double> &rectum_adjoint_data,
			     const std::string &seed_name,
			     const double seed_strength );

private:

  //! Fill a boolean array using an array of chars
  void fillBooleanArray( std::vector<bool> &bool_array,
			 const std::vector<char> &schar_array );

  //! Scale adjoint data by a factor
  void scaleAdjointData( std::vector<double> &adjoint_data,
			 const double scale_factor );
  
  // HDF5FileHandler
  HDF5FileHandler d_hdf5_file;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_PATIENT_FILE_HANDLER_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyPatientFileHandler.hpp
//---------------------------------------------------------------------------//
