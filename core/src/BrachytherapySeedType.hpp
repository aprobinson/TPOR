//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedType.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed type enumeration
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_TYPE_HPP
#define BRACHYTHERAPY_SEED_TYPE_HPP

// Std Lib Includes
#include <string>

namespace TPOR{

//! Brachytherapy seed type enumeration
enum BrachytherapySeedType{
  SEED_min = 0,
  AMERSHAM_6702_SEED = SEED_min,
  AMERSHAM_6711_SEED,
  AMERSHAM_6733_SEED,
  AMERSHAM_9011_SEED,
  BEST_2301_SEED,
  BEST_2335_SEED,
  NASI_MED_3631_SEED,
  NASI_MED_3633_SEED,
  BEBIG_I25_S06_SEED,
  IMAGYN_IS_12501_SEED,
  THERAGENICS_200_SEED,
  THERAGENICS_AGX100_SEED,
  DRAXIMAGE_LS1_SEED,
  IMPLANT_SCIENCES_3500_SEED,
  IBT_1251L_SEED,
  ISOAID_IAI_125A_SEED,
  ISOAID_IAPD_103A_SEED,
  MBI_SL125_SH125_SEED,
  SOURCE_TECH_STM1251_SEED,
  NUCLETRON_130002_SEED,
  SEED_max = NUCLETRON_130002_SEED
};

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_TYPE_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedType.hpp
//---------------------------------------------------------------------------//
