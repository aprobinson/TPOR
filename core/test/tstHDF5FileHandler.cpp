//---------------------------------------------------------------------------//
//!
//! \file   tstHDF5FileHandler.cpp
//! \author Alex Robinson
//! \brief  HDF5 file handler unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <vector>
#include <string>
#include <utility>

// Boost Includes
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

// TPOR Includes
#include "HDF5FileHandler.hpp"

//---------------------------------------------------------------------------//
// HDF5 Test File Names.
//---------------------------------------------------------------------------//
#define HDF5_TEST_FILE_NAME "HDF5_test_file.h5"
#define TEST_DATASET_NAME "/data/data_array"
#define TEST_ATTRIBUTE_NAME "attribute"
#define ROOT_GROUP "/"
#define CHILD_GROUP "/child/"

//---------------------------------------------------------------------------//
// Testing Typedefs.
//---------------------------------------------------------------------------//
typedef boost::mpl::list<std::vector<unsigned char>, 
			 std::vector<double>,
			 std::vector<std::pair<unsigned,unsigned> >,
			 std::string> 
array_types;

typedef boost::mpl::list<char, unsigned char, int, unsigned, double> 
native_types;
			 

//---------------------------------------------------------------------------//
// Testing Structs.
//---------------------------------------------------------------------------//
class TestingHDF5FileHandler : public TPOR::HDF5FileHandler
{
public:
  TestingHDF5FileHandler()
    : HDF5FileHandler()
  { /* ... */ }
  
  virtual ~TestingHDF5FileHandler()
  { /* ... */ }

  using TPOR::HDF5FileHandler::createParentGroups;
};  

//---------------------------------------------------------------------------//
// Stream operator for std::pair.
//---------------------------------------------------------------------------//
namespace std{
template<typename T1, typename T2>
std::ostream& operator<<( std::ostream &os,
			  const std::pair<T1,T2> &pair_obj )
{
  os << "{" << pair_obj.first << "," << pair_obj.second << "}";
  return os;
}
}

//---------------------------------------------------------------------------//
// Comparison operators for std::pair.
//---------------------------------------------------------------------------//
template<typename T1, typename T2>
bool operator!=( const std::pair<T1,T2> &left, const std::pair<T1,T2> &right ) 
{ return (left.first != right.first) || (left.second != right.second); }

