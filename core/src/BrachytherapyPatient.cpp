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

// Boost Includes
#include <boost/scoped_ptr.hpp>

// MOAB Includes
#include <MBInterface.hpp>
#include <MBCore.hpp>
#include <MBRange.hpp>

// TPOR Includes
#include "BrachytherapyPatient.hpp"
#include "BrachytherapyPatientFileHandler.hpp"
#include "ContractException.hpp"
#include "ExceptionTestMacros.hpp"
#include "ExceptionCatchMacros.hpp"

namespace TPOR{

// Constructor
/*! \details the prescribed dose must be in units of cGy
 */ 
BrachytherapyPatient::BrachytherapyPatient( 
					  const std::string &patient_file_name,
					  const double prescribed_dose,
					  const double urethra_weight,
					  const double rectum_weight,
					  const double margin_weight )
  : d_patient_file_name( patient_file_name ),
    d_prescribed_dose( prescribed_dose ),
    d_mesh_x_dim( 0u ),
    d_mesh_y_dim( 0u ),
    d_mesh_z_dim( 0u ),
    d_prostate_relative_vol( 0u ),
    d_urethra_relative_vol( 0u ),
    d_rectum_relative_vol( 0u ),
    d_normal_relative_vol( 0u ),
    d_urethra_weight( urethra_weight ),
    d_rectum_weight( rectum_weight ),
    d_margin_weight( margin_weight ),
    d_prostate_mask(),
    d_urethra_mask(),
    d_margin_mask(),
    d_rectum_mask(),
    d_needle_template(),
    d_treatment_plan(),
    d_treatment_plan_needles(),
    d_treatment_plan_positions(),
    d_dose_distribution(),
    d_cached_treatment_plan(),
    d_cached_treatment_plan_needles(),
    d_cached_treatment_plan_positions(),
    d_cached_dose_distribution()
{
  // Make sure the prescribed dose is valid
  testPrecondition( prescribed_dose > 0.0 );
  // Make sure that the organ weights are valid
  testPrecondition( urethra_weight > 0.0 );
  testPrecondition( rectum_weight > 0.0 );
  testPrecondition( margin_weight > 0.0 );
  
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

  unsigned needle_index = seed_position.getXIndex() +
    seed_position.getYIndex()*d_mesh_x_dim;
  d_treatment_plan_needles.insert( needle_index );

  unsigned seed_index = needle_index + 
    seed_position.getZIndex()*d_mesh_x_dim*d_mesh_y_dim;
  d_treatment_plan_positions.insert( seed_index );
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
  unsigned seed_index = seed_position.getXIndex() +
    seed_position.getYIndex()*d_mesh_x_dim +
    seed_position.getZIndex()*d_mesh_x_dim*d_mesh_y_dim;
  
  if( d_treatment_plan_positions.count( seed_index ) == 0 )
    return true;
  else
    return false;
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
  d_cached_treatment_plan_positions = d_treatment_plan_positions;
  d_cached_dose_distribution = d_dose_distribution;
}

// Load the previously saved state of the patient
void BrachytherapyPatient::loadSavedState()
{
  d_treatment_plan = d_cached_treatment_plan;
  d_treatment_plan_needles = d_cached_treatment_plan_needles;
  d_treatment_plan_positions = d_cached_treatment_plan_positions;
  d_dose_distribution = d_cached_dose_distribution;
}

// Reset the state of the patient
void BrachytherapyPatient::resetState()
{
  d_treatment_plan.clear();
  d_treatment_plan_needles.clear();
  d_treatment_plan_positions.clear();

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

// Export the patient data to a vtk vile for 3D visualization
void BrachytherapyPatient::exportDataToVTK( 
				       const bool export_treatment_plan ) const
{
  // Set the mesh element dimensions (should always be the same)
  double mesh_element_x_dim = 0.1;
  double mesh_element_y_dim = 0.1;
  double mesh_element_z_dim = 0.5;
  
  // Create the mesh element corner coordinates
  std::vector<double> coordinates;
  
  for( unsigned k = 0; k <= d_mesh_z_dim; ++k )
  {
    for( unsigned j = 0; j <= d_mesh_y_dim; ++j )
    {
      for( unsigned i = 0; i <= d_mesh_x_dim; ++i )
      {
	unsigned index = i + j*(d_mesh_x_dim+1) + 
	  k*(d_mesh_x_dim+1)*(d_mesh_y_dim+1);
	
	coordinates.push_back( i*mesh_element_x_dim );
	coordinates.push_back( j*mesh_element_y_dim );
	coordinates.push_back( k*mesh_element_z_dim );
      }
    }
  }
 
  try{
    // Initialize the MOAB interface
    boost::scoped_ptr<moab::Interface> moab_interface( new moab::Core() );

    // Tag creation flag
    unsigned tag_creation_flag = moab::MB_TAG_DENSE|moab::MB_TAG_CREAT|
      moab::MB_TAG_EXCL|moab::MB_TAG_BYTES;
    
    // Create MOAB vertices from the coordinates
    moab::Range vertices;
    
    moab::ErrorCode err = moab_interface->create_vertices(&coordinates[0], 
							  coordinates.size()/3,
							  vertices );
    
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
    
    // Create the hexahedrons
    moab::Range hexahedrons;
    moab::EntityHandle connections[8];
    
    for( unsigned k = 0; k < d_mesh_z_dim; ++k )
    {
      for( unsigned j = 0; j < d_mesh_y_dim; ++j )
      {
	for( unsigned i = 0; i < d_mesh_x_dim; ++i )
	{
	  unsigned index = i + j*(d_mesh_x_dim+1) + 
	    k*(d_mesh_x_dim+1)*(d_mesh_y_dim+1);
	  
	  connections[0] = vertices[index];
	  connections[1] = vertices[index+1];
	  connections[2] = vertices[index+(d_mesh_x_dim+1)+1];
	  connections[3] = vertices[index+(d_mesh_x_dim+1)];
	  connections[4] = vertices[index+(d_mesh_x_dim+1)*(d_mesh_y_dim+1)];
	  connections[5] = vertices[index+(d_mesh_x_dim+1)*(d_mesh_y_dim+1)+1];
	  connections[6] = vertices[index+(d_mesh_x_dim+1)*(d_mesh_y_dim+1)+
				    (d_mesh_x_dim+1)+1];
	  connections[7] = vertices[index+(d_mesh_x_dim+1)*(d_mesh_y_dim+1)+
				    (d_mesh_x_dim+1)];
	  
	  // Create a MOAB hexahedron from the connectivity data
	  moab::EntityHandle hex;
	  
	  err = moab_interface->create_element( moab::MBHEX,
						connections,
						8,
						hex );
	  
	  TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			      MOABException, 
			      moab::ErrorCodeStr[err] );
	  
	  hexahedrons.insert( hex );
	}
      }
    }

    // Create the organ mask tags
    moab::Tag prostate_tag;
    moab::Tag urethra_tag;
    moab::Tag rectum_tag;
    moab::Tag margin_tag;
    moab::Tag normal_tag;
    moab::Tag needle_template_tag;
  
    err = moab_interface->tag_get_handle( "prostate", 
					  sizeof(int), 
					  moab::MB_TYPE_INTEGER, 
					  prostate_tag, 
					  tag_creation_flag,
					  0 );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
    
    err = moab_interface->tag_get_handle( "urethra", 
					  sizeof(int), 
					  moab::MB_TYPE_INTEGER, 
					  urethra_tag, 
					  tag_creation_flag,
					  0 );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
    
    err = moab_interface->tag_get_handle( "rectum",
					  sizeof(int),
					  moab::MB_TYPE_INTEGER,
					  rectum_tag,
					  tag_creation_flag,
					  0 );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
    
    err = moab_interface->tag_get_handle( "margin",
					  sizeof(int),
					  moab::MB_TYPE_INTEGER,
					  margin_tag,
					  tag_creation_flag,
					  0 );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
    
    err = moab_interface->tag_get_handle( "normal",
					  sizeof(int),
					  moab::MB_TYPE_INTEGER,
					  normal_tag,
					  tag_creation_flag,
					  0 );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
    
    err = moab_interface->tag_get_handle( "needle_template",
					  sizeof(int),
					  moab::MB_TYPE_INTEGER,
					  needle_template_tag,
					  tag_creation_flag,
					  0 );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );

    // Convert the boolean organ masks to integer organ masks
    std::vector<int> int_prostate_mask( d_prostate_mask.size(), 0.0 );
    std::vector<int> int_urethra_mask( d_urethra_mask.size(), 0.0 );
    std::vector<int> int_rectum_mask( d_rectum_mask.size(), 0.0 );
    std::vector<int> int_margin_mask( d_margin_mask.size(), 0.0 );
    std::vector<int> int_normal_mask( d_margin_mask.size(), 0.0 );
    
    for( unsigned k = 0; k < d_mesh_z_dim; ++k )
    {
      for( unsigned j = 0; j < d_mesh_y_dim; ++j )
      {
	for( unsigned i = 0; i < d_mesh_x_dim; ++i )
	{
	  unsigned index = i + j*d_mesh_x_dim +
	    k*d_mesh_x_dim*d_mesh_y_dim;

	  if( d_prostate_mask[index] )
	    int_prostate_mask[index] = 1;
	  else if( d_urethra_mask[index] )
	    int_urethra_mask[index] = 2;
	  else if( d_rectum_mask[index] )
	    int_rectum_mask[index] = 3;
	  else if( d_margin_mask[index] )
	  {
	    int_margin_mask[index] = 4;
	    int_normal_mask[index] = 5;
	  }
	  else
	    int_normal_mask[index] = 5;
	}
      }
    }
    
    // Tag the hexes
    err = moab_interface->tag_set_data( prostate_tag, 
					hexahedrons,
					&int_prostate_mask[0] );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );

