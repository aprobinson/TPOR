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

  //! Print the treatment plan
  virtual void printTreatmentPlan( std::ostream &os ) const = 0;

  //! Print the treatment plan to std::cout
  virtual void printTreatmentPlan() const = 0;

  //! Print the dose-volume-histogram data
  virtual void printDoseVolumeHistogramData( std::ostream &os ) const = 0;

  //! Print the dose-volume-histogram data to std::cout
  virtual void printDoseVolumeHistogramData() const = 0;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_TREATMENT_PLANNER_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlanner.hpp
//---------------------------------------------------------------------------//
