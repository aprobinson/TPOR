//---------------------------------------------------------------------------//
//!
//! \file   HDF5FileHandler_def.hpp
//! \author Alex Robinson
//! \brief  HDF5 file handler template member function definitions.
//!
//---------------------------------------------------------------------------//

#ifndef HDF5_FILE_HANDLER_DEF_HPP
#define HDF5_FILE_HANDLER_DEF_HPP

// TPOR includes
#include "HDF5TypeTraits.hpp"
#include "ArrayTraits.hpp"
#include "ExceptionCatchMacros.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Write data in array to HDF5 file dataset
/*! \tparam Array An array class. Any array class that has a 
 *          TPOR::Traits::ArrayTraits specialization can be used. 
 * \param[in] data The data array to write to the HDF5 file dataset.
 * \param[in] location_in_file The location in the HDF5 file where the data will
 * \pre A valid location string, which is any string that does not start with
 * a "/", must be given to this function.
 * \note Developers: 
 * <ul>
 *  <li> The template parameters should never be given explicitly.
 *       The compiler will be able to deduce the template parameters based on 
 *       the array that is passed to the function.
 *  <li> The TPOR::Traits::ArrayTraits and TPOR::Traits::HDF5TypeTraits 
 *       structs are critical to the generality of this function. Review these 
 *       structs to better understand how this function operates.
 * </ul>
 */
template<typename Array>
void HDF5FileHandler::writeArrayToDataSet( const Array &data,
					   const std::string &location_in_file
					   )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( location_in_file.compare( 0, 1, "/" ) == 0 );

  // Type contained in the array
  typedef typename Traits::ArrayTraits<Array>::value_type value_type;
  
  // Create any parent groups that do not exist yet in the location path
  createParentGroups( location_in_file );
  
  // HDF5 exceptions can be thrown when creating a dataset or writing to a 
  // dataset
  try
  {
    hsize_t dim = getArraySize( data );
    H5::DataSpace space( 1, &dim );
    H5::DataSet dataset(d_hdf5_file->createDataSet( 
				location_in_file,
				Traits::HDF5TypeTraits<value_type>::dataType(),
				space ) );
    dataset.write( getHeadPtr( data ),
		   Traits::HDF5TypeTraits<value_type>::dataType() );
  }
  
  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Read in HDF5 file dataset and save the data to an array
/*! \tparam Array An array class. Any array class that has a 
 *          TPOR::Traits::ArrayTraits specialization can be used. 
 * \param[in,out] data The data array that will be used to store the HDF5 file 
 *                dataset.
 * \param[in] location_in_file The location in the HDF5 file where the data 
 *            will be read from.
 * \pre A valid location string, which is any string that does not start with
 * a "/", must be given to this function.
 * \note Developers: 
 * <ul>
 *  <li> The template parameters should never be given explicitly.
 *       The compiler will be able to deduce the template parameters based on 
 *       the array that is passed to the function.
 *  <li> The TPOR::Traits::ArrayTraits and TPOR::Traits::HDF5TypeTraits 
 *       structs are critical to the generality of this function. Review these 
 *       structs to better understand how this function operates.
 * </ul>
 */
template<typename Array>
void HDF5FileHandler::readArrayFromDataSet( Array &data,
					    const std::string &location_in_file
					    )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( location_in_file.compare( 0, 1, "/" ) == 0 ); 

  // Type contained in the array
  typedef typename Traits::ArrayTraits<Array>::value_type value_type;
  // The size type associated with the array
  typedef typename Traits::ArrayTraits<Array>::size_type size_type;
  
  
  // HDF5 exceptions can be thrown when opening and reading from datasets
  try
  {
    H5::DataSet dataset(d_hdf5_file->openDataSet( location_in_file ) );
    
    // Get the dataspace of the dataset
    H5::DataSpace dataspace = dataset.getSpace();
    
    // Get the number of dimensions in the dataspace
    int rank = dataspace.getSimpleExtentNdims();
    
    // Get the dimension size of each dimension in the dataspace
    hsize_t dims[rank];
    int ndims = dataspace.getSimpleExtentDims( dims, NULL );
    
    // Resize the output array to the size of the dataspace
    size_type size = dims[0];
    for( unsigned int i = 1; i < rank; ++i )
      size *= dims[i];
    
    resizeArray( data, size );
    
    // Read the data in the dataset and save it to the output array
    dataset.read( getHeadPtr( data ),
		  Traits::HDF5TypeTraits<value_type>::dataType() );
  }
  
  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Write attribute to HDF5 file dataset
