//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyTreatmentPlannerHelpers.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy treatment planner helper function definitions.
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapyTreatmentPlannerHelpers.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Return the treatment planner name given a BrachytherapyTreatmentPlannerType 
std::string brachytherapyTreatmentPlannerName( 
			 const BrachytherapyTreatmentPlannerType planner_type )
{
  std::string name;

  switch( planner_type )
  {
  case IIEM_TREATMENT_PLANNER:
    name = IIEMTreatmentPlanner::name;
    break;
  case DWDMM_TREATMENT_PLANNER:
    name = DWDMMTreatmentPlanner::name;
    break;
  case SCM_TREATMENT_PLANNER:
    name = SCMTreatmentPlanner::name;
    break;
  }

  // Make sure that the name was set
  testPostcondition( name.size() > 0 );

  return name;
}

//! Return the BrachytherapyTreatmentPlannerType given an unsigned int
BrachytherapyTreatmentPlannerType unsignedToBrachytherapyTreatmentPlannerType(
						    const unsigned planner_id )
{
  // Make sure the planner id is valid
  testPrecondition( planner_id >= PLANNER_min );
  testPrecondition( planner_id <= PLANNER_max );
  
  switch( planner_id )
  {
  case 0u: return IIEM_TREATMENT_PLANNER;
  case 1u: return DWDMM_TREATMENT_PLANNER;
  case 2u: return SCM_TREATMENT_PLANNER;
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlannerHelpers.cpp
//---------------------------------------------------------------------------//
