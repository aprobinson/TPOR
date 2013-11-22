//---------------------------------------------------------------------------//
//! 
//! \file   tstBinarySearch.cpp
//! \author Alex Robinson
//! \brief  Binary search algorithm unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>
#include <boost/array.hpp>

// TPOR Includes
#include "BinarySearch.hpp"

//---------------------------------------------------------------------------//
// Testing Typedefs.
//---------------------------------------------------------------------------//
typedef boost::mpl::list<float, double> test_types;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the binary search algorithm returns the correct array index
BOOST_AUTO_TEST_CASE_TEMPLATE( binarySearch, ScalarType, test_types )
{
  ScalarType value = 5.5;

  boost::array<ScalarType,10> array = 
    {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  
  int index = TPOR::binarySearch( &array[0], &array[9], value );

  BOOST_REQUIRE_EQUAL( index, 5 );

  value = 0.5;

  index = TPOR::binarySearch( &array[0], &array[9], value );

  BOOST_REQUIRE_EQUAL( index, 0 );

  value = 0.0;

  index = TPOR::binarySearch( &array[0], &array[9], value );
  
  BOOST_REQUIRE_EQUAL( index, 0 );
  
  value = 8.5;

  index = TPOR::binarySearch( &array[0], &array[9], value );

  BOOST_REQUIRE_EQUAL( index, 8 );

  value = 9.0;

  index = TPOR::binarySearch( &array[0], &array[9], value );

  BOOST_REQUIRE_EQUAL( index, 8 );
}

//---------------------------------------------------------------------------//
// end tstBinarySearch.cpp
//---------------------------------------------------------------------------//


