//---------------------------------------------------------------------------//
//!
//! \file   BinarySearch.hpp
//! \author Alex Robinson
//! \brief  Binary search algorithm declaration.
//!
//---------------------------------------------------------------------------//

#ifndef BINARY_SEARCH_HPP
#define BINARY_SEARCH_HPP

namespace TPOR{

//! Binary search algorithm
template<typename T>
const T* binarySearch( const T* start, const T* end, const T value );

} // end TPOR namespace

//---------------------------------------------------------------------------//
// Template includes
//---------------------------------------------------------------------------//
 
#include "BinarySearch_def.hpp"

//---------------------------------------------------------------------------//

#endif // end BINARY_SEARCH_HPP

//---------------------------------------------------------------------------//
// end BinarySearch.hpp
//---------------------------------------------------------------------------//
