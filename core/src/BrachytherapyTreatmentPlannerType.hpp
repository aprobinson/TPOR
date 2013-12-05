//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyTreatmentPlannerType.hpp
//! \author Alex Robinson
//! \brief  BrachytherapyTreatmentPlannerType enumeration.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_TREATMENT_PLANNER_TYPE_HPP
#define BRACHYTHERAPY_TREATMENT_PLANNER_TYPE_HPP

namespace TPOR{

//! Brachytherapy treatment planner type enumeration
enum BrachytherapyTreatmentPlannerType{
  PLANNER_min = 0,
  IIEM_TREATMENT_PLANNER = PLANNER_min,
  DWDMM_TREATMENT_PLANNER,
  SCM_TREATMENT_PLANNER,
  PLANNER_max = SCM_TREATMENT_PLANNER
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_TREATMENT_PLANNER_TYPE_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlannerType.hpp
//---------------------------------------------------------------------------//
