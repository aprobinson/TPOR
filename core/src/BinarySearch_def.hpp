//---------------------------------------------------------------------------//
//!
//! \file   BinarySearch_def.hpp
//! \author Alex Robinson
//! \brief  Binary search algorithm definition.
//!
//---------------------------------------------------------------------------//

#ifndef BINARY_SEARCH_DEF_HPP
#define BINARY_SEARCH_DEF_HPP

// TPOR Includes
#include "ContractException.hpp"

namespace TPOR{

// Binary search algorithm
/*! This algorithm should only be used on arrays that represent continuous 
 * data. Using this algorithm on arrays that represent discrete data will not
 * result in correct results.
 */
template<typename T>
int binarySearch( const T* start, const T* end, const T value )
{
  // The array must be valid (size > 0)
  testPrecondition( start != end );
  // The value used for the search must be within the limits of the sorted data
  testPrecondition( value >= *start );
  testPrecondition( value <= *end );

  // Make a copy of the start pointer
  const T* start_copy = start;

  int distance = end-start+1;

  while( distance > 1 )
  {
    if( value >= *(start+distance/2) )
      start += distance/2;
    else
      end = start+distance/2;

    distance = end-start;
  }

  return start-start_copy;
}

} // end TPOR namespace

#endif // end BINARY_SEARCH_DEF_HPP

//---------------------------------------------------------------------------//
// end BinarySearch_def.hpp
//---------------------------------------------------------------------------//
