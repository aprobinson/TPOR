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
  seed_msg += "\t1.) " + brachytherapySeedName( AMERSHAM_6702_SEED ) + "\n";
  seed_msg += "\t2.) " + brachytherapySeedName( AMERSHAM_6711_SEED ) + "\n";
  seed_msg += "\t3.) " + brachytherapySeedName( AMERSHAM_6733_SEED ) + "\n";
  seed_msg += "\t4.) " + brachytherapySeedName( AMERSHAM_9011_SEED ) + "\n";
  seed_msg += "\t5.) " + brachytherapySeedName( BEST_2301_SEED ) + "\n";
  seed_msg += "\t6.) " + brachytherapySeedName( BEST_2335_SEED ) + "\n";
  seed_msg += "\t7.) " + brachytherapySeedName( NASI_MED_3631_SEED ) + "\n";
  seed_msg += "\t8.) " + brachytherapySeedName( NASI_MED_3633_SEED ) + "\n";
  seed_msg += "\t9.) " + brachytherapySeedName( BEBIG_I25_S06_SEED ) + "\n";
  seed_msg += "\t10.) " + brachytherapySeedName( IMAGYN_IS_12501_SEED ) + "\n";
  seed_msg += "\t11.) " + brachytherapySeedName( THERAGENICS_200_SEED ) + "\n";
  seed_msg += "\t12.) " + brachytherapySeedName( THERAGENICS_AGX100_SEED ) +
    "\n";
  seed_msg += "\t13.) " + brachytherapySeedName( DRAXIMAGE_LS1_SEED ) + "\n";
  seed_msg += "\t14.) " + brachytherapySeedName( IMPLANT_SCIENCES_3500_SEED )
    + "\n";
  seed_msg += "\t15.) " + brachytherapySeedName( IBT_1251L_SEED ) + "\n";
  seed_msg += "\t16.) " + brachytherapySeedName( ISOAID_IAI_125A_SEED ) + "\n";
  seed_msg += "\t17.) " + brachytherapySeedName( ISOAID_IAPD_103A_SEED ) +
    "\n";
  seed_msg += "\t18.) " + brachytherapySeedName( MBI_SL125_SH125_SEED ) + "\n";
  seed_msg += "\t19.) " + brachytherapySeedName( SOURCE_TECH_STM1251_SEED ) +
    "\n";
  seed_msg += "\t20.) " + brachytherapySeedName( NUCLETRON_130002_SEED ) +"\n";
  
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
	if( seed_names[i*2].compare( "Amersham6702Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  AMERSHAM_6702_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Amersham6711Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  AMERSHAM_6711_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Amersham6733Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,	
						  AMERSHAM_6733_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Amersham9011Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  AMERSHAM_9011_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "BebigI25S06Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  BEBIG_I25_S06_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Best2301Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  BEST_2301_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Best2335Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  BEST_2335_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "DraximageLS1Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  DRAXIMAGE_LS1_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "IBt1251LSeed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  IBT_1251L_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "ImagynIS12501Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  IMAGYN_IS_12501_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "ImplantSciences3500Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  IMPLANT_SCIENCES_3500_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "IsoAidIAI125ASeed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  ISOAID_IAI_125A_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "IsoAidIAPd103ASeed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  ISOAID_IAPD_103A_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "MBISL125SH125Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  MBI_SL125_SH125_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "NASIMED3631Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  NASI_MED_3631_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "NASIMED3633Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  NASI_MED_3633_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Nucletron130002Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  NUCLETRON_130002_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "Theragenics200Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  THERAGENICS_200_SEED,
						  seed_strength ) );
	
	else if( seed_names[i*2].compare( "TheragenicsAgX100Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  THERAGENICS_AGX100_SEED,
						  seed_strength ) );
	else if( seed_names[i*2].compare( "SourceTechSTM1251Seed" ) == 0 )
	  seed.reset( new BrachytherapySeedProxy( d_seed_file,
						  SOURCE_TECH_STM1251_SEED,
						  seed_strength ) );
	else
	{
	  std::cout << "The seed " << seed_names[i*2] << " is invalid."
		    << std::endl << desc << std::endl;
	  
	  exit( 1 );
	}
      
	d_seeds.push_back( seed );
      }
    }
    else
    {
      std::cout << "The seed name and strength must be specified "
		<< "(e.g. name strength)" 
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
