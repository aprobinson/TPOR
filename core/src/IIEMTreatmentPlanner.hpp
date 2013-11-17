//---------------------------------------------------------------------------//
//!
//! \file   IIEMTreatmentPlanner.hpp
//! \author Alex Robinson
//! \brief  Iterative isodose exclusion method (IIEM) class declaration for 
//!         brachytherapy treatment planning based on Sua Yoo's algorithm.
//!
//---------------------------------------------------------------------------//

#ifndef IIEM_TREATMENT_PLANNER_HPP
#define IIEM_TREATMENT_PLANNER_HPP

// Std Lib Includes
#include <string>
#include <vector>
#include <list>
#include <utility>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR includes
#include "BrachytherapyTreatmentPlanner.hpp"
#include "SeedPosition.hpp"
#include "BrachytherapySeedFactory.hpp" 

namespace TPOR
{

//! The iterative isodose exclusion treatment planning class
class IIEMTreatmentPlanner : public BrachytherapyTreatmentPlanner
{

public:

  //! Constructor
  IIEMTreatmentPlanner( const std::string &patient_hdf5_file_name,
			const BrachytherapySeedType desired_seed_type,
			const double desired_seed_strength,
			const double prescribed_dose );

  //! Destructor
  virtual ~IIEMTreatmentPlanner()
  { /* ... */ }

  //! Calculate optimum treatment plan
  void calculateOptimumTreatmentPlan();

  //! Print the treatment plan
  void printTreatmentPlan( std::ostream &os ) const;

  //! Print the treatment plan to std::cout
  void printTreatmentPlan() const;

  //! Print the dose-volume-histogram data
  void printDoseVolumeHistogramData( std::ostream &os ) const;

  //! Print the dose-volume-histogram data to std::cout
  void printDoseVolumeHistogramData() const;

private:

  //! Create the seed position list
  void createSeedPositionList( const std::vector<double> &prostate_adjoint,
			       const std::vector<double> &urethra_adjoint,
			       const std::vector<double> &margin_adjoint,
			       const std::vector<double> &rectum_adjoint );

  //! Conduct the isodose constant iteration 
  void conductIsodoseConstantIteration( 
      unsigned needle_goal,
      std::set<unsigned> &chosen_needle_ids,
      std::list<std::pair<unsigned,SeedPosition> > &remaining_seed_positions );

  //! Conduct the needle isodose constant iteration
  void conductNeedleIsodoseConstantIteration(
			     const double start_constant,
			     const double end_constant,
			     const double step,
                             const std::set<unsigned> &chosen_needle_ids,
                             const std::list<std::pair<unsigned,SeedPosition> >
			     &remaining_seed_positions );

  //! Calculate the minimum seed isodose constant
  double calculateMinSeedIsodoseConstant() const;

  //! Map the seed dose distribution to the problem mesh
  void mapSeedDoseDistribution( const double seed_x_index,
				const double seed_y_index,
				const double seed_z_index );

  //! Calculate the prostate dose coverage (% of vol with prescribed dose)
  double calculateProstateDoseCoverage() const;
  

  //! Calculate the dose-volume-histogram data
  void calculateDoseVolumeHistogramData();				     

  // Prescribed dose
  double d_prescribed_dose;
  
  // Mesh dimensions
  unsigned d_mesh_x_dim;
  unsigned d_mesh_y_dim;
  unsigned d_mesh_z_dim;

  // Mesh element dimensions
  double d_mesh_element_x_dim;
  double d_mesh_element_y_dim;
  double d_mesh_element_z_dim;

  // Prostate volume (number of mesh elements)
  unsigned d_prostate_vol;

  // Minimum number of needles that will be needed
  unsigned d_min_number_of_needles;

  // Minimum seed isodose constant
  double d_min_isodose_constant;

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

  // Brachytherapy Seed
  BrachytherapySeedFactory::BrachytherapySeedPtr d_seed;

  // Seed positions
  std::list<std::pair<unsigned,SeedPosition> > d_seed_positions;

  // Treatment plan (needle id is coupled to a chosen seed position)
  std::list<std::pair<unsigned,SeedPosition> > d_treatment_plan;

  // Treatment plan dose distribution
  std::vector<double> d_dose_distribution;

  // Dose-volume-histogram data
  std::vector<double> d_dose_volume_histogram_data;
};

} // end TPOR namespace

#endif // end IIEM_TREATMENT_PLANNER_HPP

//---------------------------------------------------------------------------//
// end IIEMTreatmentPlanner.hpp
//---------------------------------------------------------------------------//
