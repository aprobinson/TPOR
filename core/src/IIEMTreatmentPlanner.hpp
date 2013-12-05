//---------------------------------------------------------------------------//
//!
//! \file   IIEMTreatmentPlanner.hpp
//! \author Alex Robinson
//! \brief  Iterative isodose exclusion method (IIEM) class declaration for 
//!         brachytherapy treatment planning based on Sua Yoo's algorithm.
//!
//---------------------------------------------------------------------------//

#ifndef IIEM_TREATMENT_PLANNER_HPP
#define IIEM_TREATMENT_PLANNER_HPP

// Std Lib Includes
#include <list>
#include <string>
#include <iostream>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR includes
#include "BrachytherapyTreatmentPlanner.hpp"
#include "BrachytherapySeedPosition.hpp"
#include "BrachytherapySeedProxy.hpp"
#include "BrachytherapyPatient.hpp"

namespace TPOR
{

//! The iterative isodose exclusion treatment planning class
class IIEMTreatmentPlanner : public BrachytherapyTreatmentPlanner
{

public:

  //! Constructor
  IIEMTreatmentPlanner( const boost::shared_ptr<BrachytherapyPatient> &patient,
			const boost::shared_ptr<BrachytherapySeedProxy> &seed);
  
  //! Destructor
  ~IIEMTreatmentPlanner()
  { /* ... */ }

  //! Calculate optimum treatment plan
  void calculateOptimumTreatmentPlan();
  
  // Treatment planner name
  static const std::string name;

private:

  //! Conduct the isodose constant iteration 
  void conductIsodoseConstantIteration( const unsigned needle_goal );
	      

  //! Conduct the needle isodose constant iteration
  double conductNeedleIsodoseConstantIteration(
	const double start_constant,
	const double end_constant,
	const double step,
        const std::list<BrachytherapySeedPosition> &remaining_seed_positions );

  //! Calculate the minimum seed isodose constant
  double calculateMinSeedIsodoseConstant() const;

  //! Print the treatment plan summary
  void printTreatmentPlanSummary( std::ostream &os ) const;

  // The patient
  boost::shared_ptr<BrachytherapyPatient> d_patient;

  // Minimum number of needles that will be needed
  unsigned d_min_number_of_needles;

  // Minimum seed isodose constant
  double d_min_isodose_constant;

  // Optimization time
  double d_opt_time;

  // Seed positions
  std::list<BrachytherapySeedPosition> d_seed_positions;
};

} // end TPOR namespace

#endif // end IIEM_TREATMENT_PLANNER_HPP

//---------------------------------------------------------------------------//
// end IIEMTreatmentPlanner.hpp
//---------------------------------------------------------------------------//
