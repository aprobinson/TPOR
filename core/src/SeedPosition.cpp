//---------------------------------------------------------------------------//
//!
//! \file   SeedPosition.cpp
//! \author Alex Robinson
//! \brief  Seed position class definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// TPOR Includes
#include "SeedPosition.hpp"
#include "ContractException.hpp"

namespace TPOR
{

// Isotropic seed constructor
SeedPosition::SeedPosition( unsigned x_index,
			    unsigned y_index,
			    unsigned z_index,
			    double weight,
			    BrachytherapySeedType seed_type )
  : d_x_index( x_index ),
    d_y_index( y_index ),
    d_z_index( z_index ),
    d_angle_index( -1 ),
    d_weight( weight ),
    d_seed_type( seed_type)
  
{ 
  // Make sure that the weight is valid
  testPrecondition( weight > 0.0 );
  testPrecondition( weight != std::numeric_limits<double>::infinity() );
}

// General seed constructor
SeedPosition::SeedPosition( unsigned x_index, 
			    unsigned y_index,
			    unsigned z_index,
			    int angle_index,
			    double weight,
			    BrachytherapySeedType seed_type )
{
  // Make sure that the weight is valid
  testPrecondition( weight > 0.0 );
  testPrecondition( weight != std::numeric_limits<double>::infinity() );
  // Make sure that the angle index is valid
  testPrecondition( angle_index >= 0 );
}

// Return the x index
unsigned SeedPosition::getXIndex() const
{
  return d_x_index;
}

// Return the y index
unsigned SeedPosition::getYIndex() const
{
  return d_y_index;
}

// Return the z index
unsigned SeedPosition::getZIndex() const
{
  return d_z_index;
}

// Return the weight of the seed position
double SeedPosition::getWeight() const
{
  return d_weight;
}

// Return the seed type
BrachytherapySeedType SeedPosition::getSeedType() const
{
  return d_seed_type;
}

// Less-than operator for SeedPositions
bool operator < ( const SeedPosition &position_a,
		  const SeedPosition &position_b )
{
  return position_a.d_weight < position_b.d_weight;
}

// Greater-than operator for SeedPositions
bool operator > ( const SeedPosition &position_a,
		  const SeedPosition &position_b )
{
  return position_a.d_weight > position_b.d_weight;
}

// Less-than operator for SeedPosition Pairs
bool operator < ( const std::pair<unsigned,SeedPosition> &position_a,
		  const std::pair<unsigned,SeedPosition> &position_b )
{
  return position_a.second < position_b.second;
}
  
// Greater-than operator for SeedPosition Pairs
bool operator > ( const std::pair<unsigned,SeedPosition> &position_a,
		  const std::pair<unsigned,SeedPosition> &position_b )
{
  return position_a.second > position_b.second;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end SeedPosition.cpp
//---------------------------------------------------------------------------//
