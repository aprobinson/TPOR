//---------------------------------------------------------------------------//
//!
//! \file   ArrayTraits.hpp
//! \author Alex Robinson
//! \brief  Array traits class specializations
//!
//---------------------------------------------------------------------------//

#ifndef ARRAY_TRAITS_HPP
#define ARRAY_TRAITS_HPP

// Std Lib Includes
#include <vector>
#include <string>
#include <cmath>

// TPOR Includes
#include "ArrayTraitsDecl.hpp"
#include "ContractException.hpp"

namespace TPOR{

namespace Traits{

/*! The partial specialization of the ArrayTraits struct for std::vector.
 * \ingroup array_traits
 */
template<typename T>
struct ArrayTraits<std::vector<T> >
{
  //! The size type of the array
  typedef typename std::vector<T>::size_type size_type;
  //! The type containted in the array
  typedef typename std::vector<T>::value_type value_type;
  //! The pointer type of the array
  typedef typename std::vector<T>::pointer pointer;
  //! The const pointer type of the array
  typedef typename std::vector<T>::const_pointer const_pointer;
  
  //! The head pointer of the array
  static inline pointer headPtr( std::vector<T> &array )
  { return &array[0]; }
  
  //! The head pointer of the const array
  static inline const_pointer headPtr( const std::vector<T> &array )
  { return &array[0]; }
  
  //! The size of the array
  static inline size_type size( const std::vector<T> &array )
  { return array.size(); }

  //! Resize the array
  static inline void resize( std::vector<T> &array, size_type n )
  { array.resize( n ); }
};

/*! The partial specialization of the ArrayTraits struct for const std::vector.
 * \ingroup array_traits
 */
template<typename T>
struct ArrayTraits<const std::vector<T> >
{
  //! The size type of the array
  typedef typename std::vector<T>::size_type size_type;
  //! The type containted in the array
  typedef typename std::vector<T>::value_type value_type;
  //! The pointer type of the array
  typedef typename std::vector<T>::const_pointer pointer;
  //! The const pointer type of the array
  typedef typename std::vector<T>::const_pointer const_pointer;
  
  //! The head pointer of the array
  static inline const_pointer headPtr( const std::vector<T> &array )
  { return &array[0]; }
  
  //! The size of the array
  static inline size_type size( const std::vector<T> &array )
  { return array.size(); }

  //! Resize the array
  static inline void resize( const std::vector<T> &array, size_type n )
  { testPrecondition( false ); } // cannot resize a const array
};

/*! The specialization of the ArrayTraits struct for std::string
 * \ingroup array_traits
 */
template<>
struct ArrayTraits<std::string>
{
  //! The size type of the array
  typedef typename std::string::size_type size_type;
  //! The type containted in the array
  typedef typename std::string::value_type value_type;
  //! The pointer type of the array
  typedef typename std::string::pointer pointer;
  //! The const pointer type of the array
  typedef typename std::string::const_pointer const_pointer;
  
  //! The head pointer of the array
  static inline pointer headPtr( std::string &array )
  { return &array[0]; }
  
  //! The head pointer of the const array
  static inline const_pointer headPtr( const std::string &array )
  { return &array[0]; }
  
  //! The size of the array
  static inline size_type size( const std::string &array )
  { return array.size(); }

  //! Resize the array
  static inline void resize( std::string &array, size_type n )
  { array.resize( n ); }
};

/*! The specialization of the ArrayTraits struct for const string
 * \ingroup array_traits
 */
template<>
struct ArrayTraits<const std::string>
{
  //! The size type of the array
  typedef typename std::string::size_type size_type;
  //! The type containted in the array
  typedef typename std::string::value_type value_type;
  //! The pointer type of the array
  typedef typename std::string::const_pointer pointer;
  //! The const pointer type of the array
  typedef typename std::string::const_pointer const_pointer;
  
  //! The head pointer of the array
  static inline const_pointer headPtr( const std::string &array )
  { return &array[0]; }
  
  //! The size of the array
  static inline size_type size( const std::string &array )
  { return array.size(); }

  //! Resize the array
  static inline void resize( const std::string &array, size_type n )
  { testPrecondition( false ); } // cannot resize a const array
};

} // end Traits namespace

} // end TPOR namespace

#endif // end ARRAY_TRAITS_HPP

//---------------------------------------------------------------------------//
// end ArrayTraits.hpp
//---------------------------------------------------------------------------//
