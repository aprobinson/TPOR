//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyTreatmentPlannerFactory.cpp
//! \author Alex Robinson
//! \brief  BrachytherapyTreatmentPlannerFactory class definition
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapyTreatmentPlannerFactory.hpp"
#include "IIEMTreatmentPlanner.hpp"
#include "DWDMMTreatmentPlanner.hpp"
#include "ContractException.hpp"


namespace TPOR{

// Constructor
BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerFactory(
	 const boost::shared_ptr<BrachytherapyPatient> &patient,
	 const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds )
  : d_patient( patient ),
    d_seeds( seeds )
{ 
  // Make sure that at least one seed has been requested
  testPrecondition( seeds.size() > 0 );
}

// Brachytherapy treatment planner construction method 
BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr 
BrachytherapyTreatmentPlannerFactory::createTreatmentPlanner(
			 const BrachytherapyTreatmentPlannerType planner_type )
{
  BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr
    treatment_planner;
  
  switch( planner_type )
  {
  case IIEM_TREATMENT_PLANNER:
    treatment_planner.reset( new IIEMTreatmentPlanner( d_patient, d_seeds[0]));
    break;
  case DWDMM_TREATMENT_PLANNER:
    treatment_planner.reset( new DWDMMTreatmentPlanner( d_patient, d_seeds ) );
    break;
  }

  // Make sure that the treatment planner was created successfully
  testPostcondition( treatment_planner );

  return treatment_planner;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlannerFactory.cpp
//---------------------------------------------------------------------------//
