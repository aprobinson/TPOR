//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyPatient.hpp
//! \author Alex Robinson
//! \brief  BrachytherapyPatient class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_PATIENT_HPP
#define BRACHYTHERAPY_PATIENT_HPP

// Std Lib Includes
#include <string>
#include <list>
#include <set>
#include <vector>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "TissueType.hpp"
#include "BrachytherapySeedPosition.hpp"
#include "BrachytherapyDynamicWeightSeedPosition.hpp"
#include "BrachytherapySeedProxy.hpp"

namespace TPOR{

//! Brachytherapy patient class
class BrachytherapyPatient
{

public:
  
  //! Constructor
  BrachytherapyPatient( const std::string &patient_file_name,
			const double prescribed_dose );

  //! Destructor
  ~BrachytherapyPatient()
  { /* ... */ }

  //! Insert a seed at the desired seed position and return the seed id number
  unsigned insertSeed( const BrachytherapySeedPosition &seed_position );

  //! Test if the seed position lies on an inserted needle
  bool isSeedOnNeedle( const BrachytherapySeedPosition &seed_position ) const;

  //! Test if the seed position is free
  bool isSeedPositionFree(
			const BrachytherapySeedPosition &seed_position ) const;

  //! Return the dose at a seed position (cGy)
  double getDose( const BrachytherapySeedPosition &seed_position ) const;
  
  //! Return the dose at a mesh point (cGy)
  double getDose( const unsigned x_mesh_index,
		  const unsigned y_mesh_index,
		  const unsigned z_mesh_index ) const;

  //! Return the prescribed dose
  double getPrescribedDose() const;
  
  //! Return the organ mesh x dimension
  unsigned getOrganMeshXDim() const;
  
  //! Return the organ mesh y dimension
  unsigned getOrganMeshYDim() const;
  
  //! Return the organ mesh z dimension
  unsigned getOrganMeshZDim() const;

  //! Return the prostate volume (cm^3)
  double getProstateVolume() const;

  //! Return the urethra volume (cm^3)
  double getUrethraVolume() const;

  //! Return the rectum volume (cm^3)
  double getRectumVolume() const;

  //! Return the normal volume (cm^3)
  double getNormalVolume() const;

  //! Return the prostate size (num prostate elements)
  unsigned getProstateSize() const;

  //! Return the urethra size (num urethra elements)
  unsigned getUrethraSize() const;

  //! Return the rectum size (num rectum elements)
  unsigned getRectumSize() const;

  //! Return the normal size (num normal elements)
  unsigned getNormalSize() const;

  //! Return the tissue type at a mesh point
  TissueType getTissueType( const unsigned x_mesh_index,
			    const unsigned y_mesh_index,
			    const unsigned z_mesh_index ) const;

  //! Return the potential seed positions for the desired seeds
  template<typename SeedPosition>
  std::list<SeedPosition> getPotentialSeedPositions( 
  const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds ) const;

  //! Return the potential seed positions for the desired seed
  template<typename SeedPosition>
  std::list<SeedPosition> getPotentialSeedPositions( 
		 const boost::shared_ptr<BrachytherapySeedProxy> &seed ) const;

  //! Return the prostate dose coverage (% of prostate with >= prescribed dose)
  double getProstateDoseCoverage() const;

  //! Return the number of inserted needles
  unsigned getNumInsertedNeedles() const;

  //! Return the number of inserted seeds
  unsigned getNumInsertedSeeds() const;

  //! Save the current state of the patient
  void saveState();

  //! Load the previously saved state of the patient
  void loadSavedState();

  //! Reset the state of the patient
  void resetState();

  //! Print the treatment plan
  void printTreatmentPlan( std::ostream &os ) const;

  //! Print the treatment plan to std::cout
  void printTreatmentPlan() const;

  //! Print the dose-volume-histogram data
  void printDoseVolumeHistogramData( std::ostream &os ) const;

  //! Print the dose-volume-histogram data to std::cout
  void printDoseVolumeHistogramData() const ;

private:

  //! BrachytherapySeedPosition creation policy
  template<typename SeedPosition>
  friend struct SeedPositionCreationPolicy;

  //! Create potential seed positions
  template<typename SeedPosition>
  void createSeedPositions( 
			std::list<SeedPosition> &seed_positions,
			const boost::shared_ptr<BrachytherapySeedProxy> &seed,
			const std::vector<double> &prostate_adjoint_data,
			const std::vector<double> &urethra_adjoint_data,
			const std::vector<double> &margin_adjoint_data,
			const std::vector<double> &rectum_adjoint_data ) const;
  
  // The patient file name
  std::string d_patient_file_name;

  // Prescribed dose
  double d_prescribed_dose;

  // Mesh dimensions  
  unsigned d_mesh_x_dim;
  unsigned d_mesh_y_dim;
  unsigned d_mesh_z_dim;

  // Prostate volume (number of mesh elements)
  unsigned d_prostate_relative_vol;

  // Urethra volume (number of mesh elements)
  unsigned d_urethra_relative_vol;

  // Rectum volume (number of mesh elements)
  unsigned d_rectum_relative_vol;

  // Normal volume (number of mesh elements)
  unsigned d_normal_relative_vol;