template<typename T1, typename T2>
bool operator==( const std::pair<T1,T2> &left, const std::pair<T1,T2> &right ) 
{ return (left.first == right.first) && (left.second == right.second); }

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can open and overwrite a file
BOOST_AUTO_TEST_CASE( openHDF5FileAndOverwrite )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  // If the file is not opened successfully, an exception will be thrown
  // and caught inside the member function, causing a program exit. 
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can open a file for read-write
BOOST_AUTO_TEST_CASE( openHDF5FileAndAppend )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  // If the file is not opened successfully, an exception will be thrown
  // and caught inside the member function, causing a program exit. 
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.openHDF5FileAndAppend( HDF5_TEST_FILE_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can open a file for read-only
BOOST_AUTO_TEST_CASE( openHDF5FileAndReadOnly )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  // If the file is not opened successfully, an exception will be thrown
  // and caught inside the member function, causing a program exit. 
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.openHDF5FileAndReadOnly( HDF5_TEST_FILE_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can create the necessary parent groups
// for a dataset
BOOST_AUTO_TEST_CASE( createParentGroups )
{
  TestingHDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );
  
  // If the groups are not made successfully, an exception will be thrown
  // and caught inside of the member function, causing a program exit.
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.createParentGroups( TEST_DATASET_NAME );
  
  hdf5_file_handler.createParentGroups( CHILD_GROUP );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can write an Array of Type to a dataset in 
// an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( writeArrayToDataSet, Array, array_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  typedef typename TPOR::Traits::ArrayTraits<Array>::value_type value_type;
  
  value_type test_value = TPOR::Traits::HDF5TypeTraits<value_type>::zero();
  
  Array data( 100, test_value );

  // If the dataset is not written successfuly, an exception will be thrown
  // and caught inside of the member function, causing a program exit.
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.writeArrayToDataSet( data, TEST_DATASET_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can read an Array of Type from a dataset
// in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( readArrayFromDataSet, Array, array_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  typedef typename TPOR::Traits::ArrayTraits<Array>::value_type value_type;
  
  value_type test_value = TPOR::Traits::HDF5TypeTraits<value_type>::one();

  Array original_data( 3, test_value );
  
  hdf5_file_handler.writeArrayToDataSet( original_data,
					 TEST_DATASET_NAME );

  Array copied_data;
  hdf5_file_handler.readArrayFromDataSet( copied_data,
  					  TEST_DATASET_NAME );
  std::cout << copied_data[0] << std::endl;
  BOOST_CHECK_EQUAL( original_data[0], copied_data[0] );
  BOOST_CHECK_EQUAL( original_data[1], copied_data[1] );
  BOOST_CHECK_EQUAL( original_data[2], copied_data[2] );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can write an Array of Type to a dataset
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( writeArrayToDataSetAttribute, 
			       Array, 
			       array_types)
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  typedef typename TPOR::Traits::ArrayTraits<Array>::value_type value_type;

  value_type test_value = TPOR::Traits::HDF5TypeTraits<value_type>::one();

  Array data( 100, test_value );
  
  hdf5_file_handler.writeArrayToDataSet( data, TEST_DATASET_NAME );

  // If the dataset attribute is not written successfuly, an exception will be 
  // thrown and caught inside of the member function, causing a program exit.
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.writeArrayToDataSetAttribute( data,
						  TEST_DATASET_NAME,
						  TEST_ATTRIBUTE_NAME );
  
  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can read an Array of Type from a dataset
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( readArrayFromDataSetAttribute, 
			       Array, 
			       array_types)
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  typedef typename TPOR::Traits::ArrayTraits<Array>::value_type  value_type;

  value_type test_value = TPOR::Traits::HDF5TypeTraits<value_type>::one();

  Array original_data( 3, test_value );
  
  hdf5_file_handler.writeArrayToDataSet( original_data, TEST_DATASET_NAME );

  hdf5_file_handler.writeArrayToDataSetAttribute( original_data,
						  TEST_DATASET_NAME,
						  TEST_ATTRIBUTE_NAME );

  Array copied_data;
  hdf5_file_handler.readArrayFromDataSetAttribute( copied_data,
						   TEST_DATASET_NAME,
						   TEST_ATTRIBUTE_NAME );
  
  BOOST_CHECK_EQUAL( original_data[0], copied_data[0] );
  BOOST_CHECK_EQUAL( original_data[1], copied_data[1] );
  BOOST_CHECK_EQUAL( original_data[2], copied_data[2] );
  
  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can write a single value to a dataset
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( writeValueToDataSetAttribute, 
			       Type, 
			       native_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  Type test_value = TPOR::Traits::HDF5TypeTraits<Type>::one();

  std::vector<Type> data( 100, test_value ); 
  
  hdf5_file_handler.writeArrayToDataSet( data, TEST_DATASET_NAME );

  // If the dataset attribute is not written successfuly, an exception will be 
  // thrown and caught inside of the member function, causing a program exit.
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.writeValueToDataSetAttribute( test_value,
						  TEST_DATASET_NAME,
						  TEST_ATTRIBUTE_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can read a single value from a dataset
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( readValueFromDataSetAttribute, 
			       Type,
			       native_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  Type test_value_original = TPOR::Traits::HDF5TypeTraits<Type>::one();
  
  std::vector<Type> data( 100, test_value_original ); 

  hdf5_file_handler.writeArrayToDataSet( data, TEST_DATASET_NAME );

  hdf5_file_handler.writeValueToDataSetAttribute( test_value_original,
						  TEST_DATASET_NAME,
						  TEST_ATTRIBUTE_NAME );
  
  Type test_value;
  hdf5_file_handler.readValueFromDataSetAttribute( test_value,
						   TEST_DATASET_NAME,
						   TEST_ATTRIBUTE_NAME );

  BOOST_CHECK_EQUAL( test_value_original, test_value );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can write an Array of Type to a group
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( writeArrayToGroupAttribute,
			       Array,
			       array_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  typedef typename TPOR::Traits::ArrayTraits<Array>::value_type value_type;

  value_type test_value = TPOR::Traits::HDF5TypeTraits<value_type>::one();

  Array data( 100, test_value );
  
  // If the group attribute is not written successfuly, an exception will be 
  // thrown and caught inside of the member function, causing a program exit.
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.writeArrayToGroupAttribute( data,
						CHILD_GROUP,
						TEST_ATTRIBUTE_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can read an Array of Type from a group
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( readArrayFromGroupAttribute,
			       Array,
			       array_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  typedef typename TPOR::Traits::ArrayTraits<Array>::value_type value_type;

  value_type test_value = TPOR::Traits::HDF5TypeTraits<value_type>::one();
 
  Array original_data( 3, test_value );
    
  hdf5_file_handler.writeArrayToGroupAttribute( original_data,
						CHILD_GROUP,
						TEST_ATTRIBUTE_NAME );

  Array copied_data;
  hdf5_file_handler.readArrayFromGroupAttribute( copied_data,
						 CHILD_GROUP,
						 TEST_ATTRIBUTE_NAME );
  
  BOOST_CHECK_EQUAL( original_data[0], copied_data[0] );
  BOOST_CHECK_EQUAL( original_data[1], copied_data[1] );
  BOOST_CHECK_EQUAL( original_data[2], copied_data[2] );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can write a single value to a group
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( writeValueToGroupAttribute, 
			       Type,
			       native_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  Type test_value = TPOR::Traits::HDF5TypeTraits<Type>::one();
 
  // If the group attribute is not written successfuly, an exception will be 
  // thrown and caught inside of the member function, causing a program exit.
  // Therefore, no TEST macro is needed.
  hdf5_file_handler.writeValueToGroupAttribute( test_value,
						ROOT_GROUP,
						TEST_ATTRIBUTE_NAME );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// Check that the HDF5FileHandler can read a single value from a group
// attribute in an HDF5 file
BOOST_AUTO_TEST_CASE_TEMPLATE( readValueFromGroupAttribute, 
			       Type,
			       native_types )
{
  TPOR::HDF5FileHandler hdf5_file_handler;

  hdf5_file_handler.openHDF5FileAndOverwrite( HDF5_TEST_FILE_NAME );

  Type test_value_original, test_value;
  test_value_original = TPOR::Traits::HDF5TypeTraits<Type>::zero();

  hdf5_file_handler.writeValueToGroupAttribute( test_value_original,
						ROOT_GROUP,
						TEST_ATTRIBUTE_NAME );

  hdf5_file_handler.writeValueToGroupAttribute( test_value_original,
						CHILD_GROUP,
						TEST_ATTRIBUTE_NAME );

  hdf5_file_handler.readValueFromGroupAttribute( test_value,
  						 ROOT_GROUP,
  						 TEST_ATTRIBUTE_NAME );

  BOOST_REQUIRE_EQUAL( test_value_original, test_value );
  
  hdf5_file_handler.readValueFromGroupAttribute( test_value,
						 CHILD_GROUP,
						 TEST_ATTRIBUTE_NAME );

  BOOST_REQUIRE_EQUAL( test_value_original, test_value );

  hdf5_file_handler.closeHDF5File();
}

//---------------------------------------------------------------------------//
// end tstHDF5FileHandler.cpp
//---------------------------------------------------------------------------//
