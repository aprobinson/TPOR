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
#include <boost/chrono.hpp>

// TPOR Includes
#include "BrachytherapyCommandLineProcessor.hpp"
#include "BrachytherapyPatient.hpp"
#include "BrachytherapyTreatmentPlannerFactory.hpp"

//! Main c++ command-line-interface for creating a treatment plan
int main( int argc, char** argv )
{
  // Time the execution of the entire program
  boost::chrono::steady_clock::time_point start_clock = 
    boost::chrono::steady_clock::now();
  
  TPOR::BrachytherapyCommandLineProcessor user_args( argc, argv );

  // Create the patient
  boost::shared_ptr<TPOR::BrachytherapyPatient> 
    patient( new TPOR::BrachytherapyPatient( user_args.getPatientFile(),
					     user_args.getPrescribedDose(),
					     user_args.getUrethraWeight(),
					     user_args.getRectumWeight(),
					     user_args.getMarginWeight() ) );
  
  // Create the treatment planner factory
  TPOR::BrachytherapyTreatmentPlannerFactory
    planner_factory( patient, user_args.getSeeds() );
  
  // Create the treatment planner
  TPOR::BrachytherapyTreatmentPlannerFactory::BrachytherapyTreatmentPlannerPtr
    planner = planner_factory.createTreatmentPlanner( 
						  user_args.getPlannerType() );
					       
  // Create the treatment plan
  planner->calculateOptimumTreatmentPlan();
  
  // Print the program execution time
  boost::chrono::duration<double> seconds =
    boost::chrono::steady_clock::now() - start_clock;
  
  std::cout << "Program Execution Time (s): " << seconds.count() << std::endl;

  // Print the treatment plan
  patient->printTreatmentPlan( user_args.getTreatmentPlanOutputStream() );

  // Print the dose-volume-histogram
  patient->printDoseVolumeHistogramData( user_args.getDVHOutputStream() );
  
  // Export the patient data to vtk
  patient->exportDataToVTK();

  return 0;
 }

//---------------------------------------------------------------------------//
// end treatmentplanner.cpp
//---------------------------------------------------------------------------//
