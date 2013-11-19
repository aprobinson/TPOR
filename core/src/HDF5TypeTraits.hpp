//---------------------------------------------------------------------------//
//!
//! \file   HDF5TypeTraits.hpp
//! \author Alex Robinson
//! \brief  HDF5 Type Traits specializations
//!
//---------------------------------------------------------------------------//

#ifndef HDF5_TYPE_TRAITS_HPP
#define HDF5_TYPE_TRAITS_HPP

// Std Lib Includes
#include <utility>

// TPOR Includes
#include "HDF5TypeTraitsDecl.hpp"
#include "ExceptionCatchMacros.hpp"

namespace TPOR{

namespace Traits{
 
/*! \brief The specialization of the TPOR::HDF5TypeTraits for double
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<double>
{
  //! Returns the HDF5 data type object corresponding to double
  static inline H5::PredType dataType() 
  { return H5::PredType::NATIVE_DOUBLE; }

  //! Returns the zero value for this type
  static inline double zero() 
  { return 0.0; }

  //! Returns the unity value for this type
  static inline double one()
  { return 1.0; }
};

/*! \brief The specialization of the TPOR::HDF5TypeTraits for float
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<float>
{
  //! Returns the HDF5 data type object corresponding to double
  static inline H5::PredType dataType() 
  { return H5::PredType::NATIVE_FLOAT; }

  //! Returns the zero value for this type
  static inline float zero() 
  { return 0.0f; }

  //! Returns the unity value for this type
  static inline float one()
  { return 1.0f; }
};

/*! \brief The specialization of the TPOR::HDF5TypeTraits for int
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<int>
{
  //! Returns the HDF5 data type object corresponding to int
  static inline H5::PredType dataType() 
  { return H5::PredType::NATIVE_INT; }

  //! Returns the zero value for this type
  static inline int zero()
  { return 0; }

  //! Returns the unity value for this type
  static inline int one()
  { return 1; }
};

/*! \brief The specialization of the TPOR::HDF5TypeTraits for unsigned int
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<unsigned int>
{
  //! Returns the HDF5 data type object corresponding to unsigned int
  static inline H5::PredType dataType() 
  { return H5::PredType::NATIVE_UINT; }

  //! Returns the zero value for this type
  static inline unsigned zero()
  { return 0u; }

  //! Returns the unity value for this type
  static inline unsigned one()
  { return 1u; }
};

/*! \brief The specialization of the TPOR::HDF5TypeTraits for char
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<char>
{
  //! Returns the HDF5 data type object corresponding to bool
  static inline H5::PredType dataType()
  { return H5::PredType::NATIVE_CHAR; }

  //! Returns the zero value for this type
  static inline char zero()
  { return '0'; }

  //! Returns the unity value for this type
  static inline char one()
  { return '1'; }
};

/*! \brief The specialization of the TPOR::HDF5TypeTraits for signed char
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<signed char>
{
  //! Returns the HDF5 data type object corresponding to bool
  static inline H5::PredType dataType()
  { return H5::PredType::NATIVE_SCHAR; }

  //! Returns the zero value for this type
  static inline signed char zero()
  { return 0; }

  //! Returns the unity value for this type
  static inline signed char one()
  { return 1; }
};

/*! \brief The specialization of the TPOR::HDF5TypeTraits for unsigned char
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<unsigned char>
{
  //! Returns the HDF5 data type object corresponding to bool
  static inline H5::PredType dataType()
  { return H5::PredType::NATIVE_UCHAR; }

  //! Returns the zero value for this type
  static inline unsigned char zero()
  { return 0; }

  //! Returns the unity value for this type
  static inline unsigned char one()
  { return 1; }
};

/*! \brief The partial specialization of the TPOR::HDF5TypeTraits for the 
 * std::pair struct
 * \ingroup hdf5_type_traits
 */
template<typename T, typename T2>
struct HDF5TypeTraits<std::pair<T,T2> >
{
  //! Return the HDF5 data type object corresponding to std::pair<T,T2>
  static inline H5::CompType dataType()
  { 
    typedef std::pair<T,T2> tuple;
    
    H5::CompType memtype( sizeof(tuple) );

    // The insertMember function can throw H5::DataTypeIException exceptions
    try
    {
      memtype.insertMember( "first",
			    HOFFSET( tuple, first ),
			    HDF5TypeTraits<T>::dataType() );

      memtype.insertMember( "second",
			    HOFFSET( tuple, second ),
			    HDF5TypeTraits<T2>::dataType() );
    }
    
    HDF5_EXCEPTION_CATCH_AND_EXIT();

    return memtype;
  }

  //! Returns the zero value for this type
  static inline std::pair<T,T2> zero()
  {
    return std::make_pair( HDF5TypeTraits<T>::zero(), 
			   HDF5TypeTraits<T2>::zero() );
  }

  //! Returns the unity value for this type
  static inline std::pair<T,T2> one()
  {
    return std::make_pair( HDF5TypeTraits<T>::one(),
			   HDF5TypeTraits<T2>::one() );
  }
};

} // end Traits namespace

} // end TPOR namespace

#endif // end HDF5_TYPE_TRAITS_HPP

//---------------------------------------------------------------------------//
// end HDF5TypeTraits.hpp
//---------------------------------------------------------------------------//
