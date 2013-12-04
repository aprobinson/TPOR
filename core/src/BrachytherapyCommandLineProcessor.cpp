//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapyCommandLineProcessor.cpp
//! \author Alex Robinson
//! \brief  Command-line processor definition for brachytherapy treatment
//!         planning.
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <stdlib.h>

// Boost Includes
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>

// TPOR Includes
#include "BrachytherapyCommandLineProcessor.hpp"
#include "BrachytherapySeedHelpers.hpp"

namespace TPOR{

// Constructor
/*! \details This may exit if the incorrect command-line arguments are given.
 */
BrachytherapyCommandLineProcessor::BrachytherapyCommandLineProcessor( 
								  int argc,
								  char** argv )
  : d_patient_file(),
    d_seeds(),
    d_prescribed_dose(),
    d_urethra_weight(),
    d_rectum_weight(),
    d_margin_weight(),
    d_treatment_plan_os(),
    d_dvh_os()
{ 
  // Create seed names
  std::string seed_msg = "add a desired seed with a specified air kerma ";
  seed_msg += "strength (arg = name strength):\n";
  
  for( unsigned seed_id = SEED_min; seed_id <= SEED_max; ++seed_id )
  {  
    BrachytherapySeedType seed_type =
      TPOR::unsignedToBrachytherapySeedType( seed_id );
    
    std::string seed_name = brachytherapySeedName( seed_type );
    std::string seed_nuclide = brachytherapySeedNuclide( seed_type );

    std::ostringstream oss;
    oss << seed_id+1;
    
    seed_msg += "\t" + oss.str() + ".) " + seed_name + 
      " (" + seed_nuclide;
    
    if( brachytherapySeedInProduction( seed_type ) )
      seed_msg += ")\n";
    else
      seed_msg += ", NIP)\n";
  }
  seed_msg += "*NIP = not-in-production\n";
  seed_msg += "default value: Amersham6711Seed 0.55\n";

  // Set the generic program options
  boost::program_options::options_description generic( "Allowed options" );
  generic.add_options()
    ("help,h", "produce help message")
    ("treatment_planner,t", 
     boost::program_options::value<std::string>()->default_value("SCMTreatmentPlanner"), 
     "set the treatment planner:\n"
     "\t1.) IIEMTreatmentPlanner\n"
     "\t2.) DWDMMTreatmentPlanner\n"
     "\t3.) SCMTreatmentPlanner\n")
    ("seed,s", 
     boost::program_options::value<std::vector<std::string> >()->multitoken()->composing(),
     seed_msg.c_str())
    ("prescribed_dose,d", 
     boost::program_options::value<double>()->default_value(145.0),
     "set the prescribed dose (Gy)\n"
     "default value: 145.0 Gy\n")
    ("urethra_weight",
     boost::program_options::value<double>()->default_value(1.0),
     "set the importance (weight) of the urethra relative to the prostate\n"
     "default value: 1.0\n")
    ("rectum_weight",
     boost::program_options::value<double>()->default_value(1.0),
     "set the importance (weight) of the rectum relative to the prostate\n"
     "default value: 1.0\n")
    ("margin_weight",
     boost::program_options::value<double>()->default_value(1.0),
     "set the importance (weight) of the margin relative to the prostate\n"
     "default value: 1.0\n")
    ("plan_output_file", boost::program_options::value<std::string>(),
     "set the treatment plan output file (with path)\n")
    ("dvh_output_file", boost::program_options::value<std::string>(),
     "set the dose-volume-histogram output file (with path)\n");

  // Set the hidden program options (required args)
  boost::program_options::options_description hidden( "Hidden options" );
  hidden.add_options()
    ("patient_file_name", 
     boost::program_options::value<std::string>(),
     "set the patient hdf5 file name (with path)")
    ("seed_file_name", 
     boost::program_options::value<std::string>(),
     "set the seed hdf5 file name (with path)");

  // Create the positional (requierd args) corresponding to the hidden options
  boost::program_options::positional_options_description pd;
  pd.add("patient_file_name", 1);
  pd.add("seed_file_name", 2 );

  // Create the command-line argument parser
  boost::program_options::options_description 
    cmdline_options( "Allowed options" ); 
  cmdline_options.add(generic).add(hidden);
  
  boost::program_options::variables_map vm;
  boost::program_options::store(
		       boost::program_options::command_line_parser(argc, argv).
		       options(cmdline_options).positional(pd).run(), vm);
  boost::program_options::notify( vm );

  if( vm.count( "help" ) )
  {
    std::cout << generic << std::endl;
    exit( 1 );
  }

  // The option parsing must be done in the following order
  parsePatientFile( vm );
  parseSeedFile( vm );
  parseTreatmentPlannerType( vm );
  parseBrachytherapySeeds( vm );
  parsePrescribedDose( vm );
  parseUrethraWeight( vm );
  parseRectumWeight( vm );
  parseMarginWeight( vm );
  parseTreatmentPlanOutputFile( vm );
  parseDVHOutputFile( vm );

  // Print a summary of the options specified by the user
  printUserOptionsSummary();
}

//! Return the patient file
const std::string& BrachytherapyCommandLineProcessor::getPatientFile() const
{
  return d_patient_file;
}
  
//! Return the treament planner type
BrachytherapyTreatmentPlannerType
BrachytherapyCommandLineProcessor::getPlannerType() const
{
  return d_planner_type;
}

// Return the brachytherapy seeds
const std::vector<boost::shared_ptr<BrachytherapySeedProxy> >&
BrachytherapyCommandLineProcessor::getSeeds() const
{
  return d_seeds;
}

// Return the prescribed dose
/*! \details The units of prescribed dose are cGy to conform with the doses
 * returned by brachytherapy seeds (the user specified prescribed dose should 
 * be in units of Gy).
 */
double BrachytherapyCommandLineProcessor::getPrescribedDose() const
{
  return d_prescribed_dose;
}

//! Return the urethra weight
double BrachytherapyCommandLineProcessor::getUrethraWeight() const
{
  return d_urethra_weight;
}

//! Return the rectum weight
double BrachytherapyCommandLineProcessor::getRectumWeight() const
{
  return d_rectum_weight;
}

//! Return the margin weight
double BrachytherapyCommandLineProcessor::getMarginWeight() const
{
  return d_margin_weight;
}

// Get the treatment plan output stream
std::ostream& BrachytherapyCommandLineProcessor::getTreatmentPlanOutputStream()
{
  if( d_treatment_plan_os.get() )
    return *d_treatment_plan_os;
  else
    return std::cout;
}
  
// Get the dose-volume-histogram output stream
std::ostream& BrachytherapyCommandLineProcessor::getDVHOutputStream()
{
  if( d_dvh_os.get() )
    return *d_dvh_os;
  else
    return std::cout;
}

// Parse the patient file
void BrachytherapyCommandLineProcessor::parsePatientFile( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "patient_file_name" ) )
  {
    d_patient_file = vm["patient_file_name"].as<std::string>();
  }
  else
  {
    std::cout << "The patient hdf5 file name (with path) must be specified."
	      << std::endl;
    
    exit( 1 );
  }
}

