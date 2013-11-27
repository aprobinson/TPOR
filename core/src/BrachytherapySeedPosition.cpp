//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedPosition.cpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position class definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// TPOR Includes
#include "BrachytherapySeedPosition.hpp"
#include "ContractException.hpp"

namespace TPOR
{

// Initialize the position dimensions
double BrachytherapySeedPosition::x_dimension = 0.1;
double BrachytherapySeedPosition::y_dimension = 0.1;
double BrachytherapySeedPosition::z_dimension = 0.5;

// Constructor
BrachytherapySeedPosition::BrachytherapySeedPosition( 
		        const int x_index,
			const int y_index,
			const int z_index,
			const double weight,
		        const boost::shared_ptr<BrachytherapySeedProxy> &seed )
  : d_x_index( x_index ),
    d_y_index( y_index ),
    d_z_index( z_index ),
    d_weight( weight ),
    d_seed( seed )
{ 
  // Make sure that the weight is valid
  testPrecondition( weight == weight ); // Nan test
  testPrecondition( weight > 0.0 );
  testPrecondition( weight != std::numeric_limits<double>::infinity() );
  // Make sure that the seed is valid
  testPrecondition( seed );
}

// Return the x index
unsigned BrachytherapySeedPosition::getXIndex() const
{
  return d_x_index;
}

// Return the y index
unsigned BrachytherapySeedPosition::getYIndex() const
{
  return d_y_index;
}

// Return the z index
unsigned BrachytherapySeedPosition::getZIndex() const
{
  return d_z_index;
}

// Return the weight of the seed position
double BrachytherapySeedPosition::getWeight() const
{
  return d_weight;
}

// Return the seed type
BrachytherapySeedType BrachytherapySeedPosition::getSeedType() const
{
  return d_seed->getSeedType();
}

// Return the seed name
std::string BrachytherapySeedPosition::getSeedName() const
{
  return d_seed->getSeedName();
}

// Set the position x dimension (mesh element x dimension)
void BrachytherapySeedPosition::setXDimension( const double x_dimension )
{
  BrachytherapySeedPosition::x_dimension = x_dimension;
}

// Set the position y dimension (mesh element y dimension)
void BrachytherapySeedPosition::setYDimension( const double y_dimension )
{
  BrachytherapySeedPosition::y_dimension = y_dimension;
}

// Set the position z dimension (mesh element z dimension)
void BrachytherapySeedPosition::setZDimension( const double z_dimension )
{
  BrachytherapySeedPosition::z_dimension = z_dimension;
}

// Less-than operator for BrachytherapySeedPositions
bool operator < ( const BrachytherapySeedPosition &position_a,
		  const BrachytherapySeedPosition &position_b )
{
  return position_a.d_weight < position_b.d_weight;
}

// Greater-than operator for BrachytherapySeedPositions
bool operator > ( const BrachytherapySeedPosition &position_a,
		  const BrachytherapySeedPosition &position_b )
{
  return position_a.d_weight > position_b.d_weight;
}

// Less-than operator for BrachytherapySeedPosition Pairs
bool operator < ( 
	      const std::pair<unsigned,BrachytherapySeedPosition> &position_a,
	      const std::pair<unsigned,BrachytherapySeedPosition> &position_b )
{
  return position_a.second < position_b.second;
}
  
// Greater-than operator for BrachytherapySeedPosition Pairs
bool operator > ( 
	      const std::pair<unsigned,BrachytherapySeedPosition> &position_a,
	      const std::pair<unsigned,BrachytherapySeedPosition> &position_b )
{
  return position_a.second > position_b.second;
}

// Equality operator for BrachytherapySeedPositions
bool operator == ( const BrachytherapySeedPosition &position_a,
		   const BrachytherapySeedPosition &position_b )
{
  return position_a.d_x_index == position_b.d_x_index &&
    position_a.d_y_index == position_b.d_y_index &&
    position_a.d_z_index == position_b.d_z_index;
}

// Inequality operator for BrachytherapySeedPositions
bool operator != ( const BrachytherapySeedPosition &position_a,
		   const BrachytherapySeedPosition &position_b )
{
  return position_a.d_x_index != position_b.d_x_index ||
    position_a.d_y_index != position_b.d_y_index ||
    position_a.d_z_index != position_b.d_z_index;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapySeedPosition.cpp
//---------------------------------------------------------------------------//
