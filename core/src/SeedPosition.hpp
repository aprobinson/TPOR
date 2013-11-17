//---------------------------------------------------------------------------//
//!
//! \file   SeedPosition.hpp
//! \author Alex Robinson
//! \brief  Seed position class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef SEED_POSITION_HPP
#define SEED_POSITION_HPP

// Std Lib Includes
#include <utility>

// TPOR Includes
#include "BrachytherapySeedType.hpp"

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
		BrachytherapySeedType seed_type );

  //! General seed constructor
  SeedPosition( unsigned x_index, 
		unsigned y_index,
		unsigned z_index,
		int angle_index,
		double weight,
		BrachytherapySeedType seed_type );

  //! Destructor
  virtual ~SeedPosition()
  { /* ... */ }

  //! Return the x index
  unsigned getXIndex() const;

  //! Return the y index
  unsigned getYIndex() const;
  
  //! Return the z index
  unsigned getZIndex() const;

  //! Return the weight of the seed position
  double getWeight() const;

  //! Return the seed type
  BrachytherapySeedType getSeedType() const;

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
  BrachytherapySeedType d_seed_type;
};

//! Less-than operator for SeedPositions
bool operator < ( const SeedPosition &position_a,
		  const SeedPosition &position_b );

//! Greater-than operator for SeedPositions
bool operator > ( const SeedPosition &position_a,
		  const SeedPosition &position_b );

//! Less-than operator for SeedPosition Pairs
bool operator < ( const std::pair<unsigned,SeedPosition> &position_a,
		  const std::pair<unsigned,SeedPosition> &position_b );
  
//! Greater-than operator for SeedPosition Pairs
bool operator > ( const std::pair<unsigned,SeedPosition> &position_a,
		  const std::pair<unsigned,SeedPosition> &position_b );

} // end TPOR namespace

#endif // end SEED_POSITION_HPP

//---------------------------------------------------------------------------//
// end SeedPosition.hpp
//---------------------------------------------------------------------------//