// Parse the seed file
void BrachytherapyCommandLineProcessor::parseSeedFile( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "seed_file_name" ) )
  {
    d_seed_file = vm["seed_file_name"].as<std::string>();
  }
  else
  {
    std::cout << "The seed hdf5 file name (with path) must be specified."
	      << std::endl;
    
    exit( 1 );
  }
}

// Parse the treament planner type
void BrachytherapyCommandLineProcessor::parseTreatmentPlannerType( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "treatment_planner" ) )
  {
    std::string treatment_planner_name = 
      vm["treatment_planner"].as<std::string>();
    
    if( treatment_planner_name.compare( "IIEMTreatmentPlanner" ) == 0 )
      d_planner_type = IIEM_TREATMENT_PLANNER;
    else if( treatment_planner_name.compare( "DWDMMTreatmentPlanner" ) == 0 )
      d_planner_type = DWDMM_TREATMENT_PLANNER;
    else if( treatment_planner_name.compare( "SCMTreatmentPlanner" ) == 0 )
      d_planner_type = SCM_TREATMENT_PLANNER;
    else
    {
      std::cout << "Invalid treatment planner: " << treatment_planner_name
		<< std::endl;
      exit( 1 );
    }
  }
  else
  {
    std::cout << "The desired treatment planner must be specified."
	      << std::endl;
    exit( 1 );
  }
}

// Parse the brachytherapy seeds
void BrachytherapyCommandLineProcessor::parseBrachytherapySeeds( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "seed" ) )
  {
    std::vector<std::string> seed_names = 
      vm["seed"].as<std::vector<std::string> >();

    if( seed_names.size()%2 == 0 )
    {
      if( d_planner_type == IIEM_TREATMENT_PLANNER && seed_names.size() > 2 )
      {
	std::cout << "Warning: The IIEMTreatmentPlanner only takes one seed. "
		  << "The first seed specified (" << seed_names[0] << ") "
		  << " will be used.";
      }
    
      boost::shared_ptr<BrachytherapySeedProxy> seed;
      double seed_strength = 0.0;
      
      for( unsigned i = 0; i < seed_names.size()/2; ++i )
      {
	// Extract the seed strength
	std::istringstream iss( seed_names[i*2+1] );
	iss >> seed_strength;
	
	// Create the desired seed
	for( unsigned seed_id = SEED_min; seed_id <= SEED_max; ++seed_id )
	{  
	  BrachytherapySeedType test_seed_type =
	    TPOR::unsignedToBrachytherapySeedType( seed_id );
	  
	  std::string test_seed_name = brachytherapySeedName( test_seed_type );
	  
	  if( seed_names[i*2].compare( test_seed_name ) == 0 )
	  {
	    seed.reset( new BrachytherapySeedProxy( d_seed_file,
						    test_seed_type,
						    seed_strength ) );

	    break;
	  }
	}
      
	if( seed )
	{
	  d_seeds.push_back( seed );
	  
	  seed.reset();
	}
	else
	{
	  std::cout << "The seed " << seed_names[i*2] << " is invalid."
		    << std::endl;
	  
	  exit( 1 );
	}
      }
    }
    else
    {
      std::cout << "The seed name and strength must be specified "
		<< "(e.g. -s name strength)" 
		<< std::endl;

      exit( 1 );
    }
  }
  
  // Use the default seed
  else
  {
    double seed_strength = 0.55;
    boost::shared_ptr<BrachytherapySeedProxy> 
      seed( new BrachytherapySeedProxy( d_seed_file,
					AMERSHAM_6711_SEED,
					seed_strength ) );
    if( seed )
      d_seeds.push_back( seed );
  }
}

