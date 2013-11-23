//---------------------------------------------------------------------------//
//!
//! \file   tstBrachytherapyCommandLineProcessor.cpp
//! \author Alex Robinson
//! \brief  BrachytherapyCommandLineProcessor class unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

// TPOR Includes
#include "BrachytherapyCommandLineProcessor.hpp"

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the patient file name can be accessed
BOOST_AUTO_TEST_CASE( getPatientFile )
{
  TPOR::BrachytherapyCommandLineProcessor user_args( 
		       boost::unit_test::framework::master_test_suite().argc,
		       boost::unit_test::framework::master_test_suite().argv );
  
  BOOST_CHECK_EQUAL( user_args.getPatientFile(), "test_file.h5" );
}

//---------------------------------------------------------------------------//
// Check that the planner type can be accessed
BOOST_AUTO_TEST_CASE( getPlannerType )
{
  TPOR::BrachytherapyCommandLineProcessor user_args( 
		       boost::unit_test::framework::master_test_suite().argc,
		       boost::unit_test::framework::master_test_suite().argv );
  
  BOOST_CHECK_EQUAL( user_args.getPlannerType(), 
		     TPOR::IIEM_TREATMENT_PLANNER );
}

//---------------------------------------------------------------------------//
// Check that the seeds can be accessed
BOOST_AUTO_TEST_CASE( getSeeds )
{
  TPOR::BrachytherapyCommandLineProcessor user_args( 
		       boost::unit_test::framework::master_test_suite().argc,
		       boost::unit_test::framework::master_test_suite().argv );
  
  BOOST_CHECK_EQUAL( user_args.getSeeds().size(), 1 );
  BOOST_CHECK_EQUAL( user_args.getSeeds()[0]->getSeedType(), 
		     TPOR::AMERSHAM_6711_SEED );
}

//---------------------------------------------------------------------------//
// Check that the prescribed dose can be accessed
BOOST_AUTO_TEST_CASE( getPrescribedDose )
{
  TPOR::BrachytherapyCommandLineProcessor user_args( 
		       boost::unit_test::framework::master_test_suite().argc,
		       boost::unit_test::framework::master_test_suite().argv );
  
  BOOST_CHECK_EQUAL( user_args.getPrescribedDose(), 14500.0 );
}

//---------------------------------------------------------------------------//
// end tstBrachytherapyCommandLineProcessor.cpp
//---------------------------------------------------------------------------//
