//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyTreatmentPlannerFactory.hpp
//! \author Alex Robinson
//! \brief  BrachytherapyTreatentPlannerFactory class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_TREATMENT_PLANNER_FACTORY_HPP
#define BRACHYTHERAPY_TREATMENT_PLANNER_FACTORY_HPP

// Std Lib Includes
#include <vector>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedProxy.hpp"
#include "BrachytherapyTreatmentPlanner.hpp"
#include "BrachytherapyTreatmentPlannerType.hpp"
#include "BrachytherapyPatient.hpp"

namespace TPOR{

//! Brachytherapy treatment planner factory class
class BrachytherapyTreatmentPlannerFactory{

public:

  //! Typedef for a brachytherapy treatment planner class pointer
  typedef boost::shared_ptr<BrachytherapyTreatmentPlanner> 
  BrachytherapyTreatmentPlannerPtr;
  
  //! Constructor
  BrachytherapyTreatmentPlannerFactory( 
	const boost::shared_ptr<BrachytherapyPatient> &patient,
        const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds );

  //! Destructor
  ~BrachytherapyTreatmentPlannerFactory()
  { /* ... */ }

  //! Brachytherapy treatment planner construction method
  BrachytherapyTreatmentPlannerPtr createTreatmentPlanner(
			const BrachytherapyTreatmentPlannerType planner_type );

private:

  // Patient 
  boost::shared_ptr<BrachytherapyPatient> d_patient;

  // Seeds
  std::vector<boost::shared_ptr<BrachytherapySeedProxy> > d_seeds;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_TREATMENT_PLANNER_FACTORY_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlannerFactory.hpp
//---------------------------------------------------------------------------//
