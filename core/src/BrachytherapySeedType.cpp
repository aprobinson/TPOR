//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedType.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed type to string helper function definition.
//!
//---------------------------------------------------------------------------//

// TPOR Includes
#include "BrachytherapySeedType.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Return a string corresponding to the Brachytherapy seed type
std::string brachytherapySeedTypeToString( const BrachytherapySeedType seed )
{
  std::string seed_string;
  
  switch( seed )
  {
  case AMERSHAM_6702_SEED:
    seed_string = "Amersham6702Seed";
    break;
  case AMERSHAM_6711_SEED:
    seed_string = "Amersham6711Seed";
    break;
  case AMERSHAM_6733_SEED:
    seed_string = "Amersham6733Seed";
    break;
  case AMERSHAM_9011_SEED:
    seed_string = "Amersham9011Seed";
    break;
  case BEST_2301_SEED:
    seed_string = "Best2301Seed";
    break;
  case BEST_2335_SEED:
    seed_string = "Best2335Seed";
    break;
  case NASI_MED_3631_SEED:
    seed_string = "NASIMED3631Seed";
    break;
  case NASI_MED_3633_SEED:
    seed_string = "NASIMED3633Seed";
    break;
  case BEBIG_I25_S06_SEED:
    seed_string = "BebigI25S06Seed";
    break;
  case IMAGYN_IS_12501_SEED:
    seed_string = "ImagynIS12501Seed";
    break;
  case THERAGENICS_200_SEED:
    seed_string = "Theragenics200Seed";
    break;
  case DRAXIMAGE_LS1_SEED:
    seed_string = "DraximageLS1Seed";
    break;
  case IMPLANT_SCIENCES_3500_SEED:
    seed_string = "ImplantSciences3500Seed";
    break;
  case IBT_1251L_SEED:
    seed_string = "IBt1251LSeed";
    break;
  case ISOAID_IAI_125A_SEED:
    seed_string = "IsoAidIAI125ASeed";
    break;
  case ISOAID_IAPD_103A_SEED:
    seed_string = "IsoAidIAPd103ASeed";
    break;
  case MBI_SL125_SH125_SEED:
    seed_string = "MBISL125SH125Seed";
    break;
  case SOURCE_TECH_STM1251_SEED:
    seed_string = "SourceTechSTM1251Seed";
    break;
  case NUCLETRON_130002_SEED:
    seed_string = "Nucletron130002Seed";
    break;
  }

  // Make sure that the string was created successfully
  testPostcondition( seed_string.size() > 0 );

  return seed_string;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedType.cpp
//---------------------------------------------------------------------------//
