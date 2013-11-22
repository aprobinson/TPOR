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

// TPOR Includes
#include "BrachytherapyCommandLineProcessor.hpp"
#include "BrachytherapyTreatmentPlannerFactory.hpp"

//! Main c++ command-line-interface for creating a treatment plan
int main( int argc, char** argv )
{
  TPOR::BrachytherapyCommandLineProcessor user_args( argc, argv );
  
  // Create the treatment planner factory
  TPOR::BrachytherapyTreatmentPlannerFactory
    planner_factory( user_args.getPatientFile() );
							    

  // Create the treatment planner
  TPOR::BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr
    planner = planner_factory.createTreatmentPlanner( 
					       user_args.getPlannerType(),
					       user_args.getSeeds(),
					       user_args.getPrescribedDose() );

  // Create the treatment plan
  planner->calculateOptimumTreatmentPlan();

  // Print the treatment plan
  planner->printTreatmentPlan( user_args.getTreatmentPlanOutputStream() );

  // Print the dose-volume-histogram
  planner->printDoseVolumeHistogramData( user_args.getDVHOutputStream() );

  return 0;
 }

//---------------------------------------------------------------------------//
// end treatmentplanner.cpp
//---------------------------------------------------------------------------//
