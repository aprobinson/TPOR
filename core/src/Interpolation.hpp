//---------------------------------------------------------------------------//
//!
//! \file   Interpolation.hpp
//! \author Alex Robinson
//! \brief  Interpolation function declaration.
//!
//---------------------------------------------------------------------------//

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

// Std Lib Includes
#include <math.h>

namespace TPOR{

//! Linear-Linear interpolation policy
struct LinLin
{
  //! Process independent variable
  static inline double processIndependentVar( const double indep_var )
  { return indep_var; }

  //! Process the dependent variable
  static inline double processDependentVar( const double dep_var )
  { return dep_var; }

  //! Process the interpolated value
  static inline double processInterpolatedValue( const double interp_value )
  { return interp_value; }
}

//! Log-Linear interpolation policy
struct LogLin
{
  //! Process independent variable
  static inline double processIndependentVar( const double indep_var )
  { return log(indep_var); }

  //! Process the dependent variable
  static inline double processDependentVar( const double dep_var )
  { return dep_var; }

  //! Process the interpolated value
  static inline double processInterpolatedValue( const double interp_value )
  { return interp_value; }
}

//! Linear-Log interpolation policy
struct LinLog
{
  //! Process independent variable
  static inline double processIndependentVar( const double indep_var )
  { return indep_var; }

  //! Process the dependent variable
  static inline double processDependentVar( const double dep_var )
  { return log(dep_var); }

  //! Process the interpolated value
  static inline double processInterpolatedValue( const double interp_value )
  { return exp(interp_value); }
}

//! Log-Log interpolation policy
struct LogLog
{
  //! Process independent variable
  static inline double processIndependentVar( const double indep_var )
  { return log(indep_var); }

  //! Process the dependent variable
  static inline double processDependentVar( const double dep_var )
  { return log(dep_var); }

  //! Process the interpolated value
  static inline double processInterpolatedValue( const double interp_value )
  { return exp(interp_value); }
}

//! Interpolation function
template<typename InterpolationPolicy>
double Interpolate( const double lower_indep_value,
		    const double upper_indep_value,
		    const double inner_indep_value,
		    const double lower_dep_value,
		    const double upper_dep_value );

} // end TPOR namespace

//---------------------------------------------------------------------------//
// Template includes.
//---------------------------------------------------------------------------//

#include "Interpolation_def.hpp"

//---------------------------------------------------------------------------//

#endif // end INTERPOLATION_HPP

//---------------------------------------------------------------------------//
// end Interpolation.hpp
//---------------------------------------------------------------------------//
