//---------------------------------------------------------------------------//
//!
//! \file   ContractException.hpp
//! \author Alex Robinson
//! \brief  Design-by-Contract exception handling and error policy declaration.
//!
//---------------------------------------------------------------------------//

#ifndef CONTRACT_EXCEPTION_HPP
#define CONTRACT_EXCEPTION_HPP

// Std Lib Includes
#include <exception>
#include <string>

// Boost Includes
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

// Trilinos Includes
#include <Teuchos_TestForException.hpp>

// TPOR Includes
#include "TPOR_config.hpp"

/*! \defgroup contract_exceptions_macros Design-By-Contract Exceptions and Macros
 *
 * Design-By-Contract is a powerful way to ensure that certain function
 * preconditions and postconditions are explicit. When Design-By-Contract
 * functionality is enabled, the program will exit whenever one of these
 * conditions is not met. This functionality is made possible through a series
 * of macros and an exception class. For more information on enabling
 * Design-By-Contract, please refer to the \ref DBC page.
 */

namespace TPOR
{

//---------------------------------------------------------------------------//
// Design-By-Contract Exceptions
//---------------------------------------------------------------------------//

/*! Exception class to be thrown when function contract is not met.
 *
 * The ContractExpection class is a key part of \ref DBC.
 * \ingroup contract_exceptions_macros
 */
class ContractException : public std::logic_error
{
public:
  ContractException( const std::string &msg )
    : std::logic_error( msg )
  { /* ... */ }
  
  virtual ~ContractException() throw()
  { /* ... */ }
};

} // end TPOR namespace

//---------------------------------------------------------------------------//
// Design-by-Contract macros.
//---------------------------------------------------------------------------//
/*! \page DBC Design-by-Contract
 *
 * Design-by-Contract (DBC) functionality is provided to verify function
 * preconditions, postconditions, and invariants. These checks are separated
 * from the debug build and can be activated for both release and debug
 * builds. They can be activated by setting the following in a CMake
 * configure:
 *
 * -D TPOR_ENABLE_DBC:BOOL=ON
 *
 * By default, DBC is deactivated. Although they will require additional
 * computational overhead, these checks provide a mechanism for veryifing
 * library input arguments. Note that the bounds-checking functionality used
 * within TPOR is only provided by a debug build.
 *
 * In addition, remember is provided to store values used only for DBC
 * checks and no other place in executed code.
 *
 * Developers should refer to the \ref contract_exceptions_macros page for more
 * on how this functionality is implemented.
 *
 * Design-by-Contract functionality can also be expanded to compile-time
 * preconditions which can be very useful in templated code where the
 * the template parameter must have certain properties for the desired behavior
 * to be guaranteed. This functionality will always be turned on.
 *
 * Developers should refer to the boost/static_assert.hpp manual for more
 * information on conducting static asserts, which is what the
 * testStaticPrecondition does.
 */

#if HAVE_TPOR_DBC

/*! Test a function precondition
 * \ingroup contract_exceptions_macros 
 */
#define testPrecondition(c)						\
  TEUCHOS_TEST_FOR_EXCEPTION( !(c),					\
			      TPOR::ContractException,			\
			      "Precondition exception" << std::endl ) 

/*! Test a function postcondition
 * \ingroup contract_exceptions_macros
 */
#define testPostcondition(c)						\
  TEUCHOS_TEST_FOR_EXCEPTION( !(c),					\
			      TPOR::ContractException,			\
			      "Postcondition exception" << std::endl ) 

/*! Test a function invariant
 * \ingroup contract_exceptions_macros
 */
#define testInvariant(c)						\
  TEUCHOS_TEST_FOR_EXCEPTION( !(c),					\
                              TPOR::ContractException,			\
			      "Invariant exception" << std::endl )

/*! Remember a variable that is needed for testing DBC conditions
 * \ingroup contract_exceptions_macros
 */
#define remember(c) c

#else
			      
#define testPrecondition(c)
#define testPostcondition(c)
#define testInvariant(c)
#define remember(c)

#endif // end HAVE_TPOR_DBC

/*! test a templated function static precondition
 * \ingroup contract_exception_macros
 */
#define testStaticPrecondition(c) \
  BOOST_STATIC_ASSERT( (c) )

#endif // end CONTRACT_EXCEPTION_HPP

//---------------------------------------------------------------------------//
// end Exception.hpp
//---------------------------------------------------------------------------//
