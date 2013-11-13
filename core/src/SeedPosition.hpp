//---------------------------------------------------------------------------//
//!
//! \file   SeedPosition.hpp
//! \author Alex Robinson
//! \brief  Seed position class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef SEED_POSITION_HPP
#define SEED_POSITION_HPP

// TPOR Includes
#include "SeedType.hpp"

namespace TPOR
{

//! Class that stores a seed position's indices and weight
class SeedPosition
{

public:
  
  //! Isotropic seed constructor
  SeedPosition( unsigned x_index, 
		unsigned y_index,
		unsigned z_index,
		double weight,
		SeedType seed_type );

  //! General seed constructor
  SeedPosition( unsigned x_index, 
		unsigned y_index,
		unsigned z_index,
		int angle_index,
		double weight,
		SeedType seed_type );

  //! Destructor
  virtual ~SeedPosition()
  { /* ... */ }

  //! Return the x index
  unsigned getXIndex();

  //! Return the y index
  unsigned getYIndex();
  
  //! Return the z index
  unsigned getZIndex();

  //! Return the weight of the seed position
  double getWeight();

  //! Return the seed type
  SeedType getSeedType();

  //! Comparison methods
  friend bool operator < ( const SeedPosition &position_a,
			   const SeedPosition &position_b );

  friend bool operator > ( const SeedPosition &position_a,
			   const SeedPosition &position_b );

private:

  // SeedPosition indices
  unsigned d_x_index;
  unsigned d_y_index;
  unsigned d_z_index;
  
  // SeedPosition angle index (for directional seeds)
  int d_angle_index;

  // SeedPosition weight
  double d_weight;

  // Seed type
  SeedType d_seed_type;
};

//! Less-than operator for SeedPositions
bool operator < ( const SeedPosition &position_a,
		  const SeedPosition &position_b );

//! Greater-than operator for SeedPositions
bool operator > ( const SeedPosition &position_a,
		  const SeedPosition &position_b );

} // end TPOR namespace

#endif // end SEED_POSITION_HPP

//---------------------------------------------------------------------------//
// end SeedPosition.hpp
//---------------------------------------------------------------------------//