  // Prostate mask
  std::vector<bool> d_prostate_mask;

  // Urethra mask
  std::vector<bool> d_urethra_mask;

  // Margin mask
  std::vector<bool> d_margin_mask;
  
  // Rectum mask
  std::vector<bool> d_rectum_mask;

  // Needle template
  std::vector<bool> d_needle_template;

  // Treatment plan 
  std::list<BrachytherapySeedPosition> d_treatment_plan;

  // Treatment plan needle set
  std::set<unsigned> d_treatment_plan_needles;

  // Treatment plan dose distribution
  std::vector<double> d_dose_distribution;

  // Cached treatment plan 
  std::list<BrachytherapySeedPosition> d_cached_treatment_plan;

  // Cached treatment plan needle set
  std::set<unsigned> d_cached_treatment_plan_needles;

  // Cached treatment plan dose distribution
  std::vector<double> d_cached_dose_distribution;
};

//! BrachytherapySeedPosition creation policy
template<typename SeedPosition>
struct SeedPositionCreationPolicy
{
  static void create( std::list<SeedPosition> &seed_positions,
		      const boost::shared_ptr<BrachytherapySeedProxy> &seed,
		      const BrachytherapyPatient &patient,
		      const std::vector<double> &prostate_adjoint_data,
		      const std::vector<double> &urethra_adjoint_data,
		      const std::vector<double> &margin_adjoint_data,
		      const std::vector<double> &rectum_adjoint_data )
  {
    // Filter potential seed positions to those along the template positions 
    // and set the position weight to the adjoint ratio at the position
    for( unsigned j = 0; j < patient.d_mesh_y_dim; ++j )
    {
      for( unsigned i = 0; i < patient.d_mesh_x_dim; ++i )
      {
	unsigned needle_index = i + j*patient.d_mesh_x_dim;
	
	if( patient.d_needle_template[needle_index] )
	{
	  for( unsigned slice = 0; slice < patient.d_mesh_z_dim; ++slice )
	  {
	    unsigned mask_index = needle_index + 
	      slice*patient.d_mesh_x_dim*patient.d_mesh_y_dim;
	    
	    if( patient.d_prostate_mask[mask_index] )
	    {
	      double weight = 
		(urethra_adjoint_data[mask_index] +
		 margin_adjoint_data[mask_index] +
		 rectum_adjoint_data[mask_index])/
		prostate_adjoint_data[mask_index];
	      
	      seed_positions.push_back(SeedPosition(
						    i, 
						    j, 
						    slice, 
						    weight, 
						    seed ) );
	    }
	  }
	}
      }
    }
  }
};

//! BrachytherapySeedPosition creation policy specialization
template<>
struct SeedPositionCreationPolicy<BrachytherapyDynamicWeightSeedPosition>
{
  static void create(
	     std::list<BrachytherapyDynamicWeightSeedPosition> &seed_positions,
	     const boost::shared_ptr<BrachytherapySeedProxy> &seed,
	     const BrachytherapyPatient &patient,
	     const std::vector<double> &prostate_adjoint_data,
	     const std::vector<double> &urethra_adjoint_data,
	     const std::vector<double> &margin_adjoint_data,
	     const std::vector<double> &rectum_adjoint_data )
  {
    // Organ weights (trial and error by Vibha)
    double prostate_weight = 1.0;
    double urethra_weight = 0.5;
    double margin_weight = 0.5;
    double rectum_weight = 1.0;
    
    // Filter potential seed positions to those along the template positions 
    // and set the position weight to the adjoint ratio at the position
    for( unsigned j = 0; j < patient.d_mesh_y_dim; ++j )
    {
      for( unsigned i = 0; i < patient.d_mesh_x_dim; ++i )
      {
	unsigned needle_index = i + j*patient.d_mesh_x_dim;
	
	if( patient.d_needle_template[needle_index] )
	{
	  for( unsigned slice = 0; slice < patient.d_mesh_z_dim; ++slice )
	  {
	    unsigned mask_index = needle_index + 
	      slice*patient.d_mesh_x_dim*patient.d_mesh_y_dim;
	    
	    if( patient.d_prostate_mask[mask_index] )
	    {
	      double weight = 
		(urethra_weight*urethra_adjoint_data[mask_index] +
		 margin_weight*margin_adjoint_data[mask_index] +
		 rectum_weight*rectum_adjoint_data[mask_index])/
		(prostate_weight*prostate_adjoint_data[mask_index]);
	      
	      const double* weight_multiplier = 
		&patient.d_dose_distribution[mask_index];
	      
	      seed_positions.push_back( BrachytherapyDynamicWeightSeedPosition(
							     i, 
							     j, 
							     slice, 
							     weight,
							     weight_multiplier,
							     seed ) );
	    }
	  }
	}
      }
    }
  }
};

} // end TPOR namespace

//---------------------------------------------------------------------------//
// Template includes.
//---------------------------------------------------------------------------//

#include "BrachytherapyPatient_def.hpp"

//---------------------------------------------------------------------------//

#endif // end BRACHYTHERAPY_PATIENT_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyPatient.hpp
//---------------------------------------------------------------------------//
