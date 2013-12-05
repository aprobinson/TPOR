//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyTreatmentPlannerHelpers.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy treatment planner helper function declarations.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_TREATMENT_PLANNER_HELPERS_HPP
#define BRACHYTHERAPY_TREATMENT_PLANNER_HELPERS_HPP

// Std Lib Includes
#include <string>

// TPOR Includes
#include "BrachytherapyTreatmentPlannerType.hpp"
#include "IIEMTreatmentPlanner.hpp"
#include "DWDMMTreatmentPlanner.hpp"
#include "SCMTreatmentPlanner.hpp"

namespace TPOR{

/*! Return the treatment planner name given a 
 * BrachytherapyTreatmentPlannerType enum
 */
std::string brachytherapyTreatmentPlannerName( 
			const BrachytherapyTreatmentPlannerType planner_type );

//! Return the BrachytherapyTreatmentPlannerType given an unsigned int
BrachytherapyTreatmentPlannerType unsignedToBrachytherapyTreatmentPlannerType(
						   const unsigned planner_id );

} // end TPOR namespace

#endif // end BRACHYTHERAPY_TREATMENT_PLANNER_HELPERS_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlannerHelpers.hpp
//---------------------------------------------------------------------------//