/*! \tparam Array An array class. Any array class that has a 
 *          TPOR::Traits::ArrayTraits specialization can be used. 
 * \param[in] data The data array to write to the HDF5 file dataset attribute.
 * \param[in] location_in_file The location in the HDF5 file where the dataset
 *            attribute will written.
 * \param[in] attribute_name The name of the dataset attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameters should never be given explicitly.
 *       The compiler will be able to deduce the template parameters based on 
 *       the array that is passed to the function.
 *  <li> The TPOR::Traits::ArrayTraits and TPOR::Traits::HDF5TypeTraits 
 *       structs are critical to the generality of this function. Review these 
 *       structs to better understand how this function operates.
 * </ul>
 */
template<typename Array>
void HDF5FileHandler::writeArrayToDataSetAttribute( const Array &data,
						    const std::string 
						      &dataset_location,
						    const std::string
						      &attribute_name )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( (dataset_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );

  typedef typename Traits::ArrayTraits<Array>::value_type value_type;
    
  // HDF5 exceptions can be thrown when opening a group, creating an attribute,
  // or writing an attribute to a group
  try
  {
    hsize_t dim = getArraySize(data);
    H5::DataSpace space( 1, &dim );
    H5::DataSet dataset(d_hdf5_file->openDataSet( dataset_location ) ); 
    H5::Attribute attribute(dataset.createAttribute( 
				attribute_name, 
				Traits::HDF5TypeTraits<value_type>::dataType(),
				space ) );
    
    attribute.write( Traits::HDF5TypeTraits<value_type>::dataType(),
		     getHeadPtr( data ) );  
  }

  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Read in HDF5 file dataset attribute and save the data to an array
/*! \tparam Array An array class. Any array class that has a 
 *          TPOR::Traits::ArrayTraits specialization can be used.
 * \param[in,out] data The data array that will be used to store the HDF5 file 
 *                dataset attribute.
 * \param[in] location_in_file The location in the HDF5 file where the data 
 *            will be read from.
 * \param[in] attribute_name The name of the dataset attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameters should never be given explicitly.
 *       The compiler will be able to deduce the template parameters based on 
 *       the array that is passed to the function.
 *  <li> The TPOR::Traits::ArrayTraits and TPOR::Traits::HDF5TypeTraits 
 *       structs are critical to the generality of this function. Review these 
 *       structs to better understand how this function operates.
 * </ul>
 */
template<typename Array>
void HDF5FileHandler::readArrayFromDataSetAttribute( Array &data,
						     const std::string 
						       &dataset_location,
						     const std::string 
						       &attribute_name )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( (dataset_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );

  typedef typename Traits::ArrayTraits<Array>::value_type value_type;
  typedef typename Traits::ArrayTraits<Array>::size_type size_type;
  
  // HDF5 exceptions can be thrown when opening and reading from datasets
  try
  {
    H5::DataSet dataset(d_hdf5_file->openDataSet( dataset_location ) );

    // Get the attribute associated with the dataset
    H5::Attribute attribute(dataset.openAttribute( attribute_name) );
    
    // Get the dataspace of the attribute
    H5::DataSpace dataspace = attribute.getSpace();
    
    // Get the number of dimensions in the dataspace
    int rank = dataspace.getSimpleExtentNdims();
    
    // Get the dimension size of each dimension in the dataspace
    hsize_t dims[rank];
    int ndims = dataspace.getSimpleExtentDims( dims, NULL );
    
    // Resize the output array to the size of the dataspace
    size_type size = dims[0];
    for( unsigned int i = 1; i < rank; ++i )
      size *= dims[i];
    
    resizeArray( data, size );
    
    // Read the data in the dataset and save it to the output array
    attribute.read( Traits::HDF5TypeTraits<value_type>::dataType(),
		    getHeadPtr( data ) );
  }
  
  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Write attribute to HDF5 file dataset
/*! \tparam T Data type to be written.
 * \param[in] value The data value to write to the HDF5 file dataset attribute.
 * \param[in] dataset_location The location in the HDF5 file where the dataset
 * attribute will written.
 * \param[in] attribute_name The name of the dataset attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameter should never be given explicitly.
 *       The compiler will be able to deduce the template parameter based on 
 *       the value that is passed to the function.
 *  <li> The TPOR::Traits::HDF5TypeTraits struct is critical to the 
 *       generality of this function. Review this structs to better understand 
 *       how this function operates.
 * </ul>
 */
template<typename T>
void HDF5FileHandler::writeValueToDataSetAttribute( const T &value,
						    const std::string 
						      &dataset_location,
						    const std::string
						      &attribute_name )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( (dataset_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );
  
  // HDF5 exceptions can be thrown when opening a group, creating an attribute,
  // or writing an attribute to a group
  try
  {
    hsize_t dim = 1;
    H5::DataSpace space( 1, &dim );
    H5::DataSet dataset(d_hdf5_file->openDataSet( dataset_location ) ); 
    H5::Attribute attribute(dataset.createAttribute( 
					 attribute_name, 
				         Traits::HDF5TypeTraits<T>::dataType(),
					 space ) );
    
    attribute.write( Traits::HDF5TypeTraits<T>::dataType(),
		     &value );  
  }

  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Read in HDF5 file dataset attribute and save the single value
/*! \tparam T Data type to be read.
 * \param[in,out] value The data value that will store the HDF5 file dataset 
 * attribute.
 * \param[in] dataset_location The location in the HDF5 file where the dataset
 * attribute will written.
 * \param[in] attribute_name The name of the dataset attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameter should never be given explicitly.
 *       The compiler will be able to deduce the template parameter based on 
 *       the value that is passed to the function.
 *  <li> The TPOR::Traits::HDF5TypeTraits struct is critical to the 
 *       generality of this function. Review this structs to better understand 
 *       how this function operates.
 * </ul>
 */
template<typename T>
void HDF5FileHandler::readValueFromDataSetAttribute( T &value,
						     const std::string 
						       &dataset_location,
						     const std::string 
						       &attribute_name )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( (dataset_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );
  
  // HDF5 exceptions can be thrown when opening and reading from datasets
  try
  {
    H5::DataSet dataset(d_hdf5_file->openDataSet( dataset_location ) );

    // Get the attribute associated with the dataset
    H5::Attribute attribute(dataset.openAttribute( attribute_name) );
    
    // Get the dataspace of the attribute
    H5::DataSpace dataspace = attribute.getSpace();
    
    // Get the number of dimensions in the dataspace
    int rank = dataspace.getSimpleExtentNdims();
    
    // Get the dimension size of each dimension in the dataspace
    hsize_t dims[rank];
    int ndims = dataspace.getSimpleExtentDims( dims, NULL );

    // Read the data in the dataset and save it to the output array
    attribute.read( Traits::HDF5TypeTraits<T>::dataType(),
		    &value );
  }
  
  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Write attribute to HDF5 file group
/*! \tparam Array An array class. Any array class that has a 
 *          TPOR::Traits::ArrayTraits specialization can be used.
 * \param[in] data The data array to write to the HDF5 file group attribute.
 * \param[in] group_location The location in the HDF5 file where the group
 *            attribute will written.
 * \param[in] attribute_name The name of the group attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameters should never be given explicitly.
 *       The compiler will be able to deduce the template parameters based on 
 *       the array that is passed to the function.
 *  <li> The TPOR::Traits::ArrayTraits and TPOR::Traits::HDF5TypeTraits 
 *       structs are critical to the generality of this function. Review these 
 *       structs to better understand how this function operates.
 * </ul>
 */
template<typename Array>
void HDF5FileHandler::writeArrayToGroupAttribute( const Array &data,
						  const std::string 
						    &group_location,
						  const std::string
						    &attribute_name )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( (group_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );

  typedef typename Traits::ArrayTraits<Array>::value_type value_type;

  // Create any parent groups that do not exist yet in the location path
  createParentGroups( group_location );
    
  // HDF5 exceptions can be thrown when opening a group, creating an attribute,
  // or writing an attribute to a group
  try
  {
    hsize_t dim = getArraySize( data );
    H5::DataSpace space( 1, &dim );
    H5::Group group(d_hdf5_file->openGroup( group_location ) ); 
    H5::Attribute attribute(group.createAttribute( 
				attribute_name, 
			        Traits::HDF5TypeTraits<value_type>::dataType(),
				space ) );
    
    attribute.write( Traits::HDF5TypeTraits<value_type>::dataType(),
		     getHeadPtr( data ) );  
  }

  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Read in HDF5 file group attribute and save the data to an array
/*! \tparam Array An array class. Any array class that has a 
 *          TPOR::Traits::ArrayTraits specialization can be used.
 * \param[in,out] data The data array that will be used to store the HDF5 file 
 *                group attribute.
 * \param[in] group_location The location in the HDF5 file where the group is
 *            located.
 * \param[in] attribute_name The name of the group attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameters should never be given explicitly.
 *       The compiler will be able to deduce the template parameters based on 
 *       the array that is passed to the function.
 *  <li> The TPOR::Traits::ArrayTraits and TPOR::Traits::HDF5TypeTraits 
 *       structs are critical to the generality of this function. Review these 
 *       structs to better understand how this function operates.
 * </ul>
 */
template<typename Array>
void HDF5FileHandler::readArrayFromGroupAttribute( Array &data,
						   const std::string 
						     &group_location,
						   const std::string 
						     &attribute_name )
{
  // The group_location must be absolute (start with /)
  testPrecondition( (group_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );

  typedef typename Traits::ArrayTraits<Array>::value_type value_type;
  typedef typename Traits::ArrayTraits<Array>::size_type size_type;
  
  // HDF5 exceptions can be thrown when opening and reading from datasets
  try
  {
    H5::Group group(d_hdf5_file->openGroup( group_location ) );

    // Get the attribute associated with the group
    H5::Attribute attribute(group.openAttribute( attribute_name) );
    
    // Get the dataspace of the attribute
    H5::DataSpace dataspace = attribute.getSpace();
    
    // Get the number of dimensions in the dataspace
    int rank = dataspace.getSimpleExtentNdims();
    
    // Get the dimension size of each dimension in the dataspace
    hsize_t dims[rank];
    int ndims = dataspace.getSimpleExtentDims( dims, NULL );
    
    // Resize the output array to the size of the dataspace
    size_type size = dims[0];
    for( unsigned int i = 1; i < rank; ++i )
      size *= dims[i];
    
    resizeArray( data, size );
    
    // Read the data in the dataset and save it to the output array
    attribute.read( Traits::HDF5TypeTraits<value_type>::dataType(),
		    getHeadPtr( data ) );
  }
  
  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Write attribute to HDF5 file group
/*! \tparam T Data type of value to write
 * \param[in] value The data value to write to the HDF5 file group attribute.
 * \param[in] group_location The location in the HDF5 file where the group
 * is located.
 * \param[in] attribute_name The name of the group attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameter should never be given explicitly.
 *       The compiler will be able to deduce the template parameter based on 
 *       the value that is passed to the function.
 *  <li> The TPOR::Traits::HDF5TypeTraits struct is critical to the 
 *       generality of this function. Review this structs to better understand 
 *       how this function operates.
 * </ul>
 */
template<typename T>
void HDF5FileHandler::writeValueToGroupAttribute( const T &value,
						  const std::string 
						    &group_location,
						  const std::string
						    &attribute_name )
{
  // The dataset_location must be absolute (start with /)
  testPrecondition( (group_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );

  // Create any parent groups that do not exist yet in the location path
  createParentGroups( group_location );
    
  // HDF5 exceptions can be thrown when opening a group, creating an attribute,
  // or writing an attribute to a group
  try
  {
    hsize_t dim = 1;
    H5::DataSpace space( 1, &dim );
    H5::Group group(d_hdf5_file->openGroup( group_location ) ); 
    H5::Attribute attribute(group.createAttribute( 
					 attribute_name, 
					 Traits::HDF5TypeTraits<T>::dataType(),
					 space ) );
    
    attribute.write( Traits::HDF5TypeTraits<T>::dataType(),
		     &value );  
  }

  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

// Read in HDF5 file group attribute and save the single value
/*! \tparam T Data type to be read.
 * \param[in,out] value The data value that will store the HDF5 file group 
 * attribute.
 * \param[in] group_location The location in the HDF5 file where the group
 * is located.
 * \param[in] attribute_name The name of the group attribute.
 * \pre 
 * <ul>
 *  <li> A valid location string, which is any string that does not start with
 *       a "/", must be given to this function.
 *  <li> A valid attribute name string, which is any string that does not
 *       contian a "/", must be given to this function.
 * </ul>
 * \note Developers: 
 * <ul>
 *  <li> The template parameter should never be given explicitly.
 *       The compiler will be able to deduce the template parameter based on 
 *       the value that is passed to the function.
 *  <li> The TPOR::Traits::HDF5TypeTraits struct is critical to the 
 *       generality of this function. Review this structs to better understand 
 *       how this function operates.
 * </ul>
 */
template<typename T>
void HDF5FileHandler::readValueFromGroupAttribute( T &value,
						   const std::string 
						     &group_location,
						   const std::string 
						     &attribute_name )
{
  // The group_location must be absolute (start with /)
  testPrecondition( (group_location.compare( 0, 1, "/" ) == 0) );
  // The attribute name can contain any character except /
  testPrecondition( (attribute_name.find( "/" ) == std::string::npos ) );
  
  // HDF5 exceptions can be thrown when opening and reading from datasets
  try
  {
    H5::Group group(d_hdf5_file->openGroup( group_location ) );

    // Get the attribute associated with the group
    H5::Attribute attribute(group.openAttribute( attribute_name) );
    
    // Get the dataspace of the attribute
    H5::DataSpace dataspace = attribute.getSpace();
    
    // Get the number of dimensions in the dataspace
    int rank = dataspace.getSimpleExtentNdims();
    
    // Get the dimension size of each dimension in the dataspace
    hsize_t dims[rank];
    int ndims = dataspace.getSimpleExtentDims( dims, NULL );
    
    // Read the data in the dataset and save it to the output array
    attribute.read( Traits::HDF5TypeTraits<T>::dataType(),
		    &value );
  }
  
  HDF5_EXCEPTION_CATCH_AND_EXIT();
}

} // end TPOR namespace

#endif // end HDF5_FILE_HANDLER_DEF_HPP

//---------------------------------------------------------------------------//
// end HDF5FileHandler_def.hpp
//---------------------------------------------------------------------------//
