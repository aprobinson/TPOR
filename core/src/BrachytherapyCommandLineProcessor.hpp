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
#include <boost/scoped_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedProxy.hpp"
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
  const std::string& getPatientFile() const;
  
  //! Return the treament planner type
  BrachytherapyTreatmentPlannerType getPlannerType() const;

  //! Return the brachytherapy seeds
  const std::vector<boost::shared_ptr<BrachytherapySeedProxy> >& 
  getSeeds() const;

  //! Return the prescribed dose
  double getPrescribedDose() const;

  //! Return the urethra weight
  double getUrethraWeight() const;

  //! Return the rectum weight
  double getRectumWeight() const;

  //! Return the margin weight
  double getMarginWeight() const;

  //! Get the treatment plan output stream
  std::ostream& getTreatmentPlanOutputStream();
  
  //! Get the dose-volume-histogram output stream
  std::ostream& getDVHOutputStream();

private:

  //! Parse the patient file
  void parsePatientFile( boost::program_options::variables_map &vm );

  //! Parse the seed file
  void parseSeedFile( boost::program_options::variables_map &vm );

  //! Parse the treament planner type
  void parseTreatmentPlannerType( boost::program_options::variables_map &vm );

  //! Parse the brachytherapy seeds
  void parseBrachytherapySeeds( boost::program_options::variables_map &vm );

  //! Parse the prescribed dose
  void parsePrescribedDose( boost::program_options::variables_map &vm );

  //! Parse the urethra weight
  void parseUrethraWeight( boost::program_options::variables_map &vm );
  
  //! Parse the rectum weight
  void parseRectumWeight( boost::program_options::variables_map &vm );

  //! Parse the margin weight
  void parseMarginWeight( boost::program_options::variables_map &vm );

  //! Parse the treatment plan output file name
  void parseTreatmentPlanOutputFile( 
				   boost::program_options::variables_map &vm );

  //! Parse the dose-volume-histogram output file name
  void parseDVHOutputFile( boost::program_options::variables_map &vm );

  //! Print the user options summary
  void printUserOptionsSummary();

  // The patient file
  std::string d_patient_file;

  // The seed file
  std::string d_seed_file;
  
  // The treatment planner type
  BrachytherapyTreatmentPlannerType d_planner_type;

  // The seeds
  std::vector<boost::shared_ptr<BrachytherapySeedProxy> > d_seeds;

  // The prescribed dose
  double d_prescribed_dose;

  // The urethra weight
  double d_urethra_weight;
  
  // The rectum weight
  double d_rectum_weight;

  // The margin weight
  double d_margin_weight;

  // The treatment plan output file
  boost::scoped_ptr<std::ostream> d_treatment_plan_os;

  // The dose-volume-histogram output file
  boost::scoped_ptr<std::ostream>  d_dvh_os;
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_COMMAND_LINE_PROCESSOR_HPP

//---------------------------------------------------------------------------//
// end BrachytherapyCommandLineProcessor.hpp
//---------------------------------------------------------------------------//
