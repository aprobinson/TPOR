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
			    SeedType seed_type )
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
			    SeedType seed_type )
{
  // Make sure that the weight is valid
  testPrecondition( weight > 0.0 );
  testPrecondition( weight != std::numeric_limits<double>::infinity() );
  // Make sure that the angle index is valid
  testPrecondition( angle_index >= 0 );
}

// Return the x index
unsigned SeedPosition::getXIndex()
{
  return d_indices[0];
}

// Return the y index
unsigned SeedPosition::getYIndex()
{
  return d_indices[1];
}

// Return the z index
unsigned SeedPosition::getZIndex()
{
  return d_indices[2];
}

// Return the weight of the seed position
double SeedPosition::getWeight()
{
  return d_weight;
}

// Return the seed type
SeedType SeedPosition::getSeedType()
{
  return d_seed_type;
}

// Less-than operator for SeedPositions
bool operator < ( const SeedPosition &position_a,
		  const SeedPosition &position_b )
{
  return position_a.weight < position_b.weight;
}

// Greater-than operator for SeedPositions
bool operator > ( const SeedPosition &position_a,
		  const SeedPosition &position_b )
{
  return position_a.weight > position_b.weight;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end SeedPosition.cpp
//---------------------------------------------------------------------------//
