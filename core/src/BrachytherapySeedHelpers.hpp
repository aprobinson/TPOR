//---------------------------------------------------------------------------//
//! 
//! \file   BrachytherapySeedHelpers.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed helper function declarations.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_HELPERS_HPP
#define BRACHYTHERAPY_SEED_HEPLERS_HPP

// Std Lib Includes
#include <string>

// TPOR Includes
#include "BrachytherapySeedType.hpp"
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

namespace TPOR{

//! Return the brachytherapy seed name given a BrachytherapySeedType enum
std::string brachytherapySeedName( const BrachytherapySeedType seed_type );

//! Return the BrachytherapySeedType given an unsigned int
BrachytherapySeedType unsignedToBrachytherapySeedType( const unsigned seed_id);

} // end TPOR namespace

#endif // BRACHYTHERAPY_SEED_HELPERS_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedHelpers.hpp
//---------------------------------------------------------------------------//