// Parse the prescribed dose
void BrachytherapyCommandLineProcessor::parsePrescribedDose(
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "prescribed_dose" ) )
  {
    // Convert the specified prescribed dose from Gy to cGy
    d_prescribed_dose = vm["prescribed_dose"].as<double>()*100;
    
    if( d_prescribed_dose < 0.0 )
    {
      std::cout << "The prescribed dose must be greater than 0.0" << std::endl;
      
      exit( 1 );
    }
  }
  else
  {
    std::cout << "The prescribed dose must be specified."
	      << std::endl;

    exit( 1 );
  }
}

// Parse the urethra weight
void BrachytherapyCommandLineProcessor::parseUrethraWeight( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "urethra_weight" ) )
  {
    d_urethra_weight = vm["urethra_weight"].as<double>();

    if( d_urethra_weight < 0.0 )
    {
      std::cout << "The urethra weight must be greater than 0.0" << std::endl;

      exit( 1 );
    }
  }
}
  
// Parse the rectum weight
void BrachytherapyCommandLineProcessor::parseRectumWeight( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "rectum_weight" ) )
  {
    d_rectum_weight = vm["rectum_weight"].as<double>();

    if( d_rectum_weight < 0.0 )
    {
      std::cout << "The rectum weight must be greater than 0.0" << std::endl;

      exit( 1 );
    }
  }
}

// Parse the margin weight
void BrachytherapyCommandLineProcessor::parseMarginWeight( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "margin_weight" ) )
  {
    d_margin_weight = vm["margin_weight"].as<double>();

    if( d_margin_weight < 0.0 )
    {
      std::cout << "The margin weight must be greater than 0.0" << std::endl;

      exit( 1 );
    }
  }
}

// Parse the treatment plan output file name
void BrachytherapyCommandLineProcessor::parseTreatmentPlanOutputFile( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "plan_output_file" ) )
  {
    std::string treatment_plan_output_file_name = 
      vm["plan_output_file"].as<std::string>();
    
    d_treatment_plan_os.reset( 
		    new std::ofstream( treatment_plan_output_file_name.c_str(),
				       std::ofstream::trunc ) );
  }
}

// Parse the dose-volume-histogram output file name
void BrachytherapyCommandLineProcessor::parseDVHOutputFile( 
				    boost::program_options::variables_map &vm )
{
  if( vm.count( "dvh_output_file" ) )
  {
    std::string dvh_output_file_name = 
      vm["dvh_output_file"].as<std::string>();
    
    d_dvh_os.reset( new std::ofstream( dvh_output_file_name.c_str(),
				       std::ofstream::trunc ) );
  }
}

// Print the user options summary
void BrachytherapyCommandLineProcessor::printUserOptionsSummary()
{
  std::cout << std::endl << "...User Options Summary..." << std::endl;
  std::cout << "patient_file:         " << d_patient_file << std::endl;
  std::cout << "seed_file:            " << d_seed_file << std::endl;
  std::cout << "planner:              ";
  switch( d_planner_type )
  {
  case IIEM_TREATMENT_PLANNER:
    std::cout << "IIEMTreatmentPlanner" << std::endl;
    break;
  case DWDMM_TREATMENT_PLANNER:
    std::cout << "DWDMMTreatmentPlanner" << std::endl;
    break;
  case SCM_TREATMENT_PLANNER:
    std::cout << "SCMTreatmentPlanner" << std::endl;
    break;
  }
  
  std::cout << "seeds: " << std::endl;
  for( unsigned i = 0; i < d_seeds.size(); ++i )
  {
    std::cout << "                  " << i+1 << ".) " 
	      << d_seeds[i]->getSeedName() << " " 
	      << d_seeds[i]->getSeedStrength() << std::endl;
  }

  std::cout << "prescribed dose (Gy): " << d_prescribed_dose/100 << std::endl;
  std::cout << "urethra weight:       " << d_urethra_weight << std::endl;
  std::cout << "rectum weight:        " << d_rectum_weight << std::endl;
  std::cout << "margin weight:        " << d_margin_weight << std::endl;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyCommandLineProcessor.cpp
//---------------------------------------------------------------------------//
