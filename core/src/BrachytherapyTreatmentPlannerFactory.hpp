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
#include <string>
#include <map>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedProxy.hpp"
#include "BrachytherapyTreatmentPlanner.hpp"
#include "BrachytherapyTreatmentPlannerType.hpp"

namespace TPOR{

//! Brachytherapy treatment planner factory class
class BrachytherapyTreatmentPlannerFactory{

public:

  //! Typedef for a brachytherapy treatment planner class pointer
  typedef boost::shared_ptr<BrachytherapyTreatmentPlanner> 
  BrachytherapyTreatmentPlannerPtr;
  
  //! Constructor
  BrachytherapyTreatmentPlannerFactory( 
      const std::string &patient_hdf5_file_name,
      const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds,
      const double prescribed_dose );

  //! Destructor
  ~BrachytherapyTreatmentPlannerFactory()
  { /* ... */ }

  //! Brachytherapy treatment planner construction method
  BrachytherapyTreatmentPlannerPtr createTreatmentPlanner(
			const BrachytherapyTreatmentPlannerType planner_type );

private:

  // Patient file name
  std::string d_patient_file_name;  

  // Seeds
  std::vector<boost::shared_ptr<BrachytherapySeedProxy> > d_seeds;

  // Prescribed dose
  double d_prescribed_dose;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_TREATMENT_PLANNER_FACTORY_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyTreatmentPlannerFactory.hpp
//---------------------------------------------------------------------------//
