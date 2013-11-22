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
#include "ContractException.hpp"


namespace TPOR{

// Constructor
BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerFactory(
				    const std::string &patient_hdf5_file_name )
  : d_patient_file_name( patient_hdf5_file_name )
{ /* ... */ }

// Brachytherapy treatment planner construction method (multiple seed)
BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr 
BrachytherapyTreatmentPlannerFactory::createTreatmentPlanner(
      const BrachytherapyTreatmentPlannerType planner_type,
      const std::vector<BrachytherapySeedFactory::BrachytherapySeedPtr> &seeds,
      const double prescribed_dose )
{
  // Make sure that at least one seed has been requested
  testPrecondition( seeds.size() > 0 );
  
  BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr
    treatment_planner;
  
  switch( planner_type )
  {
  case IIEM_TREATMENT_PLANNER:
    treatment_planner.reset( new IIEMTreatmentPlanner( d_patient_file_name,
						       seeds[0],
						       prescribed_dose ) );
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