    err = moab_interface->tag_set_data( urethra_tag,
					hexahedrons,
					&int_urethra_mask[0] );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );

    err = moab_interface->tag_set_data( rectum_tag,
    					hexahedrons,
    					&int_rectum_mask[0] );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
    			MOABException, 
    			moab::ErrorCodeStr[err] );

    err = moab_interface->tag_set_data( margin_tag,
    					hexahedrons,
    					&int_margin_mask[0] );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
    			MOABException, 
    			moab::ErrorCodeStr[err] );

    err = moab_interface->tag_set_data( normal_tag,
    					hexahedrons,
    					&int_normal_mask[0] );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
    			MOABException, 
    			moab::ErrorCodeStr[err] );

    int_prostate_mask.clear();
    int_urethra_mask.clear();
    int_rectum_mask.clear();
    int_margin_mask.clear();
    int_normal_mask.clear();

    // Create an expanded needle template
    std::vector<int> expanded_needle_template( d_prostate_mask.size() );
    for( unsigned k = 0; k < d_mesh_z_dim; ++k )
    {
      for( unsigned j = 0; j < d_mesh_y_dim; ++j )
      {
	for( unsigned i = 0; i < d_mesh_x_dim; ++i )
	{
	  unsigned template_index = i + j*d_mesh_x_dim;
	  unsigned ext_template_index = template_index + 
	    k*d_mesh_x_dim*d_mesh_y_dim;

	  if( d_needle_template[template_index] )
	    expanded_needle_template[ext_template_index] = 1;
	}
      }
    }
    
    err = moab_interface->tag_set_data( needle_template_tag,
    					hexahedrons,
    					&expanded_needle_template[0] );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
    			MOABException, 
    			moab::ErrorCodeStr[err] );

    if( export_treatment_plan )
    {
      // Create the treatment plan tags
      moab::Tag prostate_dose;
      moab::Tag urethra_dose;
      moab::Tag rectum_dose;
      moab::Tag margin_dose;
      moab::Tag normal_dose;
      moab::Tag treatment_plan;
      
      err = moab_interface->tag_get_handle( "prostate_dose",
					    sizeof(double),
					    moab::MB_TYPE_DOUBLE,
					    prostate_dose,
					    tag_creation_flag,
					    0 );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			  MOABException, 
			  moab::ErrorCodeStr[err] );
      
      err = moab_interface->tag_get_handle( "urethra_dose",
					    sizeof(double),
					    moab::MB_TYPE_DOUBLE,
					    urethra_dose,
					    tag_creation_flag,
					    0 );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			  MOABException, 
			  moab::ErrorCodeStr[err] );
      
      err = moab_interface->tag_get_handle( "rectum_dose",
					    sizeof(double),
					    moab::MB_TYPE_DOUBLE,
					    rectum_dose,
					    tag_creation_flag,
					    0 );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			  MOABException, 
			  moab::ErrorCodeStr[err] );
      
      err = moab_interface->tag_get_handle( "margin_dose",
					    sizeof(double),
					    moab::MB_TYPE_DOUBLE,
					    margin_dose,
					    tag_creation_flag,
					    0 );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			  MOABException, 
			  moab::ErrorCodeStr[err] );
      
      err = moab_interface->tag_get_handle( "normal_dose",
					    sizeof(double),
					    moab::MB_TYPE_DOUBLE,
					    normal_dose,
					    tag_creation_flag,
					    0 );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			  MOABException, 
			  moab::ErrorCodeStr[err] );
      
      err = moab_interface->tag_get_handle( "treatment_plan",
					    sizeof(int),
					    moab::MB_TYPE_INTEGER,
					    treatment_plan,
					    tag_creation_flag,
					    0 );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			  MOABException, 
			  moab::ErrorCodeStr[err] );
      
      // Calculate the organ doses
      std::vector<double> prostate_dose_data( d_prostate_mask.size() );
      std::vector<double> urethra_dose_data( d_urethra_mask.size() );
      std::vector<double> rectum_dose_data( d_rectum_mask.size() );
      std::vector<double> margin_dose_data( d_margin_mask.size() );
      std::vector<double> normal_dose_data( d_margin_mask.size() );

      for( unsigned k = 0; k < d_mesh_z_dim; ++k )
      {
	for( unsigned j = 0; j < d_mesh_y_dim; ++j )
	{
	  for( unsigned i = 0; i < d_mesh_x_dim; ++i )
	  {
	    unsigned index = i + j*d_mesh_x_dim +
	      k*d_mesh_x_dim*d_mesh_y_dim;
	    
	    if( d_prostate_mask[index] )
	      prostate_dose_data[index] = d_dose_distribution[index];
	    else if( d_urethra_mask[index] )
	      urethra_dose_data[index] = d_dose_distribution[index];
	    else if( d_rectum_mask[index] )
	      rectum_dose_data[index] = d_dose_distribution[index];
	    else if( d_margin_mask[index] )
	    {
	      margin_dose_data[index] = d_dose_distribution[index];
	      normal_dose_data[index] = d_dose_distribution[index];
	    }
	    else
	      normal_dose_data[index] = d_dose_distribution[index];
	  }
	}
      }

      err = moab_interface->tag_set_data( prostate_dose,
      					  hexahedrons,
      					  &prostate_dose_data[0] );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
      			  MOABException, 
      			  moab::ErrorCodeStr[err] );

      err = moab_interface->tag_set_data( urethra_dose,
      					  hexahedrons,
      					  &urethra_dose_data[0] );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
      			  MOABException, 
      			  moab::ErrorCodeStr[err] );

      err = moab_interface->tag_set_data( rectum_dose,
      					  hexahedrons,
      					  &rectum_dose_data[0] );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
      			  MOABException, 
      			  moab::ErrorCodeStr[err] );

      err = moab_interface->tag_set_data( margin_dose,
      					  hexahedrons,
      					  &margin_dose_data[0] );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
      			  MOABException, 
      			  moab::ErrorCodeStr[err] );
      
      err = moab_interface->tag_set_data( normal_dose,
      					  hexahedrons,
      					  &normal_dose_data[0] );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
      			  MOABException, 
      			  moab::ErrorCodeStr[err] );

      prostate_dose_data.clear();
      urethra_dose_data.clear();
      rectum_dose_data.clear();
      margin_dose_data.clear();
      normal_dose_data.clear();      
      
      // Simplify the treatment plan
      std::vector<int> treatment_plan_data( d_prostate_mask.size() );
      
      std::list<BrachytherapySeedPosition>::const_iterator position,
	end_position;

      position = d_treatment_plan.begin();
      end_position = d_treatment_plan.end();

      while( position != end_position )
      {
	int seed_type = position->getSeedType()+1;
	unsigned index = position->getXIndex() + 
	  position->getYIndex()*d_mesh_x_dim +
	  position->getZIndex()*d_mesh_x_dim*d_mesh_y_dim;
	
	treatment_plan_data[index] = seed_type;
	
	++position;
      }

      err = moab_interface->tag_set_data( treatment_plan,
      					  hexahedrons,
      					  &treatment_plan_data[0] );
      TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
      			  MOABException, 
      			  moab::ErrorCodeStr[err] );
    }

    // Create the output file name
    std::string vtk_file_name;
    size_t pos = d_patient_file_name.find(".h5");
    if( pos == std::string::npos )
      vtk_file_name = d_patient_file_name + ".vtk";
    else
      vtk_file_name = d_patient_file_name.substr( 0, pos ) + ".vtk";

    // Write the vtk file
    err = moab_interface->write_mesh( vtk_file_name.c_str() );
    TEST_FOR_EXCEPTION( err != moab::MB_SUCCESS, 
			MOABException, 
			moab::ErrorCodeStr[err] );
  }
  MOAB_EXCEPTION_CATCH_AND_EXIT();
}



} // end TPOR namespace

//---------------------------------------------------------------------------//
// end BrachytherapyPatient.cpp
//---------------------------------------------------------------------------//
