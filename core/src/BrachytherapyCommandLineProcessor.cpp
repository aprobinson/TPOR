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
  // Create the command-line argument parser
  boost::program_options::options_description desc( "Allowed options" );
  desc.add_options()
    ("help", "produce help message")
    ("treatment_planner,t", boost::program_options::value<std::string>(), 
     "set the treatment planner:\n"
     "\t1.) IIEMTreatmentPlanner\n"
     "\t2.) \n"
     "\t3.) \n")
    ("seeds,s", 
     boost::program_options::value<std::vector<std::string> >()->multitoken(), 
     "set the desired seeds (space separated):\n"
     "\t1.) Amersham6702Seed\n"
     "\t2.) Amersham6711Seed\n"
     "\t3.) Amersham6733Seed\n"
     "\t4.) Amersham9011Seed\n"
     "\t5.) BebigI25S06Seed\n"
     "\t6.) Best2301Seed\n"
     "\t7.) Best2335Seed\n"
     "\t8.) DraximageLS1Seed\n"
     "\t9.) IBt1251LSeed\n"
     "\t10.) ImagynIS12501Seed\n"
     "\t11.) ImplantSciences3500Seed\n"
     "\t12.) IsoAidIAI125ASeed\n"
     "\t13.) IsoAidIAIPd103ASeed\n"
     "\t14.) MBISL125SH125Seed\n"
     "\t15.) NASIMED3631Seed\n"
     "\t16.) NASIMED3633Seed\n"
     "\t17.) Nucletron130002Seed\n"
     "\t18.) SourceTechSTM1251Seed\n"
     "\t19.) Theragenics200Seed\n"
     "\t20.) TheragenicsAgX100Seed\n")
    ("prescribed_dose,d", boost::program_options::value<double>(),
     "set the prescribed dose (Gy)")
    ("patient_file_name", boost::program_options::value<std::string>(),
     "set the patient hdf5 file name (with path)")
    ("plan_output_file,f", boost::program_options::value<std::string>(),
     "set the treatment plan output file (with path)")
    ("dvh_output_file,h", boost::program_options::value<std::string>(),
     "set the dose-volume-histogram output file (with path)");

  boost::program_options::positional_options_description pd;
  pd.add("patient_file_name", 1);
  
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
  parseTreatmentPlannerType( vm, desc );
  parseBrachytherapySeeds( vm, desc );
  parsePrescribedDose( vm, desc );
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
const std::vector<BrachytherapySeedFactory::BrachytherapySeedPtr>&
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
  if( d_treatment_plan_os )
    return *d_treatment_plan_os;
  else
    return std::cout;
}
  
// Get the dose-volume-histogram output stream
std::ostream& BrachytherapyCommandLineProcessor::getDVHOutputStream()
{
  if( d_dvh_os )
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
  if( vm.count( "seeds" ) )
  {
    std::vector<std::string> seed_names = 
      vm["seeds"].as<std::vector<std::string> >();
    
    if( d_planner_type == IIEM_TREATMENT_PLANNER && seed_names.size() > 1 )
    {
      std::cout << "Warning: The IIEMTreatmentPlanner only takes one seed. "
		<< "The first seed specified (" << seed_names[0] << ") "
		<< " will be used.";
    }
    
    for( unsigned i = 0; i < seed_names.size(); ++i )
    {
      if( seed_names[i].compare( "Amersham6702Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed( 
								AMERSHAM_6702_SEED,
								 0.508 ) );
      else if( seed_names[i].compare( "Amersham6711Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								AMERSHAM_6711_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "Amersham6733Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								AMERSHAM_6733_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "Amersham9011Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed( 
								AMERSHAM_9011_SEED,
								 0.508 ) );
      else if( seed_names[i].compare( "BebigI25S06Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								BEBIG_I25_S06_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "Best2301Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								BEST_2301_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "Best2335Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								BEST_2335_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "DraximageLS1Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								DRAXIMAGE_LS1_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "IBt1251LSeed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								IBT_1251L_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "ImagynIS12501Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								IMAGYN_IS_12501_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "ImplantSciences3500Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								IMPLANT_SCIENCES_3500_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "IsoAidIAI125ASeed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								ISOAID_IAI_125A_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "IsoAidIAPd103ASeed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								ISOAID_IAPD_103A_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "MBISL125SH125Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								MBI_SL125_SH125_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "NASIMED3631Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								NASI_MED_3631_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "NASIMED3633Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								NASI_MED_3633_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "Nucletron130002Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								NUCLETRON_130002_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "Theragenics200Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								THERAGENICS_200_SEED,
								0.508 ) );
      else if( seed_names[i].compare( "TheragenicsAgX100Seed" ) == 0 )
	d_seeds.push_back( BrachytherapySeedFactory::createSeed(
								THERAGENICS_AGX100_SEED,
								0.508 ) );
      else
      {
	std::cout << "The seed " << seed_names[i] << " is invalid."
		  << std::endl << desc << std::endl;

	exit( 1 );
      }
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
		new std::ofstream( treatment_plan_output_file_name.c_str() ) );
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

    d_dvh_os.reset( new std::ofstream( dvh_output_file_name.c_str() ) );
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyCommandLineProcessor.cpp
//---------------------------------------------------------------------------//
