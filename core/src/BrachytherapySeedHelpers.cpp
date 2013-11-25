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

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedHelpers.cpp
//---------------------------------------------------------------------------//
