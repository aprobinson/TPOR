//---------------------------------------------------------------------------//
//!
//! \file   treatmentplanner.cpp
//! \author Alex Robinson
//! \brief  Main c++ command-line-interface for creating a treatment plan.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <time.h>
#include <string>
#include <vector>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapyCommandLineProcessor.hpp"
#include "BrachytherapyPatient.hpp"
#include "BrachytherapyTreatmentPlannerFactory.hpp"

//! Main c++ command-line-interface for creating a treatment plan
int main( int argc, char** argv )
{
  TPOR::BrachytherapyCommandLineProcessor user_args( argc, argv );

  // Create the patient
  boost::shared_ptr<TPOR::BrachytherapyPatient> 
    patient( new TPOR::BrachytherapyPatient( user_args.getPatientFile(),
					     user_args.getPrescribedDose() ) );
  
  // Create the treatment planner factory
  TPOR::BrachytherapyTreatmentPlannerFactory
    planner_factory( patient, user_args.getSeeds() );
  
  // Create the treatment planner
  TPOR::BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr
    planner = planner_factory.createTreatmentPlanner( 
						  user_args.getPlannerType() );
					       
  // Create the treatment plan
  planner->calculateOptimumTreatmentPlan();

  // Print the treatment plan
  patient->printTreatmentPlan( user_args.getTreatmentPlanOutputStream() );

  // Print the dose-volume-histogram
  patient->printDoseVolumeHistogramData( user_args.getDVHOutputStream() );

  return 0;
 }

//---------------------------------------------------------------------------//
// end treatmentplanner.cpp
//---------------------------------------------------------------------------//
