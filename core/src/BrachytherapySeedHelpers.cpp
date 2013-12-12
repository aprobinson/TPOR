//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedHelpers.cpp
//! \author Alex Robinson
//! \brief  Brachythery seed helper function definitions.
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapySeedHelpers.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Return the brachytherapy seed name give a BrachytherapySeedType enum
std::string brachytherapySeedName( const BrachytherapySeedType seed_type )
{
  std::string name;
  
  switch( seed_type )
  {
  case AMERSHAM_6702_SEED: 
    name = Amersham6702Seed::seed_name;
    break;
  case AMERSHAM_6711_SEED: 
    name = Amersham6711Seed::seed_name;
    break;
  case AMERSHAM_6733_SEED: 
    name = Amersham6733Seed::seed_name;
    break;
  case AMERSHAM_9011_SEED: 
    name = Amersham9011Seed::seed_name;
    break;
  case BEST_2301_SEED: 
    name = Best2301Seed::seed_name;
    break;
  case BEST_2335_SEED: 
    name = Best2335Seed::seed_name;
    break;
  case NASI_MED_3631_SEED: 
    name = NASIMED3631Seed::seed_name;
    break;
  case NASI_MED_3633_SEED: 
    name = NASIMED3633Seed::seed_name;
    break;
  case BEBIG_I25_S06_SEED: 
    name = BebigI25S06Seed::seed_name;
    break;
  case IMAGYN_IS_12501_SEED: 
    name = ImagynIS12501Seed::seed_name;
    break;
  case THERAGENICS_200_SEED: 
    name = Theragenics200Seed::seed_name;
    break;
  case THERAGENICS_AGX100_SEED: 
    name = TheragenicsAgX100Seed::seed_name;
    break;
  case DRAXIMAGE_LS1_SEED: 
    name = DraximageLS1Seed::seed_name;
    break;
  case IMPLANT_SCIENCES_3500_SEED: 
    name = ImplantSciences3500Seed::seed_name;
    break;
  case IBT_1251L_SEED: 
    name = IBt1251LSeed::seed_name;
    break;
  case ISOAID_IAI_125A_SEED: 
    name = IsoAidIAI125ASeed::seed_name;
    break;
  case ISOAID_IAPD_103A_SEED: 
    name = IsoAidIAPd103ASeed::seed_name;
    break;
  case MBI_SL125_SH125_SEED: 
    name = MBISL125SH125Seed::seed_name;
    break;
  case SOURCE_TECH_STM1251_SEED: 
    name = SourceTechSTM1251Seed::seed_name;
    break;
  case NUCLETRON_130002_SEED: 
    name = Nucletron130002Seed::seed_name;
    break;
  }

  // Make sure the name was set
  testPostcondition( name.size() > 0 );

  return name;
}

// Return the radioactive nuclide of the desired brachytherapy seed
std::string brachytherapySeedNuclide( const BrachytherapySeedType seed_type )
{
  std::string nuclide;

  const std::string i125( "I-125" );
  const std::string pd103( "Pd-103" );
  
  switch( seed_type )
  {
  case AMERSHAM_6702_SEED: 
    nuclide = i125;
    break;
  case AMERSHAM_6711_SEED: 
    nuclide = i125;
    break;
  case AMERSHAM_6733_SEED: 
    nuclide = i125;
    break;
  case AMERSHAM_9011_SEED: 
    nuclide = i125;
    break;
  case BEST_2301_SEED: 
    nuclide = i125;
    break;
  case BEST_2335_SEED: 
    nuclide = pd103;
    break;
  case NASI_MED_3631_SEED: 
    nuclide = i125;
    break;
  case NASI_MED_3633_SEED: 
    nuclide = pd103;
    break;
  case BEBIG_I25_S06_SEED: 
    nuclide = i125;
    break;
  case IMAGYN_IS_12501_SEED: 
    nuclide = i125;
    break;
  case THERAGENICS_200_SEED: 
    nuclide = pd103;
    break;
  case THERAGENICS_AGX100_SEED: 
    nuclide = i125;
    break;
  case DRAXIMAGE_LS1_SEED: 
    nuclide = i125;
    break;
  case IMPLANT_SCIENCES_3500_SEED: 
    nuclide = i125;
    break;
  case IBT_1251L_SEED: 
    nuclide = i125;
    break;
  case ISOAID_IAI_125A_SEED: 
    nuclide = i125;
    break;
  case ISOAID_IAPD_103A_SEED: 
    nuclide = pd103;
    break;
  case MBI_SL125_SH125_SEED: 
    nuclide = i125;
    break;
  case SOURCE_TECH_STM1251_SEED: 
    nuclide = i125;
    break;
  case NUCLETRON_130002_SEED: 
    nuclide = i125;
    break;
  }

  // Make sure the name was set
  testPostcondition( nuclide.size() > 0 );

  return nuclide;
}

