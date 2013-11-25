//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedFileHandler.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed hdf5 file handler class declaration
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_FILE_HANDLER_HPP
#define BRACHYTHERAPY_SEED_FILE_HANDLER_HPP

// TPOR Includes
#include "HDF5FileHandler.hpp"
#include "BrachytherapySeedType.hpp"

namespace TPOR{

//! Brachytherapy seed hdf5 file handler
class BrachytherapySeedFileHandler
{

public:

  //! Constructor
  BrachytherapySeedFileHandler( const std::string &seed_file_name );

  //! Destructor
  ~BrachytherapySeedFileHandler();

  //! Return the mesh dimensions
  void getMeshDimensions( std::vector<unsigned> &mesh_dimensions );

  //! Return the mesh element dimensions
  void getMeshElementDimensions( std::vector<double> &element_dimensions );

  //! Return the seed position indices
  void getSeedPosition( std::vector<unsigned> &seed_position );

  //! Return the seed mesh for the desired seed
  void getSeedDataMesh( std::vector<double> &seed_data_mesh,
			const TPOR::BrachytherapySeedType seed_type,
			const double air_kerma_strength );
  
private:

  //! Scale the seed data mesh by a desired air kerma strength
  void scaleSeedDataMesh( std::vector<double> &seed_data_mesh,
			  const double scale_factor );

  // HDF5FileHandler
  HDF5FileHandler d_hdf5_file;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_FILE_HANDLER_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedFileHandler.hpp
//---------------------------------------------------------------------------//
