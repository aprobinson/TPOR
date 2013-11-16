//---------------------------------------------------------------------------//
//!
//! \file   tstNucletron130002Seed.cpp
//! \author Alex Robinson
//! \brief  Nucletron130002Seed class unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "Nucletron130002Seed.hpp"

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the seed type can be returned
BOOST_AUTO_TEST_CASE( getSeedType )
{
  boost::shared_ptr<TPOR::BrachytherapySeed> 
    seed( new TPOR::Nucletron130002Seed( 1.0 ) );

  TPOR::BrachytherapySeedType seed_type = seed->getSeedType();

  BOOST_CHECK_EQUAL( seed_type, TPOR::NUCLETRON_130002_SEED );
}

//---------------------------------------------------------------------------//
// Check that the dose rate at a point can be calculated
BOOST_AUTO_TEST_CASE( getDoseRate )
{
  boost::shared_ptr<TPOR::BrachytherapySeed>
    seed( new TPOR::Nucletron130002Seed( 1.0 ) );
  
  double x = 1.0, y = 0.0, z = 0.0;
  
  double dose_rate = seed->getDoseRate( x, y, z );

  BOOST_CHECK_EQUAL( dose_rate, 0.954 );
}

//---------------------------------------------------------------------------//
// Check that the total dose at a point can be calculated
BOOST_AUTO_TEST_CASE( getTotalDose )
{
  boost::shared_ptr<TPOR::BrachytherapySeed>
    seed( new TPOR::Nucletron130002Seed( 1.0 ) );
  
  double x = 1.0, y = 0.0, z = 0.0;
  
  double total_dose = seed->getTotalDose( x, y, z );

  BOOST_CHECK_CLOSE( total_dose, 1962.0975719779, 1e-9 );
}

//---------------------------------------------------------------------------//
// end tstNucletron130002Seed.cpp
//---------------------------------------------------------------------------//
