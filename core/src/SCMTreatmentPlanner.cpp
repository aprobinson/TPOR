//---------------------------------------------------------------------------//
//! 
//! \file   SCMTreatmentPlanner.cpp
//! \author Alex Robinson
//! \brief  Set cover method (SCM) class definition brachytherapy treatment
//!         planner.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <limits>

// Boost Includes
#include <boost/chrono.hpp>

// TPOR Includes
#include "SCMTreatmentPlanner.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Constructor
SCMTreatmentPlanner::SCMTreatmentPlanner(
	 const boost::shared_ptr<BrachytherapyPatient> &patient,
	 const std::vector<boost::shared_ptr<BrachytherapySeedProxy> > &seeds )
  : d_patient( patient ),
    d_opt_time( 0.0 ),
    d_seed_positions()
{
  // Get the potential seed positions
  typedef BrachytherapySetCoverSeedPosition SeedPosition;

  std::list<SeedPosition> seed_positions = 
    d_patient->getPotentialSeedPositions<SeedPosition>( seeds );

  d_seed_positions.splice( d_seed_positions.end(), seed_positions );
}

// Calculate optimum treatment plan
void SCMTreatmentPlanner::calculateOptimumTreatmentPlan()
{
  std::cout << std::endl << "starting treatment plan optimization..."
	    << std::endl;
  
  boost::chrono::steady_clock::time_point start_clock =
    boost::chrono::steady_clock::now();

  std::list<BrachytherapySetCoverSeedPosition>::iterator position;

  // Select the first seed position
  position = min_element( d_seed_positions.begin(), d_seed_positions.end() );
  d_patient->insertSeed( *position );
  position = d_seed_positions.erase( position );
  updateSeedPositions();

  // Select the seed position with the smallest cost/coverage (weight) until
  // 98% of the prostate volume recieves the prescribed dose
  while( d_patient->getProstateDoseCoverage() < 0.98 &&
	 d_seed_positions.size() > 0 )
  {
    position = min_element( d_seed_positions.begin(), d_seed_positions.end() );

    d_patient->insertSeed( *position );
    position = d_seed_positions.erase( position );
    updateSeedPositions();

    // Determine if a needle penalty must be applied
    // while(true)
    // {
    //   // No penalty for using the same needle
    //   if( d_patient->isSeedOnNeedle( *position ) )
    //   {
    // 	d_patient->insertSeed( *position );
    // 	position = d_seed_positions.erase( position );
    // 	updateSeedPositions();
    // 	break;
    //   }
      
    //   // Penalize for choosing a new needle
    //   else
    //   {
    // 	unsigned num_needles = d_patient->getNumInsertedNeedles();
    // 	double needle_penalty =
    // 	  0.5*sqrt((30.0*30.0+(num_needles+1)*(num_needles+1))/
    // 		   (30.0*30.0-(num_needles+1)*(num_needles+1)));
	
    // 	position->multiplyWeight( needle_penalty );

    // 	std::list<BrachytherapyDynamicWeightSeedPosition>::iterator 
    // 	  test_position = min_element( d_seed_positions.begin(), 
    // 				       d_seed_positions.end() );

    // 	// The needle penalty had no effect
    // 	if( position == test_position )
    // 	{
    // 	  d_patient->insertSeed( *position );
    // 	  position = d_seed_positions.erase( position );
    // 	  updateSeedPositions();
    // 	  break;
    // 	}
	
    // 	// The needle penalty had an effect, test if another penalty must be
    // 	// applied
    // 	else
    // 	{
    // 	  position = test_position;
    // 	}
    //   }
    // }
  }

  boost::chrono::duration<double> seconds =
    boost::chrono::steady_clock::now() - start_clock;
  
  // Store the optimization time
  d_opt_time = seconds.count();

  // Check for a successful treatment plan
  if( d_patient->getProstateDoseCoverage() < 0.98 )
  {
    std::cout << "Warning: The treatment planning algorithm was not "
	      << "successful." << std::endl;
  }
  else
  {
    std::cout << std::endl << "treatment plan optimization complete."
	      << std::endl << std::endl;
  }

  // Print the treatment plan summary
  printTreatmentPlanSummary( std::cout );
}

// Update the seed positions
void SCMTreatmentPlanner::updateSeedPositions()
{
  std::list<BrachytherapySetCoverSeedPosition>::iterator position,
    end_position;

  position = d_seed_positions.begin();
  end_position = d_seed_positions.end();

  while( position != end_position )
  {
    if( d_patient->isSeedPositionFree( *position ) )
      position->updateWeight();
    else
      position = d_seed_positions.erase( position );

    ++position;
  }
}

// Print the treatment plan summary
void SCMTreatmentPlanner::printTreatmentPlanSummary( std::ostream &os ) const
{
  os.precision( 3 );
  os.setf( std::ios::fixed, std::ios::floatfield );
  os << "...Treatment Plan Summary..." << std::endl;
  os << "Plan Optimization Time (s): " << d_opt_time << std::endl;
  os << "Successful Optimization:    " 
     << (d_patient->getProstateDoseCoverage() >= 0.98 ? "Yes" : "No")
     << std::endl;
  os << "Needles Chosen:             " << d_patient->getNumInsertedNeedles()
     << std::endl;
  os << "Seeds Chosen:               " << d_patient->getNumInsertedSeeds()
     << std::endl;
  os << std::endl;
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end SCMTreatmentPlanner.cpp
//---------------------------------------------------------------------------//
