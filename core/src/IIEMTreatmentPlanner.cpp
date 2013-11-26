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
#include <time.h>

// TPOR includes
#include "ContractException.hpp"
#include "BrachytherapyPatientFileHandler.hpp"
#include "BrachytherapyAdjointDataGenerator.hpp"
#include "IIEMTreatmentPlanner.hpp"

namespace TPOR
{

// Constructor
IIEMTreatmentPlanner::IIEMTreatmentPlanner( 
		     const std::string &patient_hdf5_file_name,
		     const boost::shared_ptr<BrachytherapySeedProxy> &seed,
		     const double prescribed_dose )
  : d_mesh_x_dim( 0 ),
    d_mesh_y_dim( 0 ),
    d_mesh_z_dim( 0 ),
    d_prescribed_dose( prescribed_dose ),
    d_prostate_relative_vol( 0 ),
    d_urethra_relative_vol( 0 ),
    d_margin_relative_vol( 0 ),
    d_rectum_relative_vol( 0 ),
    d_min_number_of_needles( 0 ),
    d_min_isodose_constant( 0.0 ),
    d_init_time( 0.0 ),
    d_opt_time( 0.0 )
{
  std::cout << std::endl << "initializing IIEM treatment planner..." 
	    << std::endl;
  clock_t start_clock = clock();
  
  // Create the file handler for the desired patient file
  BrachytherapyPatientFileHandler patient_file( patient_hdf5_file_name );

  // Load in the mesh dimensions
  std::vector<unsigned> mesh_dimensions;
  patient_file.getOrganMeshDimensions( mesh_dimensions );
  
  d_mesh_x_dim = mesh_dimensions[0];
  d_mesh_y_dim = mesh_dimensions[1];
  d_mesh_z_dim = mesh_dimensions[2];

  mesh_dimensions.clear();

  // Load the prostate mask relative volume
  patient_file.getProstateMaskRelativeVolume( d_prostate_relative_vol );
  
  // Load in the prostate mask
  patient_file.getProstateMask( d_prostate_mask );

  // Load the urethra mask relative volume
  patient_file.getUrethraMaskRelativeVolume( d_urethra_relative_vol );
  
  // Load in the urethra mask
  patient_file.getUrethraMask( d_urethra_mask );

  // Load the margin mask relative volume
  patient_file.getMarginMaskRelativeVolume( d_margin_relative_vol );
  
  // Load in the margin mask
  patient_file.getMarginMask( d_margin_mask );

  // Load the rectum mask relative volume
  patient_file.getRectumMaskRelativeVolume( d_rectum_relative_vol );
  
  // Load in the rectum mask
  patient_file.getRectumMask( d_rectum_mask );

  // Load in the needle template
  patient_file.getNeedleTemplate( d_needle_template );

  std::vector<double> prostate_adjoint_data;
  std::vector<double> urethra_adjoint_data;
  std::vector<double> margin_adjoint_data;
  std::vector<double> rectum_adjoint_data;

  // Load the adjoint data for the desired seed if it has been cached  
  if( patient_file.adjointDataExists( seed->getSeedName() ) )
  {
    // Load in the prostate adjoint data
    patient_file.getProstateAdjointData( prostate_adjoint_data,
					 seed->getSeedName(),
					 seed->getSeedStrength() );

    // Load in the urethra adjoint data
    patient_file.getUrethraAdjointData( urethra_adjoint_data,
					seed->getSeedName(),
					seed->getSeedStrength() );

    // Load in the margin adjoint data
    patient_file.getMarginAdjointData( margin_adjoint_data,
				       seed->getSeedName(),
				       seed->getSeedStrength() );
  
    // Load in the rectum adjoint data
    patient_file.getRectumAdjointData( rectum_adjoint_data,
				       seed->getSeedName(),
				       seed->getSeedStrength() );
  }

  // Genenerate the adjoint data if it is not in the cache
  else
  {
    BrachytherapyAdjointDataGenerator adjoint_gen( seed );
						       
    std::cout << "generating prostate adjoint data for "
	      << seed->getSeedName() << "..." << std::endl;
    adjoint_gen.calculateCondensedAdjointDose( prostate_adjoint_data,
					       d_prostate_mask,
					       d_prostate_mask,
					       d_mesh_x_dim,
					       d_mesh_y_dim,
					       d_mesh_z_dim );

    std::cout << "generating urethra adjoint data for "
	      << seed->getSeedName() << "..." << std::endl;
    adjoint_gen.calculateCondensedAdjointDose( urethra_adjoint_data,
					       d_urethra_mask,
					       d_prostate_mask,
					       d_mesh_x_dim,
					       d_mesh_y_dim,
					       d_mesh_z_dim );

    std::cout << "generating margin adjoint data for "
	      << seed->getSeedName() << "..." << std::endl;
    adjoint_gen.calculateCondensedAdjointDose( margin_adjoint_data,
					       d_margin_mask,
					       d_prostate_mask,
					       d_mesh_x_dim,
					       d_mesh_y_dim,
					       d_mesh_z_dim );

    std::cout << "generating rectum adjoint data for "
	      << seed->getSeedName() << "..." << std::endl;
    adjoint_gen.calculateCondensedAdjointDose( rectum_adjoint_data,
					       d_rectum_mask,
					       d_prostate_mask,
					       d_mesh_x_dim,
					       d_mesh_y_dim,
					       d_mesh_z_dim );

    // Cache this adjoint data
    patient_file.setProstateAdjointData( prostate_adjoint_data,
					 seed->getSeedName(),
					 seed->getSeedStrength() );

    patient_file.setUrethraAdjointData( urethra_adjoint_data,
					seed->getSeedName(),
					seed->getSeedStrength() );

    patient_file.setMarginAdjointData( margin_adjoint_data,
				       seed->getSeedName(),
				       seed->getSeedStrength() );

    patient_file.setRectumAdjointData( rectum_adjoint_data,
				       seed->getSeedName(),
				       seed->getSeedStrength() );
  }

  // Create the seed position list
  createSeedPositionList( seed,
			  prostate_adjoint_data,
			  urethra_adjoint_data,
			  margin_adjoint_data,
			  rectum_adjoint_data );

  // Sort the seed positions
  d_seed_positions.sort();

  // Determine the minimum number of needles that will be needed 
  // (Sua's linear fit)
  double prostate_volume; 
  patient_file.getProstateMaskVolume( prostate_volume );
  
  d_min_number_of_needles = 
    static_cast<unsigned>( floor( 0.24*prostate_volume + 11.33 ) );
  
  // Determine the minimum seed isodose constant for the seed selection process
  d_min_isodose_constant = calculateMinSeedIsodoseConstant();
  
  // Initialize the dose distribution
  d_dose_distribution.resize( d_prostate_mask.size() );

  // Initialize the dose-volume-histogram array
  d_dose_volume_histogram_data.resize( 5*301 );

  clock_t end_clock = clock();

  // Store the initialization time
  d_init_time = (double)(end_clock-start_clock)/CLOCKS_PER_SEC;
}

// Calculate optimum treatment plan
void IIEMTreatmentPlanner::calculateOptimumTreatmentPlan()
{
  std::cout << std::endl << "starting treatment plan optimization..." 
	    << std::endl;
  clock_t start_clock = clock();
  
  // Make a copy of the seed positions
  std::list<std::pair<unsigned,BrachytherapySeedPosition> > 
    seed_positions_copy = d_seed_positions;

  // Create a set for the needle ids
  std::set<unsigned> chosen_needle_ids;

  // Start the selection process
  // Outer iteration: needle_goal
  // Inner iteration A: isodose_constant
  // Inner iteration B: needle_isodose_constant
  for( unsigned needle_goal = d_min_number_of_needles;
       needle_goal <= 30;
       ++needle_goal )
  {
    std::list<std::pair<unsigned,BrachytherapySeedPosition> > 
      remaining_seed_positions;

    // Conduct the inner iteration A
    std::cout << "conducting isodose constant iteration for needle goal of "
	      << needle_goal << "..." << std::endl;
    
    conductIsodoseConstantIteration( needle_goal,
				     chosen_needle_ids,
				     remaining_seed_positions );
    
    // Check for a failed inner iteration A
    if( calculateProstateDoseCoverage() < 0.98 )
      continue;

    // The inner iterations were successful
    else
      break;
  }   

  clock_t end_clock = clock();

  // Store the optimization time
  d_opt_time = (double)(end_clock-start_clock)/CLOCKS_PER_SEC;

  // Check if the algorithm failed to create a treatment plan
  if( calculateProstateDoseCoverage() < 0.98 )
  {
    std::cout << "Warning: The treatment planning algorithm was not "
	      << "successful." << std::endl;
  }  
  
  // If successful, generate the dose-volume-histogram data
  else
  {
    std::cout << std::endl << "treatment plan optimization complete." 
	      << std::endl << std::endl;
  }

  // Print the treatment plan summary
  printTreatmentPlanSummary( std::cout );

  // Calculate the dose-volume-histogram data
  calculateDoseVolumeHistogramData();
}

// Conduct the isodose constant iteration
void IIEMTreatmentPlanner::conductIsodoseConstantIteration( 
                      unsigned needle_goal,
		      std::set<unsigned> &chosen_needle_ids,
                      std::list<std::pair<unsigned,BrachytherapySeedPosition> >
		      &remaining_seed_positions )
{
  for( double isodose_constant = 0.001; 
       isodose_constant <= d_min_isodose_constant;
       isodose_constant += 0.001 )
  {

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
    
    // Map the seed dose distribution to the problem mesh (reset)
    d_treatment_plan.front().second.mapSeedDoseDistribution<Equal>( 
						           d_dose_distribution,
							   d_mesh_x_dim,
							   d_mesh_y_dim,
							   d_mesh_z_dim );
    
    // Keep selecting seeds until the needle goal has been reached
    while( chosen_needle_ids.size() < needle_goal )
    {
      // Select the next acceptable seed position
      std::list<std::pair<unsigned,BrachytherapySeedPosition> >::iterator 
	seed_position;
      
      for( seed_position = remaining_seed_positions.begin(); 
	   seed_position != remaining_seed_positions.end();
	   ++seed_position )
      {
	unsigned index = seed_position->second.getXIndex() +
	  seed_position->second.getYIndex()*d_mesh_x_dim +
	  seed_position->second.getZIndex()*d_mesh_x_dim*d_mesh_y_dim;
	
	double dose_cutoff = 
	  d_prescribed_dose*isodose_constant*d_treatment_plan.size();
	
	if( d_dose_distribution[index] < dose_cutoff )
	{
	  d_treatment_plan.push_back( *seed_position );
	  
	  chosen_needle_ids.insert( seed_position->first );

	  seed_position->second.mapSeedDoseDistribution<PlusEqual>( 
							   d_dose_distribution,
							   d_mesh_x_dim,
							   d_mesh_y_dim,
							   d_mesh_z_dim );
	  
	  seed_position = remaining_seed_positions.erase( seed_position );
	  
	  break;
	}
      }
      
      // If no acceptable seed position was found, exit this inner iteration
      if( seed_position == remaining_seed_positions.end() )
	break;
      
    }
    
    // Check for a failed iteration
    if( chosen_needle_ids.size() != needle_goal )
      continue;

    // Store a copy of the treatment plan for running refined inner iteration B
    std::list<std::pair<unsigned,BrachytherapySeedPosition> > 
      treatment_plan_copy = d_treatment_plan;

    // Store a copy of the dose distribution
    std::vector<double> dose_distribution_copy = d_dose_distribution;
    
    // Conduct the inner iteration B
    std::cout << "conducting needle isodose constant iteration..."
	      << std::endl;
    double needle_isodose_constant = 
      conductNeedleIsodoseConstantIteration( 1.02,
					     1.08,
					     0.02,
					     chosen_needle_ids,
					     remaining_seed_positions );

    // Check for a successful inner iteration B
    if( calculateProstateDoseCoverage() >= 0.98 )
    {
      // Run a refined inner iteration B
      d_treatment_plan = treatment_plan_copy;
      d_dose_distribution = dose_distribution_copy;
      
      needle_isodose_constant = 
	conductNeedleIsodoseConstantIteration( needle_isodose_constant-0.019,
					       needle_isodose_constant,
					       0.001,
					       chosen_needle_ids,
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
		const std::set<unsigned> &chosen_needle_ids,
                const std::list<std::pair<unsigned,BrachytherapySeedPosition> >
		&remaining_seed_positions )
{
  // Make sure the start constant is valid
  testPrecondition( start_constant > 0.0 );
  // Make sure the start constant is less than the end constant
  testPrecondition( start_constant < end_constant );
  // Make sure the step is less than the distance between the start/end const
  testPrecondition( step <= end_constant - start_constant );

  // Store a copy of the treatment plan as it currently is
  std::list<std::pair<unsigned,BrachytherapySeedPosition> > 
    treatment_plan_copy = d_treatment_plan;

  // Store a copy of the dose distribution as is currently is
  std::vector<double> dose_distribution_copy = d_dose_distribution;

  // Store a copy of the remaining seed positions
  std::list<std::pair<unsigned,BrachytherapySeedPosition> > 
    remaining_seed_positions_copy = remaining_seed_positions;

  // Store the optimum needle isodose constant
  double optimum_needle_isodose_constant = 0.0;
  
  for( double needle_isodose_constant = start_constant;
       needle_isodose_constant <= end_constant;
       needle_isodose_constant += step )
  {    
    // Select seeds along current needles until 98% of prostate receives 
    // prescribed dose
    while( calculateProstateDoseCoverage() < 0.98 )
    {
      // Select the next acceptable seed position
      std::list<std::pair<unsigned,BrachytherapySeedPosition> >::iterator 
	seed_position;
      
      for( seed_position = remaining_seed_positions_copy.begin(); 
	   seed_position != remaining_seed_positions_copy.end();
	   ++seed_position )
      {
	unsigned index = seed_position->second.getXIndex() +
	  seed_position->second.getYIndex()*d_mesh_x_dim +
	  seed_position->second.getZIndex()*d_mesh_x_dim*d_mesh_y_dim;
	
	double dose_cutoff = d_prescribed_dose*needle_isodose_constant;
	
	if( d_dose_distribution[index] < dose_cutoff &&
	    chosen_needle_ids.count( seed_position->first ) == 1 )
	{
	  d_treatment_plan.push_back( *seed_position );
	  
	  seed_position->second.mapSeedDoseDistribution<PlusEqual>( 
							   d_dose_distribution,
							   d_mesh_x_dim,
							   d_mesh_y_dim,
							   d_mesh_z_dim );
	  seed_position = remaining_seed_positions_copy.erase( seed_position );
	  
	  break;
	}
      }
      
      // If no acceptable seed position was found, exit this inner iteration
      if( seed_position == remaining_seed_positions_copy.end() )
	break;
    }

    // Check if the inner iteration was successful
    if( calculateProstateDoseCoverage() >= 0.98 )
    {
      optimum_needle_isodose_constant = needle_isodose_constant;
      break;
    }
    else
    {
      // Reset the treatment plan
      d_treatment_plan = treatment_plan_copy;

      // Reset the dose distribution
      d_dose_distribution = dose_distribution_copy;
      
      // Reset the remaining seed positions
      remaining_seed_positions_copy = remaining_seed_positions;
    }
  }

  return optimum_needle_isodose_constant;
}

// Create the seed position list
void IIEMTreatmentPlanner::createSeedPositionList( 
			 const boost::shared_ptr<BrachytherapySeedProxy> &seed,
			 const std::vector<double> &prostate_adjoint,
			 const std::vector<double> &urethra_adjoint,
			 const std::vector<double> &margin_adjoint,
			 const std::vector<double> &rectum_adjoint )
{ 
  // Filter potential seed positions to those along the template positions and
  // set the position weight to the adjoint ratio at the position
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
	    double weight = 
	      (urethra_adjoint[mask_index] + margin_adjoint[mask_index]+
	       rectum_adjoint[mask_index])/prostate_adjoint[mask_index];
	    
	    BrachytherapySeedPosition seed_position(i, j, slice, weight, seed);
	    
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

  std::list<std::pair<unsigned,BrachytherapySeedPosition> >::const_iterator 
    position, end_position;
  
  position = d_seed_positions.begin();
  end_position = d_seed_positions.end();
  
  std::vector<double> tmp_dose_distribution( d_prostate_mask.size(), 0.0 );
  
  while( position != end_position )
  {
    position->second.mapSeedDoseDistribution<Equal>( tmp_dose_distribution,
						     d_mesh_x_dim,
						     d_mesh_y_dim,
						     d_mesh_z_dim );
        
    for( unsigned index = 0; index < tmp_dose_distribution.size(); ++index )
    {
      if( d_prostate_mask[index] )
      {
	if( tmp_dose_distribution[index] > d_prescribed_dose )
	  isodose_constant += d_prescribed_dose;
	else
	  isodose_constant += tmp_dose_distribution[index];
      }
    }
    
    isodose_constant /= (d_prostate_relative_vol*d_prescribed_dose);
    
    if( isodose_constant < min_isodose_constant )
      min_isodose_constant = isodose_constant;

    ++position;
  }

  return min_isodose_constant;
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

  return (double)number_elements_covered/d_prostate_relative_vol;
}

//! Calculate the prostate dose coverage (% of vol with prescribed dose)
void IIEMTreatmentPlanner::calculateDoseVolumeHistogramData()
{

  unsigned prostate_elements = 0, urethra_elements = 0, normal_elements = 0,
    rectum_elements = 0;
  unsigned normal_relative_vol = d_mesh_x_dim*d_mesh_y_dim*d_mesh_z_dim -
    d_prostate_relative_vol - d_urethra_relative_vol - d_rectum_relative_vol;
  
  for( int dose = 0; dose <= 300; ++dose )
  {
    for( int k = 0; k < d_mesh_z_dim; ++k )
    {
      for( int j = 0; j < d_mesh_y_dim; ++j )
      {
	for( int i = 0; i < d_mesh_x_dim; ++i )
	{
	  unsigned index = i + j*d_mesh_x_dim + k*d_mesh_x_dim*d_mesh_y_dim;
	  double dose_cgy = dose*100;
	  
	  if( d_prostate_mask[index] && d_dose_distribution[index] >= 
	      dose_cgy )
	    ++prostate_elements;
	  else if( d_urethra_mask[index] && d_dose_distribution[index] >= 
		   dose_cgy)
	    ++urethra_elements;
	  else if( d_rectum_mask[index] && d_dose_distribution[index] >= 
		   dose_cgy )
	    ++rectum_elements;
	  else if( d_dose_distribution[index] >= dose_cgy )
	    ++normal_elements;
	}
      }
    }

    d_dose_volume_histogram_data[dose*5] = dose;
    d_dose_volume_histogram_data[dose*5+1] = 
      (double)prostate_elements/d_prostate_relative_vol;
    d_dose_volume_histogram_data[dose*5+2] = 
      (double)urethra_elements/d_urethra_relative_vol;
    d_dose_volume_histogram_data[dose*5+3] = 
      (double)rectum_elements/d_rectum_relative_vol;
    d_dose_volume_histogram_data[dose*5+4] = 
      (double)normal_elements/normal_relative_vol;

    prostate_elements = 0;
    urethra_elements = 0;
    rectum_elements = 0;
    normal_elements = 0;
  }
}

// Print the treatment plan
void IIEMTreatmentPlanner::printTreatmentPlan( std::ostream &os ) const
{
  printTreatmentPlanSummary( os );
  
  os << "Needle Seed  Seed Type\t\tSeed Indices\n";

  std::list<std::pair<unsigned,BrachytherapySeedPosition> >::const_iterator
    position, end_position;
  position = d_treatment_plan.begin();
  end_position = d_treatment_plan.end();
  
  unsigned position_number = 1;
  
  while( position != end_position )
  {
    os << std::setw(3) << position->first << "    "
       << std::setw(2) << position_number << "    "
       << position->second.getSeedName() << " "
       << std::setw(3) << position->second.getXIndex() << " "
       << std::setw(3) << position->second.getYIndex() << " "
       << std::setw(2) << position->second.getZIndex() << "\n";

    ++position;
    ++position_number;
  }

  os << std::endl;
}

// Print the treatment plan to std::cout
void IIEMTreatmentPlanner::printTreatmentPlan() const
{
  printTreatmentPlan( std::cout );
}

// Print the treatment plan summary
void IIEMTreatmentPlanner::printTreatmentPlanSummary( std::ostream &os ) const
{
  std::set<unsigned> needle_set;
  std::list<std::pair<unsigned,BrachytherapySeedPosition> >::const_iterator
    start = d_treatment_plan.begin(), end = d_treatment_plan.end();
  while( start != end )
  {
    needle_set.insert( start->first );

    ++start;
  }
  
  os.precision( 3 );
  os.setf( std::ios::fixed, std::ios::floatfield );
  os << "...Treatment Plan Summary..." << std::endl;
  os << "Initialization Time (s):    " << d_init_time << std::endl;
  os << "Plan Optimization Time (s): " << d_opt_time << std::endl;
  os << "Total Time (s):             " << (d_init_time + d_opt_time) 
     << std::endl;
  os << "Successful Optimization:    " 
     << (calculateProstateDoseCoverage() >= 0.98 ? "Yes" : "No")
     << std::endl;
  os << "Needles Chosen:             " << needle_set.size() 
     << std::endl;
  os << "Seeds Chosen:               " << d_treatment_plan.size() 
     << std::endl;
  os << std::endl;
}

// Print the dose-volume-histogram data
void IIEMTreatmentPlanner::printDoseVolumeHistogramData( 
						       std::ostream &os ) const
{
  os << "# Dose[Gy] Prostate Urethra  Rectum   Normal\n";
  os.precision( 6 );
  os.setf( std::ios::fixed, std::ios::floatfield );
  
  for( unsigned i = 0; i <= 300; ++i )
  {
    os << std::setw( 10 ) << d_dose_volume_histogram_data[i*5] << " " 
       << d_dose_volume_histogram_data[i*5+1] << " "
       << d_dose_volume_histogram_data[i*5+2] << " "
       << d_dose_volume_histogram_data[i*5+3] << " "
       << d_dose_volume_histogram_data[i*5+4] << "\n";
  }
  
  os << std::endl;
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
