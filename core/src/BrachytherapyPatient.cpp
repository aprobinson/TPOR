//---------------------------------------------------------------------------//
//!
//! \file   BrachtherapyPatient.cpp
//! \author Alex Robinson
//! \brief  BrachytherapyPatient class definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <algorithm>
#include <iomanip>
#include <map>

// TPOR Includes
#include "BrachytherapyPatient.hpp"
#include "BrachytherapyPatientFileHandler.hpp"
#include "ContractException.hpp"

namespace TPOR{

// Constructor
/*! \details the prescribed dose must be in units of cGy
 */ 
BrachytherapyPatient::BrachytherapyPatient( 
					  const std::string &patient_file_name,
					  const double prescribed_dose )
  : d_patient_file_name( patient_file_name ),
    d_prescribed_dose( prescribed_dose ),
    d_mesh_x_dim( 0u ),
    d_mesh_y_dim( 0u ),
    d_mesh_z_dim( 0u ),
    d_prostate_relative_vol( 0u ),
    d_urethra_relative_vol( 0u ),
    d_rectum_relative_vol( 0u ),
    d_normal_relative_vol( 0u ),
    d_prostate_mask(),
    d_urethra_mask(),
    d_margin_mask(),
    d_rectum_mask(),
    d_needle_template(),
    d_treatment_plan(),
    d_treatment_plan_needles(),
    d_dose_distribution(),
    d_cached_treatment_plan(),
    d_cached_treatment_plan_needles(),
    d_cached_dose_distribution()
{
  // Make sure the prescribed dose is valid
  testPrecondition( prescribed_dose > 0.0 );
  
  std::cout << std::endl << "creating patient..." << std::endl;

  // Create the file handler for the patient
  BrachytherapyPatientFileHandler patient_file( d_patient_file_name );

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
  
  // Load in the margin mask
  patient_file.getMarginMask( d_margin_mask );

  // Load the rectum mask relative volume
  patient_file.getRectumMaskRelativeVolume( d_rectum_relative_vol );
  
  // Load in the rectum mask
  patient_file.getRectumMask( d_rectum_mask );

  // Set the normal tissue relative volume
  d_normal_relative_vol = d_mesh_x_dim*d_mesh_y_dim*d_mesh_z_dim -
    d_prostate_relative_vol - d_urethra_relative_vol - d_rectum_relative_vol;

  // Load in the needle template
  patient_file.getNeedleTemplate( d_needle_template );

  // Resize the dose distribution vectors
  unsigned size = d_mesh_x_dim*d_mesh_y_dim*d_mesh_z_dim;
  d_dose_distribution.resize( size, 0.0 );
  d_cached_dose_distribution.resize( size, 0.0 );
}

// Insert a seed at the desired seed position and return the seed id number
unsigned BrachytherapyPatient::insertSeed( 
			       const BrachytherapySeedPosition &seed_position )
{
  // Make sure that the seed position hasn't already been added
  testPrecondition( isSeedPositionFree( seed_position ) );
  
  // Map the seed dose distribution to the patient dose distribution
  if( d_treatment_plan.size() == 0 )
  {
    seed_position.mapSeedDoseDistribution<Equal>( d_dose_distribution,
						  d_mesh_x_dim,
						  d_mesh_y_dim,
						  d_mesh_z_dim );
  }
  else
  {
    seed_position.mapSeedDoseDistribution<PlusEqual>( d_dose_distribution,
						      d_mesh_x_dim,
						      d_mesh_y_dim,
						      d_mesh_z_dim );
  }

  d_treatment_plan.push_back( seed_position );

  d_treatment_plan_needles.insert( seed_position.getXIndex() +
				   seed_position.getYIndex()*d_mesh_x_dim );
}

// Test if the seed position lies on an inserted needle
bool BrachytherapyPatient::isSeedOnNeedle( 
			 const BrachytherapySeedPosition &seed_position ) const
{
  unsigned needle_index = seed_position.getXIndex() +
    seed_position.getYIndex()*d_mesh_x_dim;
  
  if( d_treatment_plan_needles.count( needle_index ) == 1 )
    return true;
  else
    return false;
}

// Test if the seed position is unique
bool BrachytherapyPatient::isSeedPositionFree( 
			 const BrachytherapySeedPosition &seed_position ) const
{
  std::list<BrachytherapySeedPosition>::const_iterator 
    position = d_treatment_plan.begin();
  std::list<BrachytherapySeedPosition>::const_iterator 
    end_position = d_treatment_plan.end();

  bool free_position = true;
  
  while( position != end_position )
  {
    if( *position == seed_position )
    {
      free_position = false;
      
      break;
    }

    ++position;
  }

  return free_position;
}

// Return the dose at a seed position
double BrachytherapyPatient::getDose( 
			const BrachytherapySeedPosition &seed_position ) const
{
  return getDose( seed_position.getXIndex(),
		  seed_position.getYIndex(),
		  seed_position.getZIndex() );
}
  
// Return the dose at a mesh point (cGy)
double BrachytherapyPatient::getDose( const unsigned x_mesh_index,
				      const unsigned y_mesh_index,
				      const unsigned z_mesh_index ) const
{
  // Make sure that the indices are valid
  testPrecondition( x_mesh_index < d_mesh_x_dim );
  testPrecondition( y_mesh_index < d_mesh_y_dim );
  testPrecondition( z_mesh_index < d_mesh_z_dim );

  unsigned index = x_mesh_index + y_mesh_index*d_mesh_x_dim +
    z_mesh_index*d_mesh_x_dim*d_mesh_y_dim;

  return d_dose_distribution[index];
}

// Return the prescribed dose
double BrachytherapyPatient::getPrescribedDose() const
{
  return d_prescribed_dose;
}
  
// Return the organ mesh x dimension
unsigned BrachytherapyPatient::getOrganMeshXDim() const
{
  return d_mesh_x_dim;
}
  
// Return the organ mesh y dimension
unsigned BrachytherapyPatient::getOrganMeshYDim() const
{
  return d_mesh_y_dim;
}
  
// Return the organ mesh z dimension
unsigned BrachytherapyPatient::getOrganMeshZDim() const
{
  return d_mesh_z_dim;
}

// Return the prostate mask size (number of prostate elements)
double BrachytherapyPatient::getProstateVolume() const
{
  return d_prostate_relative_vol*0.1*0.1*0.5;
}

// Return the urethra mask size (number of urethra elements)
double BrachytherapyPatient::getUrethraVolume() const
{
  return d_urethra_relative_vol*0.1*0.1*0.5;
}

// Return the rectum mask size (number of rectum elements)
double BrachytherapyPatient::getRectumVolume() const
{
  return d_rectum_relative_vol*0.1*0.1*0.5;
}

// Return the normal mask size (number of normal elements)
double BrachytherapyPatient::getNormalVolume() const
{
  return d_normal_relative_vol*0.1*0.1*0.5;
}

// Return the prostate size (num prostate elements)
unsigned BrachytherapyPatient::getProstateSize() const
{
  return d_prostate_relative_vol;
}

// Return the urethra size (num urethra elements)
unsigned BrachytherapyPatient::getUrethraSize() const
{
  return d_urethra_relative_vol;
}

// Return the rectum size (num rectum elements)
unsigned BrachytherapyPatient::getRectumSize() const
{
  return d_rectum_relative_vol;
}

// Return the normal size (num normal elements)
unsigned BrachytherapyPatient::getNormalSize() const
{
  return d_normal_relative_vol;
}

// Return the tissue type at a mesh point
TissueType BrachytherapyPatient::getTissueType( 
					    const unsigned x_mesh_index,
					    const unsigned y_mesh_index,
					    const unsigned z_mesh_index ) const
{
  // Make sure that the indices are valid
  testPrecondition( x_mesh_index < d_mesh_x_dim );
  testPrecondition( y_mesh_index < d_mesh_y_dim );
  testPrecondition( z_mesh_index < d_mesh_z_dim );

  unsigned index = x_mesh_index + y_mesh_index*d_mesh_x_dim +
    z_mesh_index*d_mesh_x_dim*d_mesh_y_dim;
  
  if( d_prostate_mask[index] )
    return PROSTATE_TISSUE;
  else if( d_urethra_mask[index] )
    return URETHRA_TISSUE;
  else if( d_rectum_mask[index] )
    return RECTUM_TISSUE;
  else if( d_margin_mask[index] )
    return MARGIN_TISSUE;
  else
    return NORMAL_TISSUE;
}

// Return the prostate dose coverage
double BrachytherapyPatient::getProstateDoseCoverage() const
{
  unsigned number_elements_covered = 0;
  
  for( unsigned i = 0; i < d_dose_distribution.size(); ++i )
  {
    if( d_prostate_mask[i] && d_dose_distribution[i] > d_prescribed_dose )
      ++number_elements_covered;
  }

  return (double)number_elements_covered/d_prostate_relative_vol;
}

// Return the number of inserted needles
unsigned BrachytherapyPatient::getNumInsertedNeedles() const
{
  return d_treatment_plan_needles.size();
}

// Return the number of inserted seeds
unsigned BrachytherapyPatient::getNumInsertedSeeds() const
{
  return d_treatment_plan.size();
}

// Save the current state of the patient
void BrachytherapyPatient::saveState()
{
  d_cached_treatment_plan = d_treatment_plan;
  d_cached_treatment_plan_needles = d_treatment_plan_needles;
  d_cached_dose_distribution = d_dose_distribution;
}

// Load the previously saved state of the patient
void BrachytherapyPatient::loadSavedState()
{
  d_treatment_plan = d_cached_treatment_plan;
  d_treatment_plan_needles = d_cached_treatment_plan_needles;
  d_dose_distribution = d_cached_dose_distribution;
}

// Reset the state of the patient
void BrachytherapyPatient::resetState()
{
  d_treatment_plan.clear();
  d_treatment_plan_needles.clear();

  std::fill( d_dose_distribution.begin(), d_dose_distribution.end(), 0.0 );
}

// Print the treatment plan
void BrachytherapyPatient::printTreatmentPlan( std::ostream &os ) const
{
  os << "Needle Seed  Seed Type\t\tSeed Indices\n";

  std::list<BrachytherapySeedPosition>::const_iterator position, end_position;
  position = d_treatment_plan.begin();
  end_position = d_treatment_plan.end();
  
  unsigned position_number = 1;

  std::map<unsigned,unsigned> needle_id_map;
  
  while( position != end_position )
  {
    unsigned needle_index = position->getXIndex() + 
      position->getYIndex()*d_mesh_x_dim;
    
    unsigned needle_id;
    
    if( needle_id_map.count( needle_index ) == 0 )
    {
      needle_id = needle_id_map.size()+1;
      needle_id_map[needle_index] = needle_id;
    }
    else
      needle_id = needle_id_map[needle_index];
      
    os << std::setw(3) << needle_id << "    "
       << std::setw(2) << position_number << "    "
       << position->getSeedName() << " "
       << std::setw(3) << position->getXIndex() << " "
       << std::setw(3) << position->getYIndex() << " "
       << std::setw(2) << position->getZIndex() << "\n";

    ++position;
    ++position_number;
  }

  os << std::endl;
}

// Print the treatment plan to std::cout
void BrachytherapyPatient::printTreatmentPlan() const
{
  printTreatmentPlan( std::cout );
}

// Print the dose-volume-histogram data
void BrachytherapyPatient::printDoseVolumeHistogramData( 
						       std::ostream &os ) const
{
  os << "# Dose[Gy] Prostate Urethra  Rectum   Normal\n";
  os.precision( 6 );
  os.setf( std::ios::fixed, std::ios::floatfield );

  unsigned prostate_elements = 0, urethra_elements = 0, normal_elements = 0,
    rectum_elements = 0;
  
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

    os << std::setw( 10 ) << dose << " " 
       << (double)prostate_elements/d_prostate_relative_vol << " "
       << (double)urethra_elements/d_urethra_relative_vol << " "
       << (double)rectum_elements/d_rectum_relative_vol << " "
       << (double)normal_elements/d_normal_relative_vol << "\n";

    prostate_elements = 0;
    urethra_elements = 0;
    rectum_elements = 0;
    normal_elements = 0;
  }
  
  os << std::endl;
}

// Print the dose-volume-histogram data to std::cout
void BrachytherapyPatient::printDoseVolumeHistogramData() const
{
  printDoseVolumeHistogramData( std::cout );
}

} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyPatient.cpp
//---------------------------------------------------------------------------//
