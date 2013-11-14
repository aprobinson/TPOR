//---------------------------------------------------------------------------//
//!
//! \file   Interpolation.hpp
//! \author Alex Robinson
//! \brief  Interpolation function declaration.
//!
//---------------------------------------------------------------------------//

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

namespace TPOR{

//! Linear-linear interpolation function
double linlinInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 );

//! Log-linear interpolation function
double loglinInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 );

//! Linear-log interpolation function
double linlogInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 );

//! Log-log interpolation function
double loglogInterp( const double x0,
		     const double x1,
		     const double x,
		     const double y0,
		     const double y1 );

} // end TPOR namespace

#endif // end INTERPOLATION_HPP

//---------------------------------------------------------------------------//
// end Interpolation.hpp
//---------------------------------------------------------------------------//
