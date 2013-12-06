//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyPatient_def.hpp
//! \author Alex Robinson
//! \brief  BrachytherapyPatient class template definitions.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_PATIENT_DEF_HPP
#define BRACHYTHERAPY_PATIENT_DEF_HPP

// TPOR Includes
#include "BrachytherapyPatientFileHandler.hpp"
#include "BrachytherapyAdjointDataGenerator.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Get the potential seed positions
template<typename SeedPosition>
std::list<SeedPosition> 
BrachytherapyPatient::getPotentialSeedPositions( 
   const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds ) const
{
  // Create the file handler for the patient
  BrachytherapyPatientFileHandler patient_file( d_patient_file_name );
  
  std::list<SeedPosition> potential_seed_positions;

  std::vector<double> prostate_adjoint_data;
  std::vector<double> urethra_adjoint_data;
  std::vector<double> margin_adjoint_data;
  std::vector<double> rectum_adjoint_data;

  for( unsigned s = 0; s < seeds.size(); ++s )
  {
    // Load the adjoint data for the desired seed if it has been cached  
    if( patient_file.adjointDataExists( seeds[s]->getSeedName() ) )
    {
      // Load in the prostate adjoint data
      patient_file.getProstateAdjointData( prostate_adjoint_data,
					   seeds[s]->getSeedName(),
					   seeds[s]->getSeedStrength() );
      
      // Load in the urethra adjoint data
      patient_file.getUrethraAdjointData( urethra_adjoint_data,
					  seeds[s]->getSeedName(),
					  seeds[s]->getSeedStrength() );
      
      // Load in the margin adjoint data
      patient_file.getMarginAdjointData( margin_adjoint_data,
					 seeds[s]->getSeedName(),
					 seeds[s]->getSeedStrength() );
      
      // Load in the rectum adjoint data
      patient_file.getRectumAdjointData( rectum_adjoint_data,
					 seeds[s]->getSeedName(),
					 seeds[s]->getSeedStrength() );
    }
    
    // Genenerate the adjoint data if it is not in the cache
    else
    {
      BrachytherapyAdjointDataGenerator adjoint_gen( seeds[s] );
      
      std::cout << "generating prostate adjoint data for "
		<< seeds[s]->getSeedName() << "..." << std::endl;
      adjoint_gen.calculateCondensedAdjointDose( prostate_adjoint_data,
						 d_prostate_mask,
						 d_prostate_mask,
						 d_mesh_x_dim,
						 d_mesh_y_dim,
						 d_mesh_z_dim );
      
      std::cout << "generating urethra adjoint data for "
		<< seeds[s]->getSeedName() << "..." << std::endl;
      adjoint_gen.calculateCondensedAdjointDose( urethra_adjoint_data,
						 d_urethra_mask,
						 d_prostate_mask,
						 d_mesh_x_dim,
						 d_mesh_y_dim,
						 d_mesh_z_dim );
      
      std::cout << "generating margin adjoint data for "
		<< seeds[s]->getSeedName() << "..." << std::endl;
      adjoint_gen.calculateCondensedAdjointDose( margin_adjoint_data,
						 d_margin_mask,
						 d_prostate_mask,
						 d_mesh_x_dim,
						 d_mesh_y_dim,
						 d_mesh_z_dim );
      
      std::cout << "generating rectum adjoint data for "
		<< seeds[s]->getSeedName() << "..." << std::endl;
      adjoint_gen.calculateCondensedAdjointDose( rectum_adjoint_data,
						 d_rectum_mask,
						 d_prostate_mask,
						 d_mesh_x_dim,
						 d_mesh_y_dim,
						 d_mesh_z_dim );
      
      // Cache this adjoint data
      patient_file.setProstateAdjointData( prostate_adjoint_data,
					   seeds[s]->getSeedName(),
					   seeds[s]->getSeedStrength() );
      
      patient_file.setUrethraAdjointData( urethra_adjoint_data,
					  seeds[s]->getSeedName(),
					  seeds[s]->getSeedStrength() );
      
      patient_file.setMarginAdjointData( margin_adjoint_data,
					 seeds[s]->getSeedName(),
					 seeds[s]->getSeedStrength() );
      
      patient_file.setRectumAdjointData( rectum_adjoint_data,
					 seeds[s]->getSeedName(),
					 seeds[s]->getSeedStrength() );
    }

    // Filter potential seed positions to those along the template positions
    for( unsigned j = 0; j < d_mesh_y_dim; ++j )
    {
      for( unsigned i = 0; i < d_mesh_x_dim; ++i )
      {
	unsigned needle_index = i + j*d_mesh_x_dim;
	
	if( d_needle_template[needle_index] )
	{
	  for( unsigned slice = 0; slice < d_mesh_z_dim; ++slice )
	  {	    
	    SeedPositionCreationPolicy<SeedPosition>::create( 
						      potential_seed_positions,
						      seeds[s],
						      *this,
						      prostate_adjoint_data,
						      urethra_adjoint_data,
						      margin_adjoint_data,
						      rectum_adjoint_data,
						      i,
						      j, 
						      slice );
	  }
	}
      }
    }
  }

  // Make sure that the seed positions have been created
  testPostcondition( potential_seed_positions.size() > 0 );
  
  return potential_seed_positions;
}

// Get the potential seed positions
template<typename SeedPosition>
std::list<SeedPosition> 
BrachytherapyPatient::getPotentialSeedPositions( 
	          const boost::shared_ptr<BrachytherapySeedProxy> &seed ) const
{
  std::vector<boost::shared_ptr<BrachytherapySeedProxy> > seeds( 1 );
  seeds[0] = seed;

  return getPotentialSeedPositions<SeedPosition>( seeds );
}

} // end TPOR namespace

#endif // end BRACHYTHERAPY_PATIENT_DEF_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyPatient_def.hpp
//---------------------------------------------------------------------------//