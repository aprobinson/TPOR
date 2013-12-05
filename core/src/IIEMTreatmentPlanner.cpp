//---------------------------------------------------------------------------//
//!
//! \file   IIEMTreatmentPlanner.cpp
//! \author Alex Robinson
//! \brief  Iterative isodose exclusion method (IIEM) class definition for
//!         brachytherapy treatment planning based on Sua Yoo's algorithm.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <limits>
#include <set>

// Boost Includes
#include <boost/chrono.hpp>

// TPOR includes
#include "IIEMTreatmentPlanner.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Initialize the name static member
const std::string IIEMTreatmentPlanner::name = "IIEMTreatmentPlanner";

// Constructor
IIEMTreatmentPlanner::IIEMTreatmentPlanner( 
			const boost::shared_ptr<BrachytherapyPatient> &patient,
			const boost::shared_ptr<BrachytherapySeedProxy> &seed )
		     
  : d_patient( patient ),
    d_min_number_of_needles( 0 ),
    d_min_isodose_constant( 0.0 ),
    d_opt_time( 0.0 ),
    d_seed_positions()
{
  // Get the potential seed positions
  d_seed_positions = 
    d_patient->getPotentialSeedPositions<BrachytherapySeedPosition>( seed );
  
  // Sort the seed positions
  d_seed_positions.sort();

  // Determine the minimum number of needles that will be needed 
  // (Sua's linear fit)  
  d_min_number_of_needles = 
    static_cast<unsigned>(floor(0.24*d_patient->getProstateVolume() + 11.33));
  
  // Determine the minimum seed isodose constant for the seed selection process
  d_min_isodose_constant = calculateMinSeedIsodoseConstant();
}

