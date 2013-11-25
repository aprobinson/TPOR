//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedPosition.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed position class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_POSITION_HPP
#define BRACHYTHERAPY_SEED_POSITION_HPP

// Std Lib Includes
#include <utility>
#include <vector>
#include <string>

// Boost Includes
#include <boost/shared_ptr.hpp>

// TPOR Includes
#include "BrachytherapySeedProxy.hpp"

namespace TPOR
{

//! Class that stores a brachytherapy seed, position indices, and weight
class BrachytherapySeedPosition
{

public:
  
  //! Constructor
  BrachytherapySeedPosition( 
		       const int x_index, 
		       const int y_index,
		       const int z_index,
		       const double weight,
		       const boost::shared_ptr<BrachytherapySeedProxy> &seed );

  //! Destructor
  virtual ~BrachytherapySeedPosition()
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

  //! Return the seed name
  std::string getSeedName() const;

  //! Map the dose from the seed at this position
  void mapSeedDoseDistribution( std::vector<double> &dose_mesh,
				const unsigned mesh_x_dimension,
				const unsigned mesh_y_dimension,
				const unsigned mesh_z_dimension ) const;

  //! Set the position x dimension (mesh element x dimension)
  static void setXDimension( const double x_dimension );

  //! Set the position y dimension (mesh element y dimension)
  static void setYDimension( const double y_dimension );

  //! Set the position z dimension (mesh element z dimension)
  static void setZDimension( const double z_dimension );

  //! Comparison methods
  friend bool operator < ( const BrachytherapySeedPosition &position_a,
			   const BrachytherapySeedPosition &position_b );

  friend bool operator > ( const BrachytherapySeedPosition &position_a,
			   const BrachytherapySeedPosition &position_b );

private:
  
  // Position dimensions (cm)
  static double x_dimension;
  static double y_dimension;
  static double z_dimension;

  // Position indices
  int d_x_index;
  int d_y_index;
  int d_z_index;

  // Position weight
  double d_weight;

  // Brachytherapy seed
  boost::shared_ptr<BrachytherapySeedProxy> d_seed;
};

//! Less-than operator for BrachytherapySeedPositions
bool operator < ( const BrachytherapySeedPosition &position_a,
		  const BrachytherapySeedPosition &position_b );

//! Greater-than operator for BrachytherapySeedPositions
bool operator > ( const BrachytherapySeedPosition &position_a,
		  const BrachytherapySeedPosition &position_b );

//! Less-than operator for BrachytherapySeedPosition Pairs
bool operator < ( 
	     const std::pair<unsigned,BrachytherapySeedPosition> &position_a,
	     const std::pair<unsigned,BrachytherapySeedPosition> &position_b );
  
//! Greater-than operator for BrachytherapySeedPosition Pairs
bool operator > ( 
	     const std::pair<unsigned,BrachytherapySeedPosition> &position_a,
	     const std::pair<unsigned,BrachytherapySeedPosition> &position_b );

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_POSITION_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedPosition.hpp
//---------------------------------------------------------------------------//

