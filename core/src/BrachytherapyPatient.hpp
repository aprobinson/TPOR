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
#include <boost/unordered_set.hpp>

// TPOR Includes
#include "TissueType.hpp"
#include "BrachytherapySeedPosition.hpp"
#include "BrachytherapyDynamicWeightSeedPosition.hpp"
#include "BrachytherapySetCoverSeedPosition.hpp"
#include "BrachytherapySeedProxy.hpp"

namespace TPOR{

//! Brachytherapy patient class
class BrachytherapyPatient
{

public:
  
  //! Constructor
  BrachytherapyPatient( const std::string &patient_file_name,
			const double prescribed_dose,
			const double urethra_weight = 1.0,
			const double rectum_weight = 1.0,
			const double margin_weight = 1.0 );

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

  //! Export the patient data to a vtk file for 3D visualization
  void exportDataToVTK( const bool export_treatment_plan = true ) const;

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

  // Weight of the urethra relative to the prostate 
  double d_urethra_weight;
  
  // Weight of the rectum relative to the prostate
  double d_rectum_weight;

  // Weight of the margin relative to the prostate
  double d_margin_weight;

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
  boost::unordered_set<unsigned> d_treatment_plan_needles;

  // Treatment plan position set
  boost::unordered_set<unsigned> d_treatment_plan_positions;

  // Treatment plan dose distribution
  std::vector<double> d_dose_distribution;

  // Cached treatment plan 
  std::list<BrachytherapySeedPosition> d_cached_treatment_plan;

  // Cached treatment plan needle set
  boost::unordered_set<unsigned> d_cached_treatment_plan_needles;

  // Cached treatment plan position set
  boost::unordered_set<unsigned> d_cached_treatment_plan_positions;

  // Cached treatment plan dose distribution
  std::vector<double> d_cached_dose_distribution;
};

//! Generic seed position creation policy
template<typename SeedPosition>
struct SeedPositionCreationPolicy
{
  static void create( std::list<SeedPosition> &seed_positions,
		      const boost::shared_ptr<BrachytherapySeedProxy> &seed,
		      const BrachytherapyPatient &patient,
		      const std::vector<double> &prostate_adjoint_data,
		      const std::vector<double> &urethra_adjoint_data,
		      const std::vector<double> &margin_adjoint_data,
		      const std::vector<double> &rectum_adjoint_data,
		      const unsigned mesh_x_index,
		      const unsigned mesh_y_index,
		      const unsigned mesh_z_index )
  {
    unsigned mask_index = mesh_x_index + mesh_y_index*patient.d_mesh_x_dim +
      mesh_z_index*patient.d_mesh_x_dim*patient.d_mesh_y_dim;
    
    if( patient.d_prostate_mask[mask_index] )
    {
      double weight = 
	(patient.d_urethra_weight*urethra_adjoint_data[mask_index] +
	 patient.d_margin_weight*margin_adjoint_data[mask_index] +
	 patient.d_rectum_weight*rectum_adjoint_data[mask_index])/
	prostate_adjoint_data[mask_index];
      
      seed_positions.push_back( SeedPosition( mesh_x_index, 
					      mesh_y_index, 
					      mesh_z_index, 
					      weight, 
					      seed ) );
    }
  }
};

//! BrachytherapyDynamicWeightSeedPosition creation policy specialization
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
	     const std::vector<double> &rectum_adjoint_data,
	     const unsigned mesh_x_index,
	     const unsigned mesh_y_index,
	     const unsigned mesh_z_index )
  {
    unsigned mask_index = mesh_x_index + mesh_y_index*patient.d_mesh_x_dim +
      mesh_z_index*patient.d_mesh_x_dim*patient.d_mesh_y_dim;
    
    if( patient.d_prostate_mask[mask_index] )
    {
      double weight = 
	(patient.d_urethra_weight*urethra_adjoint_data[mask_index] +
	 patient.d_margin_weight*margin_adjoint_data[mask_index] +
	 patient.d_rectum_weight*rectum_adjoint_data[mask_index])/
	prostate_adjoint_data[mask_index];
      
      const double* weight_multiplier = 
	&patient.d_dose_distribution[mask_index];
      
      seed_positions.push_back( BrachytherapyDynamicWeightSeedPosition(
							     mesh_x_index, 
							     mesh_y_index, 
							     mesh_z_index, 
							     weight,
							     weight_multiplier,
							     seed ) );
    }
  }
};

//! BrachytherapySetCoverSeedPosition creation policy specialization
template<>
struct SeedPositionCreationPolicy<BrachytherapySetCoverSeedPosition>
{
  static void create(
	     std::list<BrachytherapySetCoverSeedPosition> &seed_positions,
	     const boost::shared_ptr<BrachytherapySeedProxy> &seed,
	     const BrachytherapyPatient &patient,
	     const std::vector<double> &prostate_adjoint_data,
	     const std::vector<double> &urethra_adjoint_data,
	     const std::vector<double> &margin_adjoint_data,
	     const std::vector<double> &rectum_adjoint_data,
	     const unsigned mesh_x_index,
	     const unsigned mesh_y_index,
	     const unsigned mesh_z_index )
  {    
    unsigned mask_index = mesh_x_index + mesh_y_index*patient.d_mesh_x_dim +
      mesh_z_index*patient.d_mesh_x_dim*patient.d_mesh_y_dim;
    
    // Create pointers to the dose distribution and the prostate mask
    const std::vector<double>* dose_distribution = 
      &patient.d_dose_distribution;
    const std::vector<bool>* prostate_mask =
      &patient.d_prostate_mask;
    
    if( patient.d_prostate_mask[mask_index] )
    {
      double cost = 
	(patient.d_urethra_weight*urethra_adjoint_data[mask_index] +
	 patient.d_margin_weight*margin_adjoint_data[mask_index] +
	 patient.d_rectum_weight*rectum_adjoint_data[mask_index])/
	prostate_adjoint_data[mask_index];
      
      seed_positions.push_back( BrachytherapySetCoverSeedPosition(
						     mesh_x_index, 
						     mesh_y_index, 
						     mesh_z_index, 
						     cost,
						     patient.d_prescribed_dose,
						     dose_distribution,
						     prostate_mask,
						     patient.d_mesh_x_dim,
						     patient.d_mesh_y_dim,
						     patient.d_mesh_z_dim,
						     seed ) );
      seed_positions.back().updateWeight();
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
