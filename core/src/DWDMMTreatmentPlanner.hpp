//---------------------------------------------------------------------------//
//!
//! \file   DWDMMTreatmentPlanner.hpp
//! \author Alex Robinson
//! \brief  Dynamically weighted dose multiplication method (DWDMM) class 
//!         declaration brachytherapy treatment planner based on Vibha's
//!         algorithm.
//!
//---------------------------------------------------------------------------//

#ifndef DWDMM_TREATMENT_PLANNER_HPP
#define DWDMM_TREATMENT_PLANNER_HPP

// Std Lib Includes
#include <iostream>
#include <vector>

// TPOR Includes
#include "BrachytherapyTreatmentPlanner.hpp"
#include "BrachytherapyPatient.hpp"
#include "BrachytherapyDynamicWeightSeedPosition.hpp"

namespace TPOR{

//! The dynamically weighted dose mult. method treatment planning class
class DWDMMTreatmentPlanner : public BrachytherapyTreatmentPlanner
{

public:

  //! Constructor
  DWDMMTreatmentPlanner( 
	const boost::shared_ptr<BrachytherapyPatient> &patient,
	const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds );

  //! Destructor
  ~DWDMMTreatmentPlanner()
  { /* ... */ }

  //! Calculate optimum treatment plan
  void calculateOptimumTreatmentPlan();

  // Treatment planner name
  static const std::string name;

private:

  //! Update the seed positions
  void updateSeedPositions();

  //! Print the treatment plan summary
  void printTreatmentPlanSummary( std::ostream &os ) const;

  // The patient
  boost::shared_ptr<BrachytherapyPatient> d_patient;

  // Optimization time
  double d_opt_time;

  // Seed positions
  std::list<BrachytherapyDynamicWeightSeedPosition> d_seed_positions;
};

} // end TPOR namespace

#endif // end DWDMM_TREATMENT_PLANNER_HPP

//---------------------------------------------------------------------------//
// end DWDMMTreatmentPlanner.hpp
//---------------------------------------------------------------------------//