// Return if the brachytherapy seed is still in production
/*! \details According to the Joint AAPM/RPC Registry of Brachytherapy Sources
 * Meeting of the AAPM Dosimetric Prerequisites (Dec. 3 2013).
 */
bool brachytherapySeedInProduction( const BrachytherapySeedType seed_type )
{
  switch( seed_type )
  {
  case AMERSHAM_6702_SEED: return false;
  case AMERSHAM_6711_SEED: return true;
  case AMERSHAM_6733_SEED: return false;    
  case AMERSHAM_9011_SEED: return true;    
  case BEST_2301_SEED: return true;    
  case BEST_2335_SEED: return true;    
  case NASI_MED_3631_SEED: return false;    
  case NASI_MED_3633_SEED: return false;    
  case BEBIG_I25_S06_SEED: return true;    
  case IMAGYN_IS_12501_SEED: return false;    
  case THERAGENICS_200_SEED: return true;    
  case THERAGENICS_AGX100_SEED: return true;    
  case DRAXIMAGE_LS1_SEED: return false;    
  case IMPLANT_SCIENCES_3500_SEED: return false;    
  case IBT_1251L_SEED: return false;    
  case ISOAID_IAI_125A_SEED: return true;    
  case ISOAID_IAPD_103A_SEED: return true;    
  case MBI_SL125_SH125_SEED: return false;    
  case SOURCE_TECH_STM1251_SEED: return true;    
  case NUCLETRON_130002_SEED: return true;    
  default: return false;
  }
}

// Return the BrachytherapySeedType given an unsigned int
BrachytherapySeedType unsignedToBrachytherapySeedType( const unsigned seed_id )
{
  // Make sure the seed id is valid
  testPrecondition( seed_id >= SEED_min );
  testPrecondition( seed_id <= SEED_max );

  switch( seed_id )
  {
  case 0u: return AMERSHAM_6702_SEED;
  case 1u: return AMERSHAM_6711_SEED;
  case 2u: return AMERSHAM_6733_SEED;
  case 3u: return AMERSHAM_9011_SEED;
  case 4u: return BEST_2301_SEED;
  case 5u: return BEST_2335_SEED;
  case 6u: return NASI_MED_3631_SEED;
  case 7u: return NASI_MED_3633_SEED;
  case 8u: return BEBIG_I25_S06_SEED;
  case 9u: return IMAGYN_IS_12501_SEED;
  case 10u: return THERAGENICS_200_SEED;
  case 11u: return THERAGENICS_AGX100_SEED;
  case 12u: return DRAXIMAGE_LS1_SEED;
  case 13u: return IMPLANT_SCIENCES_3500_SEED;
  case 14u: return IBT_1251L_SEED;
  case 15u: return ISOAID_IAI_125A_SEED;
  case 16u: return ISOAID_IAPD_103A_SEED;
  case 17u: return MBI_SL125_SH125_SEED;
  case 18u: return SOURCE_TECH_STM1251_SEED;
  case 19u: return NUCLETRON_130002_SEED;
  }
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedHelpers.cpp
//---------------------------------------------------------------------------//
