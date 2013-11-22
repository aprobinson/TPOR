//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyCommandLineProcessor.hpp
//! \author Alex Robinson
//! \brief  Command-line processor declaration for brachytherapy treatment
//!         planning.
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_COMMAND_LINE_PROCESSOR_HPP
#define BRACHYTHERAPY_COMMAND_LINE_PROCESSOR_HPP

// Std Lib Includes
#include <vector>
#include <iostream>
#include <fstream>

// Boost Includes
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedFactory.hpp"
#include "BrachytherapyTreatmentPlannerType.hpp"

namespace TPOR{

//! Brachytherapy command-line processor
class BrachytherapyCommandLineProcessor
{
  
public:

  //! Constructor
  BrachytherapyCommandLineProcessor( int argc, char** argv );

  //! Destructor
  ~BrachytherapyCommandLineProcessor()
  { /* ... */ }

  //! Return the patient file
  std::string getPatientFile() const;
  
  //! Return the treament planner type
  BrachytherapyTreatmentPlannerType getPlannerType() const;

  //! Return the brachytherapy seeds
  std::vector<BrachytherapySeedFactory::BrachytherapySeedPtr> getSeeds() const;

  //! Return the prescribed dose
  double getPrescribedDose() const;

  //! Get the treatment plan output stream
  std::ostream& getTreatmentPlanOutputStream();
  
  //! Get the dose-volume-histogram output stream
  std::ostream& getDVHOutputStream();

private:

  //! Parse the patient file
  void parsePatientFile( boost::program_options::variables_map &vm,
			 boost::program_options::options_description &desc );

  //! Parse the treament planner type
  void parseTreatmentPlannerType( 
			   boost::program_options::variables_map &vm,
			   boost::program_options::options_description &desc );

  //! Parse the brachytherapy seeds
  void parseBrachytherapySeeds( 
			   boost::program_options::variables_map &vm,
			   boost::program_options::options_description &desc );

  //! Parse the prescribed dose
  void parsePrescribedDose(boost::program_options::variables_map &vm,
			   boost::program_options::options_description &desc );

  //! Parse the treatment plan output file name
  void parseTreatmentPlanOutputFile( 
			   boost::program_options::variables_map &vm,
			   boost::program_options::options_description &desc );

  //! Parse the dose-volume-histogram output file name
  void parseDVHOutputFile( boost::program_options::variables_map &vm,
			   boost::program_options::options_description &desc );

  // The patient file
  std::string d_patient_file;
  
  // The treatment planner type
  BrachytherapyTreatmentPlannerType d_planner_type;

  // The seeds
  std::vector<BrachytherapySeedFactory::BrachytherapySeedPtr> d_seeds;

  // The prescribed dose
  double d_prescribed_dose;

  // The treatment plan output file
  boost::shared_ptr<std::ostream> d_treatment_plan_output_file;

  // The dose-volume-histogram output file
  boost::shared_ptr<std::ostream>  d_dvh_output_file;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_COMMAND_LINE_PROCESSOR_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyCommandLineProcessor.hpp
//---------------------------------------------------------------------------//
