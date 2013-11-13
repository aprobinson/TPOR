//---------------------------------------------------------------------------//
//!
//! \file   Interpolation_def.hpp
//! \author Alex Robinson
//! \brief  Interpolation function definition.
//!
//---------------------------------------------------------------------------//

#ifndef INTERPOLATION_DEF_HPP
#define INTERPOLATION_DEF_HPP

// TPOR Includes
#include "ContractException.hpp"

namespace TPOR{

// Interpolation function
template<typename InterpolationPolicy>
double Interpolate( const double lower_indep_value,
		    const double upper_indep_value,
		    const double inner_indep_value,
		    const double lower_dep_value,
		    const double upper_dep_value )
{
  // Make sure the independent variable range is valid
  testPrecondition( upper_indep_value > lower_indep_value );
  // Make sure the dependent variable range is valie
  testPrecondition( upper_dep_value > lower_dep_value );
  // Make sure that the inner independent value is valid
  testPrecondition( inner_indep_value >= lower_indep_value );
  testPrecondition( inner_indep_value <= upper_indep_value );
  
  double processed_lower_indep_value = 
    InterpolationPolicy::processIndependentVar( lower_indep_value );
  double processed_upper_indep_value = 
    InterpolationPolicy::processIndependentVar( upper_indep_value );
  double processed_inner_indep_value = 
    InterpolationPolicy::processIndependentVar( inner_indep_value );
  double processed_lower_dep_value = 
    InterpolationPolicy::processDependentVar( lower_dep_value );
  double processed_upper_dep_value =
    InterpolationPolicy::processDependentVar( upper_dep_value );

  double slope = (processed_upper_dep_value - processed_lower_dep_value)/
    (processed_upper_indep_value - processed_lower_indep_value);
  double interpolated_value = processed_lower_dep_value + 
    slope*(processed_inner_indep_value - processed_lower_indep_value);

  return InterpolationPolicy::processInterpolatedValue( interpolated_value );  
}

} // end TPOR namespace

#endif // end INTERPOLATION_DEF_HPP

//---------------------------------------------------------------------------//
// end Interpolation_def.hpp
//---------------------------------------------------------------------------//
