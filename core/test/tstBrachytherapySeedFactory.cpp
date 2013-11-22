//---------------------------------------------------------------------------//
//!
//! \file   tstBrachytherapySeedFactory.cpp
//! \author Alex Robinson
//! \brief  BrachytherapySeedFactory class unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

// TPOR Includes
#include "BrachytherapySeedFactory.hpp"

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the factory can successfully create every type of seed
BOOST_AUTO_TEST_CASE( createSeed )
{
  TPOR::BrachytherapySeedFactory::BrachytherapySeedPtr seed_ptr = 
    TPOR::BrachytherapySeedFactory::createSeed( TPOR::AMERSHAM_6702_SEED, 
						1.0 );
  TPOR::BrachytherapySeedType seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::AMERSHAM_6702_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::AMERSHAM_6711_SEED,
						      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::AMERSHAM_6711_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::AMERSHAM_6733_SEED,
						      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::AMERSHAM_6733_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::AMERSHAM_9011_SEED,
						      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::AMERSHAM_9011_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( TPOR::BEST_2301_SEED,
							 1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::BEST_2301_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( TPOR::BEST_2335_SEED,
							 1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::BEST_2335_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::NASI_MED_3631_SEED,
						      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::NASI_MED_3631_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::BEBIG_I25_S06_SEED,
						      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::BEBIG_I25_S06_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						    TPOR::IMAGYN_IS_12501_SEED,
						    1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::IMAGYN_IS_12501_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						    TPOR::THERAGENICS_200_SEED,
						    1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::THERAGENICS_200_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						      TPOR::DRAXIMAGE_LS1_SEED,
						      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::DRAXIMAGE_LS1_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
					      TPOR::IMPLANT_SCIENCES_3500_SEED,
					      1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::IMPLANT_SCIENCES_3500_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( TPOR::IBT_1251L_SEED,
							 1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::IBT_1251L_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						    TPOR::ISOAID_IAI_125A_SEED,
						    1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::ISOAID_IAI_125A_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						    TPOR::MBI_SL125_SH125_SEED,
						    1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::MBI_SL125_SH125_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						TPOR::SOURCE_TECH_STM1251_SEED,
						1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::SOURCE_TECH_STM1251_SEED );

  seed_ptr = TPOR::BrachytherapySeedFactory::createSeed( 
						   TPOR::NUCLETRON_130002_SEED,
						   1.0 );
  seed_type = seed_ptr->getSeedType();
  BOOST_CHECK_EQUAL( seed_type, TPOR::NUCLETRON_130002_SEED );
}

//---------------------------------------------------------------------------//
// end tstBrachytherapySeedFactory.cpp
//---------------------------------------------------------------------------//
