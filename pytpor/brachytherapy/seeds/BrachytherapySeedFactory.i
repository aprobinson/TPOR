//---------------------------------------------------------------------------//
//!
//! \file   BrachytherapySeedFactory.i
//! \author Alex Robinson
//! \brief  SWIG interface file for the BrachytherapySeedFactory class
//!
//---------------------------------------------------------------------------//

%module factory
%include "boost_shared_ptr.i"
%shared_ptr(BrachytherapySeed)
%{
#include "BrachytherapySeedFactory.hpp"
%}

#include "BrachytherapySeedFactory.hpp"

//---------------------------------------------------------------------------//
// end BrachytherapySeedFactory.i
//---------------------------------------------------------------------------//
