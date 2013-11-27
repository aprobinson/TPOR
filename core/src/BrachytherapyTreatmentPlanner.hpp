//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyTreatmentPlanner.hpp
//! \author Alex Robinson
//! \brief  Base class for brachytherapy treatment planning algorithms
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_TREATMENT_PLANNER_HPP
#define BRACHYTHERAPY_TREATMENT_PLANNER_HPP

// Std lib includes
#include <iostream>

namespace TPOR
{

//! Base class for brachytherapy treatment planning algorithms
class BrachytherapyTreatmentPlanner
{
  
public:
  
  //! Default constructor
  BrachytherapyTreatmentPlanner()
  { /* ... */ }

  //! Destructor
  virtual ~BrachytherapyTreatmentPlanner()
  { /* ... */ }

  //! Calculate optimum treatment plan
  virtual void calculateOptimumTreatmentPlan() = 0;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_TREATMENT_PLANNER_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlanner.hpp
//---------------------------------------------------------------------------//
