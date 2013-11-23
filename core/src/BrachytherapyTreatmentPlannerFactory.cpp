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
      const std::string &patient_hdf5_file_name,
      const std::vector<BrachytherapySeedFactory::BrachytherapySeedPtr> &seeds,
      const double prescribed_dose )
  : d_patient_file_name( patient_hdf5_file_name ),
    d_seeds( seeds ),
    d_prescribed_dose( prescribed_dose )
{ 
  // Make sure that at least one seed has been requested
  testPrecondition( seeds.size() > 0 );
  // Make sure the prescribed dose is valid
  testPrecondition( prescribed_dose > 0 );
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
    treatment_planner.reset( new IIEMTreatmentPlanner( d_patient_file_name,
						       d_seeds[0],
						       d_prescribed_dose ) );
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
