//---------------------------------------------------------------------------//
//!
//! \file   tstContractException.cpp
//! \author Alex Robinson
//! \brief  Contract exception class unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <stdexcept>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

// FACEMC Includes
#include "TPOR_config.hpp"
#include "ContractException.hpp"

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that a TPOR::ContractException looks different than a 
// std::runtime_error as it inherits from std::logic_error.
BOOST_AUTO_TEST_CASE( differentiation_test )
{
  try
  {
    throw TPOR::ContractException( "TPOR logic error" );
  }
  catch( const std::runtime_error& exception )
  {
    BOOST_CHECK( 0 );
  }
  catch( ... )
  {
    BOOST_CHECK( 1 );
  }
}

//---------------------------------------------------------------------------//
// Check that a TPOR::ContractException can be caught and the appropriate
// error message is written
BOOST_AUTO_TEST_CASE( message_test )
{
  std::string message;

  try
  {
    throw TPOR::ContractException( "contract broken" );
  }
  catch( const TPOR::ContractException& exception )
  {
    message = std::string( exception.what() );
  }
  catch( ... )
  {
    BOOST_CHECK( 0 );
  }

  const std::string true_message( "contract broken" );
  BOOST_CHECK( 0 == message.compare( true_message ) );
}

//---------------------------------------------------------------------------//
// Check that we can throw a TPOR::ContractException with TEST_FOR_EXCEPTION
BOOST_AUTO_TEST_CASE( teuchos_throw_test )
{
  try
  {
    TEST_FOR_EXCEPTION( true,
			TPOR::ContractException,
			"TPOR assertion failed" << std::endl );
  }
  catch( const TPOR::ContractException& assertion )
  {
    BOOST_CHECK( 1 );
  }
  catch( ... )
  {
    BOOST_CHECK( 0 );
  }
}

//---------------------------------------------------------------------------//
// Test the precondition check for DBC
BOOST_AUTO_TEST_CASE( precondition_test )
{

  try
  {
    testPrecondition( 0 );
    throw std::runtime_error( "this shouldn't be thrown" );
  }
  catch( const TPOR::ContractException& exception )
  {
#if HAVE_TPOR_DBC
    std::string message( exception.what() );
    std::string partial_message( "Precondition exception" );
    std::string::size_type idx = message.find( partial_message );
    if( idx == std::string::npos )
    {
      BOOST_CHECK( 0 );
    }
#else
    BOOST_CHECK( 0 );
#endif
  }
  catch( ... )
  {
#if HAVE_TPOR_DBC
    BOOST_CHECK( 0 );
#endif
  }
}

//---------------------------------------------------------------------------//
// Test the postcondition check for DBC.
BOOST_AUTO_TEST_CASE( postcondition_test )
{

  try
  {
    testPostcondition( 0 );
    throw std::runtime_error( "this shouldn't be thrown" );
  }
  catch( const TPOR::ContractException& exception )
  {
#if HAVE_TPOR_DBC
    std::string message( exception.what() );
    std::string partial_message( "Postcondition exception" );
    std::string::size_type idx = message.find( partial_message );
    if( idx == std::string::npos )
    {
      BOOST_CHECK( 0 );
    }
#else
    BOOST_CHECK( 0 );
#endif
  }
  catch( ... )
  {
#if HAVE_TPOR_DBC
    BOOST_CHECK( 0 );
#endif
  }
}

//---------------------------------------------------------------------------//
// Test the invariant check for DBC.
BOOST_AUTO_TEST_CASE( invariant_test )
{

  try
  {
    testInvariant( 0 );
    throw std::runtime_error( "this shouldn't be thrown" );
  }
  catch( const TPOR::ContractException& exception )
  {
#if HAVE_TPOR_DBC
    std::string message( exception.what() );
    std::string partial_message( "Invariant exception" );
    std::string::size_type idx = message.find( partial_message );
    if( idx == std::string::npos )
    {
      BOOST_CHECK( 0 );
    }
#else
    BOOST_CHECK( 0 );
#endif
  }
  catch( ... )
  {
#if HAVE_TPOR_DBC
    BOOST_CHECK( 0 );
#endif
  }
}

//---------------------------------------------------------------------------//
// Test that we can remember a value and check it with DBC
BOOST_AUTO_TEST_CASE( remember_test )
{
  remember( int test_value_1 = 0 );
  remember( int test_value_2 = 1 );

  try
  {
    testInvariant( test_value_1 );
  }
  catch( const TPOR::ContractException& exception )
  {
#if HAVE_TPOR_DBC
    BOOST_CHECK( 1 );
#else
    BOOST_CHECK( 0 );
#endif
  }
  catch( ... )
  {
#if HAVE_TPOR_DBC
    BOOST_CHECK( 0 );
#endif
  }

  try
  {
    testInvariant( test_value_2 );
    BOOST_CHECK( 1 );
  }
  catch( ... )
  {
    BOOST_CHECK( 0 );
  }
}

//---------------------------------------------------------------------------//
// end tstContractException.cpp
//---------------------------------------------------------------------------//
