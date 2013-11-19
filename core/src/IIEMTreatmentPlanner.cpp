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
#include "BrachytherapySeedFactory.hpp"
#include "AdjointDataGenerator.hpp"
#include "IIEMTreatmentPlanner.hpp"

namespace TPOR
{

// Constructor
IIEMTreatmentPlanner::IIEMTreatmentPlanner( 
				 const std::string &patient_hdf5_file_name,
				 const BrachytherapySeedType desired_seed_type,
				 const double desired_seed_strength,
				 const double prescribed_dose )
  : d_prescribed_dose( prescribed_dose ),
    d_mesh_x_dim( 0 ),
    d_mesh_y_dim( 0 ),
    d_mesh_z_dim( 0 ),
    d_mesh_element_x_dim( 0.0 ),
    d_mesh_element_y_dim( 0.0 ),
    d_mesh_element_z_dim( 0.0 ),
    d_prostate_vol( 0 ),
    d_min_number_of_needles( 0 ),
    d_min_isodose_constant( 0.0 )
{
  // Create the file handler for the desired patient file
  BrachytherapyPatientFileHandler patient_file( patient_hdf5_file_name );

  // Load in the mesh dimensions
  std::vector<unsigned> mesh_dimensions;
  patient_file.getOrganMeshDimensions( mesh_dimensions );
  
  d_mesh_x_dim = mesh_dimensions[0];
  d_mesh_y_dim = mesh_dimensions[1];
  d_mesh_z_dim = mesh_dimensions[2];

  mesh_dimensions.clear();

  // Load the mesh element dimensions
  std::vector<double> mesh_element_dimensions;
  patient_file.getMeshElementDimensions( mesh_element_dimensions );
  d_mesh_element_x_dim = mesh_element_dimensions[0];
  d_mesh_element_y_dim = mesh_element_dimensions[1];
  d_mesh_element_z_dim = mesh_element_dimensions[2];

  mesh_element_dimensions.clear();

  // Load the prostate mask volume
  patient_file.getProstateMaskRelativeVolume( d_prostate_vol );
  
  // Load in the prostate mask
  patient_file.getProstateMask( d_prostate_mask );

  // Load in the urethra mask
  patient_file.getUrethraMask( d_urethra_mask );

  // Load in the margin mask
  patient_file.getMarginMask( d_margin_mask );

  // Load in the rectum mask
  patient_file.getRectumMask( d_rectum_mask );

  // Load in the needle template
  patient_file.getNeedleTemplate( d_needle_template );
 
  // Create the desired brachytherapy seed
  d_seed = BrachytherapySeedFactory::createSeed( desired_seed_type,
						 desired_seed_strength );  

  std::vector<double> prostate_adjoint_data;
  std::vector<double> urethra_adjoint_data;
  std::vector<double> margin_adjoint_data;
  std::vector<double> rectum_adjoint_data;

  // Load the adjoint data for the desired seed if it has been cached  
  if( patient_file.adjointDataExists( desired_seed_type ) )
  {
    // Load in the prostate adjoint data
    patient_file.getProstateAdjointData( prostate_adjoint_data,
					 desired_seed_type,
					 desired_seed_strength );

    // Load in the urethra adjoint data
    patient_file.getUrethraAdjointData( urethra_adjoint_data,
					desired_seed_type,
					desired_seed_strength );

    // Load in the margin adjoint data
    patient_file.getMarginAdjointData( margin_adjoint_data,
				       desired_seed_type,
				       desired_seed_strength );
  
    // Load in the rectum adjoint data
    patient_file.getRectumAdjointData( rectum_adjoint_data,
				       desired_seed_type,
				       desired_seed_strength );
  }

  // Genenerate the adjoint data if it is not in the cache
  else
  {
    AdjointDataGenerator adjoint_gen( d_seed,
				      d_mesh_element_x_dim,
				      d_mesh_element_y_dim,
				      d_mesh_element_z_dim );
    
    adjoint_gen.calculateAdjointDose( prostate_adjoint_data,
				      d_prostate_mask,
				      d_mesh_x_dim,
				      d_mesh_y_dim,
				      d_mesh_z_dim );

    adjoint_gen.calculateAdjointDose( urethra_adjoint_data,
				      d_urethra_mask,
				      d_mesh_x_dim,
				      d_mesh_y_dim,
				      d_mesh_z_dim );

    adjoint_gen.calculateAdjointDose( margin_adjoint_data,
				      d_margin_mask,
				      d_mesh_x_dim,
				      d_mesh_y_dim,
				      d_mesh_z_dim );

    adjoint_gen.calculateAdjointDose( rectum_adjoint_data,
				      d_rectum_mask,
				      d_mesh_x_dim,
				      d_mesh_y_dim,
				      d_mesh_z_dim );

    // Cache this adjoint data
    patient_file.setProstateAdjointData( prostate_adjoint_data,
					 desired_seed_type,
					 desired_seed_strength );

    patient_file.setUrethraAdjointData( urethra_adjoint_data,
					desired_seed_type,
					desired_seed_strength );

    patient_file.setMarginAdjointData( margin_adjoint_data,
				       desired_seed_type,
				       desired_seed_strength );

    patient_file.setRectumAdjointData( rectum_adjoint_data,
				       desired_seed_type,
				       desired_seed_strength );
  }

  // Create the seed position list
  createSeedPositionList( prostate_adjoint_data,
			  urethra_adjoint_data,
			  margin_adjoint_data,
			  rectum_adjoint_data );

  // Sort the seed positions
  d_seed_positions.sort();

  // Determine the minimum number of needles that will be needed 
  // (Sua's linear fit)
  double true_prostate_vol = d_prostate_vol*d_mesh_element_x_dim*
    d_mesh_element_y_dim*d_mesh_element_z_dim;
  unsigned d_min_number_of_needles = 
    (unsigned)floor( 0.24*true_prostate_vol + 11.33 );

  // Determine the minimum seed isodose constant for the seed selection process
  d_min_isodose_constant = calculateMinSeedIsodoseConstant();

  // Initialize the dose distribution
  d_dose_distribution.resize( d_prostate_mask.size() );
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
  
  // If successful, generate the dose-volume-histogram data
  else
  {
    calculateDoseVolumeHistogramData();
  }
}

// Conduct the isodose constant iteration
void IIEMTreatmentPlanner::conductIsodoseConstantIteration( 
       unsigned needle_goal,
       std::set<unsigned> &chosen_needle_ids,
       std::list<std::pair<unsigned,SeedPosition> > &remaining_seed_positions )
{
  for( double isodose_constant = 0.001; 
       isodose_constant < d_min_isodose_constant;
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
	  seed_position->second.getYIndex()*d_mesh_x_dim +
	  seed_position->second.getZIndex()*d_mesh_x_dim*
	  d_mesh_y_dim;
	
	double dose_cutoff = 
	  d_prescribed_dose*isodose_constant*chosen_needle_ids.size();
	
	if( d_dose_distribution[index] < dose_cutoff )
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
void IIEMTreatmentPlanner::conductNeedleIsodoseConstantIteration( 
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
  double optimum_needle_isodose_constant = 0.0;
  
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
	  seed_position->second.getYIndex()*d_mesh_x_dim +
	  seed_position->second.getZIndex()*d_mesh_x_dim*d_mesh_y_dim;
	
	double dose_cutoff = d_prescribed_dose*needle_isodose_constant;
	
	if( d_dose_distribution[index] < dose_cutoff &&
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

  // Run a refined iteration if the previous iteration was successful
  if( step > 0.001 )
  {
    if( calculateProstateDoseCoverage() >= 0.98 )
    {
      d_treatment_plan = treatment_plan_copy;
      
      double start_needle_isodose_constant = 
	optimum_needle_isodose_constant-0.019;
      conductNeedleIsodoseConstantIteration( start_needle_isodose_constant,
					     optimum_needle_isodose_constant,
					     0.001,
					     chosen_needle_ids,
					     remaining_seed_positions );
    }
  }					   
}

// Create the seed position list
void IIEMTreatmentPlanner::createSeedPositionList( 
				   const std::vector<double> &prostate_adjoint,
				   const std::vector<double> &urethra_adjoint,
				   const std::vector<double> &margin_adjoint,
				   const std::vector<double> &rectum_adjoint )
{
  // Filter potential seed positions to those along the template positions and
  // set the position weight to the adjoint ratio at the position
  double weight = 0;
  unsigned needle = 0;
  
  for( unsigned j = 0; j < d_mesh_y_dim; ++j )
  {
    for( unsigned i = 0; i < d_mesh_x_dim; ++i )
    {
      unsigned needle_index = i + j*d_mesh_x_dim;
	  
      if( d_needle_template[needle_index] )
      {
	for( unsigned slice = 0; slice < d_mesh_z_dim; ++slice )
	{
	  unsigned mask_index = needle_index + slice*d_mesh_x_dim*d_mesh_y_dim;
	  
	  if( d_prostate_mask[mask_index] )
	  {
	    weight = (urethra_adjoint[mask_index] + margin_adjoint[mask_index]+
		      rectum_adjoint[mask_index])/prostate_adjoint[mask_index];
      
	    SeedPosition seed_position(i, 
				       j, 
				       slice, 
				       weight, 
				       d_seed->getSeedType() );
	    d_seed_positions.push_back(std::make_pair(needle, seed_position));
	  }
	}
      
	++needle;
      }
    }
  }
}

// Calculate the minimum seed isodose constant
double IIEMTreatmentPlanner::calculateMinSeedIsodoseConstant() const
{
  double min_isodose_constant = std::numeric_limits<double>::infinity();
  double isodose_constant = 0.0;

  std::list<std::pair<unsigned,SeedPosition> >::const_iterator 
    position, end_position;
  
  position = d_seed_positions.begin();
  end_position = d_seed_positions.end();
  
  while( position != end_position )
  {
    double seed_x = position->second.getXIndex()*d_mesh_element_x_dim + 
      d_mesh_element_x_dim/2;
    double seed_y = position->second.getYIndex()*d_mesh_element_y_dim + 
      d_mesh_element_y_dim/2;
    double seed_z = position->second.getZIndex()*d_mesh_element_z_dim + 
      d_mesh_element_z_dim/2;
    
    for( unsigned k = 0; k < d_mesh_z_dim; ++k )
    {
      double mesh_z = k*d_mesh_element_z_dim + d_mesh_element_z_dim/2;
      
      for( unsigned j = 0; j < d_mesh_y_dim; ++j )
      {
	double mesh_y = j*d_mesh_element_y_dim + d_mesh_element_y_dim/2;
	
	for( unsigned i = 0; i < d_mesh_x_dim; ++i )
	{
	  double mesh_x = i*d_mesh_element_z_dim + d_mesh_element_z_dim/2;
	  
	  if( d_prostate_mask[i+j*d_mesh_x_dim+k*d_mesh_x_dim*d_mesh_y_dim] )
	  {
	    double dose = d_seed->getTotalDose( mesh_x - seed_x,
						mesh_y - seed_y,
						mesh_z - seed_z );
	    if( dose > d_prescribed_dose )
	      dose = d_prescribed_dose;
	    
	    isodose_constant += dose;
	  }
	}
      }
    }
    
    isodose_constant /= (d_prostate_vol*d_prescribed_dose);
    
    if( isodose_constant < min_isodose_constant )
      min_isodose_constant = isodose_constant;
  }

  return min_isodose_constant;
}

// Map the seed dose distribution to the problem mesh
void IIEMTreatmentPlanner::mapSeedDoseDistribution( const double seed_x_index,
						    const double seed_y_index,
						    const double seed_z_index )
{
  // Make sure that the indices are valid
  testPrecondition( seed_x_index < d_mesh_x_dim );
  testPrecondition( seed_y_index < d_mesh_y_dim );
  testPrecondition( seed_z_index < d_mesh_z_dim );

  double seed_x = seed_x_index*d_mesh_element_x_dim + d_mesh_element_x_dim/2;
  double seed_y = seed_y_index*d_mesh_element_y_dim + d_mesh_element_y_dim/2;
  double seed_z = seed_z_index*d_mesh_element_z_dim + d_mesh_element_z_dim/2;

  for( unsigned k = 0; k < d_mesh_z_dim; ++k )
  {
    double mesh_z = k*d_mesh_element_z_dim + d_mesh_element_z_dim/2;
    
    for( unsigned j = 0; j < d_mesh_y_dim; ++j )
    {
      double mesh_y = j*d_mesh_element_y_dim + d_mesh_element_y_dim/2;

      for( unsigned i = 0; i < d_mesh_x_dim; ++i )
      {
	double mesh_x = i*d_mesh_element_x_dim + d_mesh_element_x_dim/2;

	d_dose_distribution[i+j*d_mesh_x_dim+k*d_mesh_x_dim*d_mesh_y_dim] +=
	  d_seed->getTotalDose( mesh_x - seed_x,
				mesh_y - seed_y,
				mesh_z - seed_z );
      }
    }
  }
}

// Calculate the prostate dose coverage (% of vol with prescribed dose)
double IIEMTreatmentPlanner::calculateProstateDoseCoverage() const
{
  unsigned number_elements_covered = 0;
  
  for( unsigned i = 0; i < d_dose_distribution.size(); ++i )
  {
    if( d_prostate_mask[i] && d_dose_distribution[i] > d_prescribed_dose )
      ++number_elements_covered;
  }

  return (double)number_elements_covered/d_prostate_vol;
}

//! Calculate the prostate dose coverage (% of vol with prescribed dose)
void IIEMTreatmentPlanner::calculateDoseVolumeHistogramData()
{
  
}

// Print the treatment plan
void IIEMTreatmentPlanner::printTreatmentPlan( std::ostream &os ) const
{

}

// Print the treatment plan to std::cout
void IIEMTreatmentPlanner::printTreatmentPlan() const
{
  printTreatmentPlan( std::cout );
}

// Print the dose-volume-histogram data
void IIEMTreatmentPlanner::printDoseVolumeHistogramData( 
						       std::ostream &os ) const
{

}

// Print the dose-volume-histogram data to std::cout
void IIEMTreatmentPlanner::printDoseVolumeHistogramData() const
{
  printDoseVolumeHistogramData( std::cout );
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end IIEMTreatmentPlanner.cpp
//---------------------------------------------------------------------------//
