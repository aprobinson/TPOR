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
  std::list<BrachytherapySeedPosition> getPotentialSeedPositions( 
  const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds ) const;

  //! Return the potential seed positions for the desired seed
  std::list<BrachytherapySeedPosition> getPotentialSeedPositions( 
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

  //! Create potential seed positions
  void createSeedPositions( 
			std::list<BrachytherapySeedPosition> &seed_positions,
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

} // end TPOR namespace

#endif // end BRACHYTHERAPY_PATIENT_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyPatient.hpp
//---------------------------------------------------------------------------//
