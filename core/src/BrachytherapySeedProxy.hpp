//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedProxy.hpp
//! \author Alex Robinson
//! \brief  Brachytherapy seed proxy class declaration
//!
//---------------------------------------------------------------------------//

#ifndef BRACHYTHERAPY_SEED_PROXY_HPP
#define BRACHYTHERAPY_SEED_PROXY_HPP

// Std Lib Includes
#include <vector>
#include <string>
#include <stdlib.h>

// TPOR Includes
#include "BrachytherapySeed.hpp"
#include "ContractException.hpp"

namespace TPOR{

//! Brachytherapy seed proxy class
class BrachytherapySeedProxy 
{

public:

  //! Constructor
  BrachytherapySeedProxy( const std::string seed_file_name,
			  const BrachytherapySeedType seed_type,
			  const double air_kerma_strength );

  //! Return the seed type
  BrachytherapySeedType getSeedType() const;

  //! Return the seed name
  std::string getSeedName() const;
  
  //! Return the seed strength (air-kerma strength)
  double getSeedStrength() const;

  //! Return the total dose at time = infinity at a given point (cGy)
  double getTotalDose( const int x,
		       const int y,
		       const int z ) const;

private:

  // The seed dose distribution mesh
  std::vector<double> d_dose_distribution_mesh;

  // The seed dose distribution mesh dimensions
  unsigned d_mesh_x_dim;
  unsigned d_mesh_y_dim;
  unsigned d_mesh_z_dim;

  // The seed center indices
  int d_seed_x_index;
  int d_seed_y_index;
  int d_seed_z_index;

  // The seed type
  BrachytherapySeedType d_seed_type;

  // The seed name
  std::string d_seed_name;
  
  // The seed strength
  double d_air_kerma_strength;
};

//---------------------------------------------------------------------------//
// Inline definitions.
//---------------------------------------------------------------------------//
// Return the total dose at time = infinity at a given point (cGy)
inline double BrachytherapySeedProxy::getTotalDose( const int x,
						    const int y,
						    const int z ) const
{
  // Make sure the x, y, and z indices are in range
  testPrecondition( abs(x) < d_mesh_x_dim/2 );
  testPrecondition( abs(y) < d_mesh_y_dim/2 );
  testPrecondition( abs(z) < d_mesh_z_dim/2 );
  
  return d_dose_distribution_mesh[(d_seed_x_index+x)+
				  (d_seed_y_index+y)*d_mesh_x_dim+
				  (d_seed_z_index+z)*d_mesh_x_dim*d_mesh_y_dim];
}

} // end TPOR namespace

#endif // end BRACHYTHERAPY_SEED_PROXY_HPP

//---------------------------------------------------------------------------//
// end BrachytherapySeedProxy.hpp
//---------------------------------------------------------------------------//