// Calculate optimum treatment plan
void IIEMTreatmentPlanner::calculateOptimumTreatmentPlan()
{
  std::cout << std::endl << "starting treatment plan optimization..." 
	    << std::endl;
  boost::chrono::steady_clock::time_point start_clock = 
    boost::chrono::steady_clock::now();
  
  // Start the selection process
  // Outer iteration: needle_goal
  // Inner iteration A: isodose_constant
  // Inner iteration B: needle_isodose_constant
  for( unsigned needle_goal = d_min_number_of_needles; 
       needle_goal <= 30;
       ++needle_goal )
  {
    // Conduct the inner iteration A
    std::cout << "conducting isodose constant iteration for needle goal of "
	      << needle_goal << "..." << std::endl;
    
    conductIsodoseConstantIteration( needle_goal );
    
    // Check for a failed inner iteration A
    if( d_patient->getProstateDoseCoverage() < 0.98 )
      continue;

    // The inner iterations were successful
    else
      break;
  }   
  
  boost::chrono::duration<double> seconds =
    boost::chrono::steady_clock::now() - start_clock;
  
  // Store the optimization time
  d_opt_time = seconds.count();

  // Check if the algorithm failed to create a treatment plan
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

// Conduct the isodose constant iteration
void IIEMTreatmentPlanner::conductIsodoseConstantIteration( 
						   const unsigned needle_goal )
	       
{
  for( double isodose_constant = 0.001; 
       isodose_constant <= d_min_isodose_constant;
       isodose_constant += 0.001 )
  {
    // Copy the stored seed positions
    std::list<BrachytherapySeedPosition> remaining_seed_positions = 
      d_seed_positions;

    // Reset the patient state
    d_patient->resetState();
    
    // Select the first seed
    d_patient->insertSeed( remaining_seed_positions.front() );
    remaining_seed_positions.pop_front();
    
    // Keep selecting seeds until the needle goal has been reached
    while( d_patient->getNumInsertedNeedles() < needle_goal )
    {
      // Select the next acceptable seed position
      std::list<BrachytherapySeedPosition>::iterator seed_position;
      
      for( seed_position = remaining_seed_positions.begin(); 
	   seed_position != remaining_seed_positions.end();
	   ++seed_position )
      {	
	double dose_cutoff = d_patient->getPrescribedDose()*isodose_constant*
	  d_patient->getNumInsertedSeeds();
	  	
	if( d_patient->getDose( *seed_position ) < dose_cutoff )
	{
	  d_patient->insertSeed( *seed_position );
	  seed_position = remaining_seed_positions.erase( seed_position );
	  
	  break;
	}
      }
      
      // If no acceptable seed position was found, exit this inner iteration
      if( seed_position == remaining_seed_positions.end() )
	break;
      
    }
    
    // Check for a failed iteration
    if( d_patient->getNumInsertedNeedles() != needle_goal )
      continue;

    // Store a copy of the treatment plan for running refined inner iteration B
    d_patient->saveState();
        
    // Conduct the inner iteration B
    std::cout << "conducting needle isodose constant iteration..."
	      << std::endl;
    double needle_isodose_constant = 
      conductNeedleIsodoseConstantIteration( 1.02,
					     1.08,
					     0.02,
					     remaining_seed_positions );

    // Check for a successful inner iteration B
    if( d_patient->getProstateDoseCoverage() >= 0.98 )
    {
      // Run a refined inner iteration B
      d_patient->loadSavedState();
            
      needle_isodose_constant = 
	conductNeedleIsodoseConstantIteration( needle_isodose_constant-0.019,
					       needle_isodose_constant,
					       0.001,
					       remaining_seed_positions );
      break;
    }
  }
}

// Conduct the needle isodose constant iteration
double IIEMTreatmentPlanner::conductNeedleIsodoseConstantIteration( 
	 const double start_constant,
	 const double end_constant,
	 const double step,
	 const std::list<BrachytherapySeedPosition> &remaining_seed_positions )
{
  // Make sure the start constant is valid
  testPrecondition( start_constant > 0.0 );
  // Make sure the start constant is less than the end constant
  testPrecondition( start_constant < end_constant );
  // Make sure the step is less than the distance between the start/end const
  testPrecondition( step <= end_constant - start_constant );
  
  // Save the patient state
  d_patient->saveState();

  // Store a copy of the remaining seed positions
  std::list<BrachytherapySeedPosition> remaining_seed_positions_copy = 
    remaining_seed_positions;

  // Store the optimum needle isodose constant
  double optimum_needle_isodose_constant = 0.0;
  
  for( double needle_isodose_constant = start_constant;
       needle_isodose_constant <= end_constant;
       needle_isodose_constant += step )
  {    
    // Select seeds along current needles until 98% of prostate receives 
    // prescribed dose
    while( d_patient->getProstateDoseCoverage() < 0.98 )
    {
      // Select the next acceptable seed position
      std::list<BrachytherapySeedPosition>::iterator seed_position;
      
      for( seed_position = remaining_seed_positions_copy.begin(); 
	   seed_position != remaining_seed_positions_copy.end();
	   ++seed_position )
      {
	double dose_cutoff = 
	  d_patient->getPrescribedDose()*needle_isodose_constant;
	
	if( d_patient->getDose( *seed_position ) < dose_cutoff &&
	    d_patient->isSeedOnNeedle( *seed_position ) )
	{
	  d_patient->insertSeed( *seed_position );
	  seed_position = remaining_seed_positions_copy.erase( seed_position );
	  
	  break;
	}
      }
      
      // If no acceptable seed position was found, exit this inner iteration
      if( seed_position == remaining_seed_positions_copy.end() )
	break;
    }

    // Check if the inner iteration was successful
    if( d_patient->getProstateDoseCoverage() >= 0.98 )
    {
      optimum_needle_isodose_constant = needle_isodose_constant;
      break;
    }
    else
    {
      d_patient->loadSavedState();
            
      // Reset the remaining seed positions
      remaining_seed_positions_copy = remaining_seed_positions;
    }
  }

  return optimum_needle_isodose_constant;
}

// Calculate the minimum seed isodose constant
double IIEMTreatmentPlanner::calculateMinSeedIsodoseConstant() const
{
  double min_isodose_constant = std::numeric_limits<double>::infinity();
  double isodose_constant = 0.0;

  std::list<BrachytherapySeedPosition>::const_iterator 
    position, end_position;
  
  position = d_seed_positions.begin();
  end_position = d_seed_positions.end();

  unsigned mesh_x_dim = d_patient->getOrganMeshXDim();
  unsigned mesh_y_dim = d_patient->getOrganMeshYDim();
  unsigned mesh_z_dim = d_patient->getOrganMeshZDim();
  
  std::vector<double> tmp_dose_distribution( mesh_x_dim*mesh_y_dim*mesh_z_dim);
  
  while( position != end_position )
  {
    position->mapSeedDoseDistribution<Equal>( tmp_dose_distribution,
					      mesh_x_dim,
					      mesh_y_dim,
					      mesh_z_dim );
        
    for( unsigned k = 0; k < mesh_z_dim; ++k )
    {
      for( unsigned j = 0; j < mesh_y_dim; ++j )
      {
	for( unsigned i = 0; i < mesh_x_dim; ++i )
	{
	  if( d_patient->getTissueType( i, j, k ) == PROSTATE_TISSUE )
	  {
	    unsigned index = i + j*mesh_x_dim + k*mesh_x_dim*mesh_y_dim;
	    
	    if( tmp_dose_distribution[index] > d_patient->getPrescribedDose() )
	      isodose_constant += d_patient->getPrescribedDose();
	    else
	      isodose_constant += tmp_dose_distribution[index];
	  }
	}
      }
    }
    
    isodose_constant /= 
      (d_patient->getProstateSize()*d_patient->getPrescribedDose());
      
    if( isodose_constant < min_isodose_constant )
      min_isodose_constant = isodose_constant;

    ++position;
  }

  return min_isodose_constant;
}

// Print the treatment plan summary
void IIEMTreatmentPlanner::printTreatmentPlanSummary( std::ostream &os ) const
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
// end IIEMTreatmentPlanner.cpp
//---------------------------------------------------------------------------//
