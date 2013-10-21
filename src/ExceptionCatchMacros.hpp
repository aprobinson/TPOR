//---------------------------------------------------------------------------//
//!
//! \file   ExceptionCatchMacros.hpp
//! \author Alex Robinson
//! \brief  Macros for catching exceptions
//!
//---------------------------------------------------------------------------//

#ifndef EXCEPTION_CATCH_MACROS_HPP
#define EXCEPTION_CATCH_MACROS_HPP

// Std Lib Includes
#include <stdexcept>
#include <iostream>
#include <sstream>

// Trilinos Includes
#include "Teuchos_FancyOStream.hpp"
#include "Teuchos_TestForException.hpp"

// TPOR Includes
#include "MOABException.hpp"

/*! \defgroup exception_macros Exception Catch Macros
 *
 * These macros are design to catch certain types of exceptions, print
 * out error messages with the file name and line where the exception
 * occurred and then exit the program.
 */

/*! Catch statement macro for catching std::exception exceptions
 *
 * This macro is based off of the Teuchos_StandardCatchMacro. This macro
 * should be used anywhere that a std::exception is thrown in order to
 * properly document the exception and exit the program.
 * \ingroup exception_macros
 */
#define STD_EXCEPTION_CATCH_AND_EXIT()			    \
  catch( const std::exception &exception )		    \
    {							    \
    std::ostringstream oss;				    \
    oss << " *** Caught std::exception Exception *** \n\n"; \
    oss << "File: " << __FILE__ << "\n";		    \
    oss << "Line: " << __LINE__ << "\n";		    \
    Teuchos::OSTab scsi_tab(oss);			    \
    scsi_tab.o() << exception.what() << "\n";		    \
    std::cerr << std::flush;				    \
    std::cerr << oss.str();				    \
    exit(EXIT_FAILURE);					    \
    }							    \

/*! Catch statement macro for catching HDF5 H5::Exception exceptions
 * 
 * This macro is based off of the Teuchos_StandardCatchMacro. The only class
 * that handles the HDF5 interface is the TPOR::HDF5FileHandler. This
 * macro will only appear in that class. The HDF5 library can throw a
 * variety of H5:Exception classes. This macro will be used after any
 * HDF5 library call that can throw one of these exceptions.
 * \ingroup exception_macros
 */
#define HDF5_EXCEPTION_CATCH_AND_EXIT()			\
  catch( const H5::Exception &exception )		\
    {							\
    std::ostringstream oss;				\
    oss << " *** Caught HDF5 H5::Exception *** \n\n";	\
    oss << "File: " << __FILE__ << "\n";		\
    oss << "Line: " << __LINE__ << "\n";		\
    Teuchos::OSTab scsi_tab(oss);			\
    scsi_tab.o() << exception.getFuncName() << "\n";	\
    scsi_tab.o() << exception.getDetailMsg() << "\n";	\
    std::cerr << std::flush;				\
    std::cerr << oss.str();				\
    exit(EXIT_FAILURE);					\
  } \

/*! Catch statement macro for catching TPOR::MOABException exceptions
 *
 * This macro is based off of the Teuchos_StandardCatchMacro. This macro
 * should be used anywhere that a TPOR::MOABException is thrown in order to
 * properly document the exception and exit the program.
 * \ingroup exception_macros
 */
#define MOAB_EXCEPTION_CATCH_AND_EXIT()				 \
  catch( const TPOR::MOABException &exception )			 \
    {								 \
    std::ostringstream oss;					 \
    oss << " *** Caught TPOR::MOABException Exception *** \n\n"; \
    oss << "File: " << __FILE__ << "\n";			 \
    oss << "Line: " << __LINE__ << "\n";			 \
    Teuchos::OSTab scsi_tab(oss);				 \
    scsi_tab.o() << exception.what() << "\n";			 \
    std::cerr << std::flush;					 \
    std::cerr << oss.str();					 \
    exit(EXIT_FAILURE);						 \
  }								 \

#endif // end EXCEPTION_CATCH_MACROS_HPP

//---------------------------------------------------------------------------//
// end ExceptionCatchMacros.hpp
//---------------------------------------------------------------------------//
