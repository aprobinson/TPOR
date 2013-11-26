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

    std::ostringstream oss;
    oss << seed_id+1;
    
    seed_msg += "\t" + oss.str() + ".) " + seed_name + "\n";
  }
  
  // Create the command-line argument parser
  boost::program_options::options_description desc( "Allowed options" );
  desc.add_options()
    ("help", "produce help message")
    ("patient_file_name", 
     boost::program_options::value<std::string>(),
     "set the patient hdf5 file name (with path)")
    ("seed_file_name", 
     boost::program_options::value<std::string>(),
     "set the seed hdf5 file name (with path)")
    ("treatment_planner,t", boost::program_options::value<std::string>(), 
     "set the treatment planner:\n"
     "\t1.) IIEMTreatmentPlanner\n"
     "\t2.) \n"
     "\t3.) \n")
    ("seed,s", 
     boost::program_options::value<std::vector<std::string> >()->multitoken()->composing(), 
     seed_msg.c_str())
    ("prescribed_dose,d", 
     boost::program_options::value<double>()->default_value(145.0),
     "set the prescribed dose (Gy)\n"
     "default value: 145.0 Gy")
    ("plan_output_file", boost::program_options::value<std::string>(),
     "set the treatment plan output file (with path)")
    ("dvh_output_file", boost::program_options::value<std::string>(),
     "set the dose-volume-histogram output file (with path)");

  boost::program_options::positional_options_description pd;
  pd.add("patient_file_name", 1);
  pd.add("seed_file_name", 2 );
  
  boost::program_options::variables_map vm;
  boost::program_options::store(
		       boost::program_options::command_line_parser(argc, argv).
		       options(desc).positional(pd).run(), vm);
  boost::program_options::notify( vm );

  if( vm.count( "help" ) )
  {
    std::cout << desc << std::endl;
    exit( 1 );
  }

  parsePatientFile( vm, desc );
  parseSeedFile( vm, desc );
  parseTreatmentPlannerType( vm, desc );
  parseBrachytherapySeeds( vm, desc );
  parsePrescribedDose( vm, desc );
  parseTreatmentPlanOutputFile( vm, desc );
  parseDVHOutputFile( vm, desc );
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
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
{
  if( vm.count( "patient_file_name" ) )
  {
    d_patient_file = vm["patient_file_name"].as<std::string>();
  }
  else
  {
    std::cout << "The patient hdf5 file name (with path) must be specified."
	      << std::endl << desc << std::endl;
    
    exit( 1 );
  }
}

// Parse the seed file
void BrachytherapyCommandLineProcessor::parseSeedFile( 
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
{
  if( vm.count( "seed_file_name" ) )
  {
    d_seed_file = vm["seed_file_name"].as<std::string>();
  }
  else
  {
    std::cout << "The seed hdf5 file name (with path) must be specified."
	      << std::endl << desc << std::endl;
    
    exit( 1 );
  }
}

// Parse the treament planner type
void BrachytherapyCommandLineProcessor::parseTreatmentPlannerType( 
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
{
  if( vm.count( "treatment_planner" ) )
  {
    std::string treatment_planner_name = 
      vm["treatment_planner"].as<std::string>();
    
    if( treatment_planner_name.compare( "IIEMTreatmentPlanner" ) == 0 )
      d_planner_type = IIEM_TREATMENT_PLANNER;
    else
    {
      std::cout << "Invalid treatment planner: " << treatment_planner_name
		<< std::endl << desc << std::endl;
      exit( 1 );
    }
  }
  else
  {
    std::cout << "The desired treatment planner must be specified."
	      << std::endl << desc << std::endl;
    exit( 1 );
  }
}

// Parse the brachytherapy seeds
void BrachytherapyCommandLineProcessor::parseBrachytherapySeeds( 
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
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
		    << std::endl << desc << std::endl;
	  
	  exit( 1 );
	}
      }
    }
    else
    {
      std::cout << "The seed name and strength must be specified "
		<< "(e.g. -s name strength)" 
		<< std::endl << desc << std::endl;

      exit( 1 );
    }
  }
  else
  {
    std::cout << "At least one seed must be specified."
	      << std::endl << desc << std::endl;
    exit( 1 );
  }
}

// Parse the prescribed dose
void BrachytherapyCommandLineProcessor::parsePrescribedDose(
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
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
	      << std::endl << desc << std::endl;

    exit( 1 );
  }
}

// Parse the treatment plan output file name
void BrachytherapyCommandLineProcessor::parseTreatmentPlanOutputFile( 
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
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
			    boost::program_options::variables_map &vm,
			    boost::program_options::options_description &desc )
{
  if( vm.count( "dvh_output_file" ) )
  {
    std::string dvh_output_file_name = 
      vm["dvh_output_file"].as<std::string>();
    
    d_dvh_os.reset( new std::ofstream( dvh_output_file_name.c_str(),
				       std::ofstream::trunc ) );
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyCommandLineProcessor.cpp
//---------------------------------------------------------------------------//
