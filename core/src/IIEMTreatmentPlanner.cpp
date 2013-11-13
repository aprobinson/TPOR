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
#include <math.h>
#include <limits>
#include <set>

// TPOR includes
#include "ContractException.hpp"
#include "BrachytherapyPatientFileHandler.hpp"
#include "IIEMTreatmentPlanner.hpp"

namespace TPOR
{

// Constructor
IIEMTreatmentPlanner::IIEMTreatmentPlanner( 
				     const std::string &patient_hdf5_file_name,
				     const SeedType desired_seed_type,
				     const double prescribed_dose )
  : d_prescribed_dose( prescribed_dose ),
    d_mesh_x_dimension( 0 ),
    d_mesh_y_dimension( 0 ),
    d_mesh_z_dimension( 0 ),
    d_seed_mesh_x_dimension( 0 ),
    d_seed_mesh_y_dimension( 0 ),
    d_seed_mesh_z_dimension( 0 ),
    d_seed_type( desired_seed_type ),
    d_prostate_volume( 0 ),
    d_min_number_of_needles( 0 ),
    d_ciso1_max( 0.0 )
{
  // Create the file handler for the desired patient file
  BrachytherapyPatientFileHandler patient_hdf5_file( patient_hdf5_file_name );

  // Load in the mesh dimensions
  std::vector<unsigned> mesh_dimensions;
  patient_hdf5_file.getOrganMeshDimensions( mesh_dimensions );
  
  d_mesh_x_dimension = mesh_dimensions[0];
  d_mesh_y_dimension = mesh_dimensions[1];
  d_mesh_z_dimension = mesh_dimensions[2];

  mesh_dimensions.clear();

  // Load the seed mesh dimensions
  std::vector<unsigned> seed_mesh_dimensions;
  patient_hdf5_file.getSeedMeshDimensions( seed_mesh_dimensions );

  d_seed_mesh_x_dimension = seed_mesh_dimensions[0];
  d_seed_mesh_y_dimension = seed_mesh_dimensions[1];
  d_seed_mesh_z_dimension = seed_mesh_dimesnions[2];

  seed_mesh_dimension.clear();

  // Load the prostate mask volume
  patient_hdf5_file.getProstateMaskVolume( d_prostate_vol );
  
  // Load in the prostate mask
  patient_hdf5_file.getProstateMask( d_prostate_mask );

  // Load in the urethra mask
  patient_hdf5_file.getUrethraMask( d_urethra_mask );

  // Load in the margin mask
  patient_hdf5_file.getMarginMask( d_margin_mask );

  // Load in the rectum mask
  patient_hdf5_file.getRectumMask( d_rectum_mask );

  // Load in the needle template
  patient_hdf5_file.getNeedleTemplate( d_needle_template );
 
  // Load in the seed dose distribution
  patient_hdf5_file.getSeedData( d_seed_dose_distribution, desired_seed_type );

  // Load in the prostate adjoint data
  std::vector<double> prostate_adjoint_data;
  patient_hdf5_file.getProstateAdjointData( prostate_adjoint_data,
					    desired_seed_type );

  // Load in the urethra adjoint data
  std::vector<double> urethra_adjoint_data;
  patient_hdf5_file.getUrethraAdjointData( urethra_adjoint_data,
					   desired_seed_type );

  // Load in the margin adjoint data
  std::vector<double> margin_adjoint_data;
  patient_hdf5_file.getMarginAdjointData( margin_adjoint_data,
					  desired_seed_type );
  
  // Load in the rectum adjoint data
  std::vector<double> rectum_adjoint_data;
  patient_hdf5_file.getRectumAdjointData( rectum_adjoint,
					  desired_seed_type );

  // Filter potential seed positions to those along the template positions and
  // set the position weight to the adjoint ratio at the position
  double weight = 0;
  unsigned x_index, y_index, index;
  
  for( unsigned slice = 0; slice < d_z_dimension; ++slice )
  {
    for( unsigned needle = 0; needle < d_needle_template.size(); ++needle )
    {
      x_index = d_needle_template[needle].first;
      y_index = d_needle_template[needle].second;
      index = y_index + x_index*d_y_dimension + 
	slice*d_y_dimension*d_x_dimension;
      
      weight = (urethra_adjoint[index] + margin_adjoint[index] + 
		rectum_adjoint[index])/prostate_adjoint[index];
      
      seed_position.push_back( std::make_pair( needle, 
					       SeedPosition( x_index, 
							     y_index, 
							     slice, 
							     weight, 
							     d_seed_type ) ) );
    }
  }

  // Sort the seed positions
  d_seed_positions.sort( compareSeedPositions );

  // Determine the minimum number of needles that will be needed 
  // (Sua's linear fit)
  unsigned d_min_number_of_needles = 
    static_cast<unsigned>( floor( 0.24*(d_prostate_vol*0.1*0.1*0.5) + 11.33 ));

  // Determine the minimum seed isodose constant for the seed selection process
  double d_max_isodose_constant = calculateMinSeedIsodoseConstant();

  // Initialize the dose distribution
  d_dose_distrubition.resize( d_prostate_mask.size() );
  }

// Calculate optimum treatment plan
void IIEMTreatmentPlanner::calculateOptimumTreatmentPlan()
{
  // Make a copy of the seed positions
  std::list<std::pair<unsigned,SeedPosition> > seed_positions_copy = 
    d_seed_positions;

  // Create a set for the needle ids
  std::set<unsigned> chosen_needle_ids;

  // Start the selection process
  // Outer iteration: needle_goal
  // Inner iteration A: isodose_constant
  // Inner iteration B: needle_isodose_constant
  for( unsigned needle_goal = d_min_number_of_needles;
       needle_goal <= 19;
       ++needle_goal )
  {
    std::list<std::pair<unsigned,SeedPosition> > remaining_seed_positions;

    // Conduct the inner iteration A
    conductIsodoseConstantIteration( needle_goal,
				     chosen_needle_ids,
				     remaining_seed_positions );
    
    // Check for a failed inner iteration A
    if( chosen_needle_ids.size() != needle_goal )
      continue;

    // Conduct the inner iteration B
    conductNeedleIsodoseConstantIteration( 1.02,
					   1.08,
					   0.02,
					   chosen_needle_ids,
					   remaining_seed_positions );
    
    // Check for a failed inner iteration B
    if( calculateProstateDoseCoverage() < 0.98 )
      continue;

    // The inner iterations were successful
    else
      break;
  }      

  // Check if the algorithm failed to create a treatment plan
  if( calculateProstateDoseCoverage() < 0.98 )
  {
    std::cout << "Warning: The treatment planning algorithm was not "
	      << "successful." << std::endl;
  }    
}

// Conduct the isodose constant iteration
void IIEMTreatmentPlanner::conductIsodoseConstantIteration( 
       unsigned needle_goal,
       std::set<unsigned> &chosen_needle_ids,
       std::list<std::pair<unsigned,SeedPosition> > &remaining_seed_positions )
{
  for( double isodose_constant = 0.001; 
       isodose_constant < d_max_isodose_constant;
       isodose_constant += 0.001 )
  {
    // Set the current dose for each mesh element to zero
    std::fill(d_dose_distribution.begin(),
	      d_dose_distribution.end(),
	      0.0 );

    // Reset the chosen needle id set
    chosen_needle_ids.clear();

    // Reset the treatment plan
    d_treatment_plan.clear();

    // Copy the stored seed positions
    remaining_seed_positions = d_seed_positions;
    
    // Select the first seed
    d_treatment_plan.push_back( remaining_seed_positions.front() );
    remaining_seed_positions.pop_front();

    // Add the needle index to the chosen needle set
    chosen_needle_ids.insert( d_treatment_plan.front().first );
    
    // Map the seed dose distribution to the problem mesh
    mapSeedDoseDistribution( d_treatment_plan.front().second.getXIndex(),
			     d_treatment_plan.front().second.getYIndex(),
			     d_treatment_plan.front().second.getZIndex() );
    
    // Keep selecting seeds until the needle goal has been reached
    while( chosen_needle_ids.size() < needle_goal )
    {
      // Select the next acceptable seed position
      std::list<std::pair<unsigned,SeedPosition> >::iterator seed_position;
      
      for( seed_position = remaining_seed_positions.begin(); 
	   seed_position != remaining_seed_positions.end();
	   ++seed_position )
      {
	unsigned index = seed_position->second.getXIndex() +
	  seed_position->second.getYIndex()*d_mesh_x_dimension +
	  seed_position->second.getZIndex()*d_mesh_x_dimension*
	  d_mesh_y_dimension;
	
	double dose_cutoff = 
	  d_prescribed_dose*isodose_constant*chosen_needle_ids.size();
	
	if( d_dose_distrubition[index] < dose_cutoff )
	{
	  d_treatment_plan.push_back( *seed_position );
	  
	  // Check if a new needle has been chosen
	  if( chosen_needle_ids.find( seed_position->first ) ==
	      chosen_needle_ids.end() )
	    chosen_needle_ids.insert( seed_position->first );
	  
	  seed_position = remaining_seed_positions.erase( seed_position );
	  
	  break;
	}
      }
      
      // If no acceptable seed position was found, exit this inner iteration
      if( seed_position == remaining_seed_positions.end() )
	break;
      
      // If an acceptable seed position was found, map the seed's dose
      else
      {
	mapSeedDoseDistribution( d_treatment_plan.back().second.getXIndex(),
				 d_treatment_plan.back().second.getYIndex(),
				 d_treatment_plan.back().second.getZIndex());
      }
    }
    
    // Check if the inner iteration was successful
    if( chosen_needle_ids.size() == needle_goal )
      break;
  }
}

// Conduct the needle isodose constant iteration
double IIEMTreatmentPlanner::conductNeedleIsodoseConstantIteration( 
			     const double start_constant,
			     const double end_constant,
			     const double step,
			     const std::set<unsigned> &chosen_needle_ids,
                             const std::list<std::pair<unsigned,SeedPosition> >
			     &remaining_seed_positions )
{
  // Make sure the start constant is valid
  testPrecondition( start_constant > 0.0 );
  // Make sure the start constant is less than the end constant
  testPrecondition( start_constant < end_constant );
  // Make sure the step is less than the distance between the start/end const
  testPrecondition( step <= end_constant - start_constant );

  // Store a copy of the treatment plan as it currently is
  std::list<std::pair<unsigned,SeedPosition> > treatment_plan_copy = 
    d_treatment_plan;

  // Store a copy of the remaining seed positions
  std::list<std::pair<unsigned,SeedPosition> > remaining_seed_positions_copy =
    remaining_seed_positions;

  // Store the optimum needle isodose constant
  optimum_needle_isodose_constant = 0.0;
  
  for( double needle_isodose_constant = start_constant;
       needle_isodose_constant <= end_constant;
       needle_isodose_constant += step )
  {
    // Reset the treatment plan
    d_treatment_plan = treatment_plan_copy;

    // Reset the remaining seed positions
    remaining_seed_positions_copy = remaining_seed_positions;
    
    // Select seeds along current needles until 98% of prostate receives 
    // prescribed dose
    while( calculateProstateDoseCoverage() < 0.98 )
    {
      // Select the next acceptable seed position
      std::list<std::pair<unsigned,SeedPosition> >::iterator seed_position;
      
      for( seed_position = remaining_seed_positions_copy.begin(); 
	   seed_position != remaining_seed_positions_copy.end();
	   ++seed_position )
      {
	unsigned index = seed_position->second.getXIndex() +
	  seed_position->second.getYIndex()*d_mesh_x_dimension +
	  seed_position->second.getZIndex()*d_mesh_x_dimension*
	  d_mesh_y_dimension;
	
	double dose_cutoff = d_prescribed_dose*needle_isodose_constant;
	
	if( d_dose_distrubition[index] < dose_cutoff &&
	    chosen_needle_ids.find( seed_position->first ) != 
	    chosen_needle_ids.end() )
	{
	  d_treatment_plan.push_back( *seed_position );
	  seed_position = remaining_seed_positions_copy.erase( seed_position );
	  
	  break;
	}
      }
      
      // If no acceptable seed position was found, exit this inner iteration
      if( seed_position == remaining_seed_positions_copy.end() )
	break;
      
      // If an acceptable seed position was found, map the seed's dose
      else
      {
	mapSeedDoseDistribution( d_treatment_plan.back().second.getXIndex(),
				 d_treatment_plan.back().second.getYIndex(),
				 d_treatment_plan.back().second.getZIndex());
      }
    }

    // Check if the inner iteration was successful
    if( calculateProstateDoseCoverage() >= 0.98 )
      optimum_needle_isodose_constant = needle_isodose_constant;
      break;
  }

  // Run a refined iteration
  if( step > 0.001 )
  {
    d_treatment_plan = treatment_plan_copy;
    
    start_needle_isodose_constant = optimum_needle_isodose_constant-0.019;
    conductNeedleIsodoseConstantIteration( start_needle_isodose_constant,
					   optimum_needle_isodose_constant,
					   0.001,
					   chosen_needle_ids,
					   remaining_seed_positions );
  }					   
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end IIEMTreatmentPlanner.cpp
//---------------------------------------------------------------------------//
