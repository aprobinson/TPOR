//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedFactory.cpp
//! \author Alex Robinson
//! \brief  Factory class definition for creating brachytherapy seed objects
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// TPOR Includes
#include "BrachytherapySeedFactory.hpp"
#include "Amersham6702Seed.hpp"
#include "Amersham6711Seed.hpp"
#include "Amersham6733Seed.hpp"
#include "Amersham9011Seed.hpp"
#include "Best2301Seed.hpp"
#include "Best2335Seed.hpp"
#include "NASIMED3631Seed.hpp"
#include "NASIMED3633Seed.hpp"
#include "BebigI25S06Seed.hpp"
#include "ImagynIS12501Seed.hpp"
#include "Theragenics200Seed.hpp"
#include "TheragenicsAgX100Seed.hpp"
#include "DraximageLS1Seed.hpp"
#include "ImplantSciences3500Seed.hpp"
#include "IBt1251LSeed.hpp"
#include "IsoAidIAI125ASeed.hpp"
#include "IsoAidIAPd103ASeed.hpp"
#include "MBISL125SH125Seed.hpp"
#include "SourceTechSTM1251Seed.hpp"
#include "Nucletron130002Seed.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Seed construction method
BrachytherapySeedFactory::BrachytherapySeedPtr
BrachytherapySeedFactory::createSeed( const BrachytherapySeedType seed_name,
				      const double air_kerma_strength,
				      const double orientation_angle )
{
  // Make sure the air kerma strength is valid
  testPrecondition( air_kerma_strength > 0.0 );
  testPrecondition( air_kerma_strength < 
		    std::numeric_limits<double>::infinity() ); 
  
  BrachytherapySeedPtr seed_ptr;

  switch(seed_name)
  {
  case AMERSHAM_6702_SEED:
    seed_ptr.reset( new Amersham6702Seed( air_kerma_strength ) );
    break;
  case AMERSHAM_6711_SEED:
    seed_ptr.reset( new Amersham6711Seed( air_kerma_strength ) );
    break;
  case AMERSHAM_6733_SEED:
    seed_ptr.reset( new Amersham6733Seed( air_kerma_strength ) );
    break;
  case AMERSHAM_9011_SEED:
    seed_ptr.reset( new Amersham9011Seed( air_kerma_strength ) );
    break;
  case BEST_2301_SEED:
    seed_ptr.reset( new Best2301Seed( air_kerma_strength ) );
    break;
  case BEST_2335_SEED:
    seed_ptr.reset( new Best2335Seed( air_kerma_strength ) );
    break;
  case NASI_MED_3631_SEED:
    seed_ptr.reset( new NASIMED3631Seed( air_kerma_strength ) );
    break;
  case NASI_MED_3633_SEED:
    seed_ptr.reset( new NASIMED3633Seed( air_kerma_strength ) );
    break;
  case BEBIG_I25_S06_SEED:
    seed_ptr.reset( new BebigI25S06Seed( air_kerma_strength ) );
    break;
  case IMAGYN_IS_12501_SEED:
    seed_ptr.reset( new ImagynIS12501Seed( air_kerma_strength ) );
    break;
  case THERAGENICS_200_SEED:
    seed_ptr.reset( new Theragenics200Seed( air_kerma_strength) );
    break;
  case THERAGENICS_AGX100_SEED:
    seed_ptr.reset( new TheragenicsAgX100Seed( air_kerma_strength ) );
    break;
  case DRAXIMAGE_LS1_SEED:
    seed_ptr.reset( new DraximageLS1Seed( air_kerma_strength ) );
    break;
  case IMPLANT_SCIENCES_3500_SEED:
    seed_ptr.reset( new ImplantSciences3500Seed( air_kerma_strength ) );
    break;
  case IBT_1251L_SEED:
    seed_ptr.reset( new IBt1251LSeed( air_kerma_strength ) );
    break;
  case ISOAID_IAI_125A_SEED:
    seed_ptr.reset( new IsoAidIAI125ASeed( air_kerma_strength ) );
    break;
  case ISOAID_IAPD_103A_SEED:
    seed_ptr.reset( new IsoAidIAPd103ASeed( air_kerma_strength ) );
    break;
  case MBI_SL125_SH125_SEED:
    seed_ptr.reset( new MBISL125SH125Seed( air_kerma_strength ) );
    break;
  case SOURCE_TECH_STM1251_SEED:
    seed_ptr.reset( new SourceTechSTM1251Seed( air_kerma_strength ) );
    break;
  case NUCLETRON_130002_SEED:
    seed_ptr.reset( new Nucletron130002Seed( air_kerma_strength ) );
    break;
  }

  // Make sure that the seed was created successfully
  testPostcondition( seed_ptr );

  return seed_ptr;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedFactory.cpp
//---------------------------------------------------------------------------//

