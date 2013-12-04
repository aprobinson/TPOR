//---------------------------------------------------------------------------//
//!
//! \file   SCMTreatmentPlanner.hpp
//! \author Alex Robinson
//! \brief  Set cover method (SCM) class declaration brachytherapy treatment
//!         planner.
//!
//---------------------------------------------------------------------------//

#ifndef SCM_TREATMENT_PLANNER_HPP
#define SCM_TREATMENT_PLANNER_HPP

// Std Lib Includes
#include <iostream>
#include <vector>

// TPOR Includes
#include "BrachytherapyTreatmentPlanner.hpp"
#include "BrachytherapyPatient.hpp"
#include "BrachytherapySetCoverSeedPosition.hpp"

namespace TPOR{

//! The set cover method treatment planning class
class SCMTreatmentPlanner : public BrachytherapyTreatmentPlanner
{

public:

  //! Constructor
  SCMTreatmentPlanner(
	const boost::shared_ptr<BrachytherapyPatient> &patient,
	const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds );

  //! Destructor
  ~SCMTreatmentPlanner()
  { /* ... */ }

  //! Calculate optimum treatment plan
  void calculateOptimumTreatmentPlan();

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
  std::list<BrachytherapySetCoverSeedPosition> d_seed_positions;
};

} // end TPOR namespace

#endif // end SCM_TREATMENT_PLANNER_HPP

//---------------------------------------------------------------------------//
// end SCMTreatmentPlanner.hpp
//---------------------------------------------------------------------------//

