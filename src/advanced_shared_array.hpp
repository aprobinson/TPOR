#ifndef ADVANCED_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED
#define ADVANCED_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED

//shared array that can be access with iterators

#include <boost/config.hpp>   // for broken compiler workarounds

#if defined(BOOST_NO_MEMBER_TEMPLATES) && !defined(BOOST_MSVC6_MEMBER_TEMPLATES)
#include <boost/smart_ptr/detail/shared_array_nmt.hpp>
#else

#include <memory>             // TR1 cyclic inclusion fix
#include <new>

#include <boost/assert.hpp>
#include <boost/checked_delete.hpp>

#include <boost/smart_ptr/detail/shared_count.hpp>
#include <boost/detail/workaround.hpp>

#include <cstddef>            // for std::ptrdiff_t
#include <algorithm>          // for std::swap
#include <functional>         // for std::less

#include <bits/stl_iterator_base_funcs.h>
#include <bits/functexcept.h>
#include <bits/concept_check.h>
#include <initializer_list>
  
//
//  advanced_shared_array
//  advanced_shared_array adds iterator functionality to the shared_array
//

template<typename T> 
class advanced_shared_array
{
  //concept requirements
  //__glibcxx_class_requires(T, _SGIAssignableConcept)

private:
  T* _M_start;                           // first element of array
  T* _M_finish;                          // one element past last element
  T* _M_end_of_storage;                  // last element of array
  boost::detail::shared_count pn;             // reference counter
  typedef boost::checked_array_deleter<T>             deleter;
  typedef advanced_shared_array<T>                    this_type;

public:
  typedef T                                           value_type;
  typedef T*                                          pointer;
  typedef const T*                                    const_pointer;
  typedef T&                                          reference;
  typedef const T&                                    const_reference;
  typedef __gnu_cxx::__normal_iterator<pointer, advanced_shared_array> 
  iterator;
  typedef __gnu_cxx::__normal_iterator<const_pointer, advanced_shared_array>
  const_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::reverse_iterator<iterator>               reverse_iterator;
  typedef size_t                                        size_type;
  typedef ptrdiff_t                                     difference_type;

  //default constructor
  explicit advanced_shared_array() 
    : _M_start(0), _M_finish(0), _M_end_of_storage(0), pn(_M_start, deleter())
  { }

  //constructor from current array
  advanced_shared_array(T * p, size_type __n) 
    : _M_start(p), _M_finish(_M_start+__n), _M_end_of_storage(_M_finish-1),
      pn(p, deleter()) { }

  //constructor with new array creation
  //advanced_shared_array(size_type __n)
  //{
  // BOOST_ASSERT(__n > 0);
  //_M_start = new value_type[__n];
  //_M_finish = _M_start+__n;
  //_M_end_of_storage = _M_finish-1;
  //pn(_M_start, deleter());
  //}    

  //copy constructor
  advanced_shared_array(advanced_shared_array<T> const &r)
    : _M_start(r._M_start), _M_finish(r._M_finish), 
      _M_end_of_storage(r._M_end_of_storage), pn(r.pn) { }

  //assignment
  advanced_shared_array & operator=(advanced_shared_array<T> const &r)
  {
    this_type( r ).swap( *this );
    return *this;
  }
  
  void reset(T * p, size_type __n)
  {
    BOOST_ASSERT(p == 0 || p != _M_start);
    this_type(p,__n).swap(*this);
  }

  void reset()
  {
    this_type().swap(*this);
  }

  T & operator[] (std::ptrdiff_t i) const
  {
    BOOST_ASSERT(_M_start != 0);
    BOOST_ASSERT(i >= 0);
    return _M_start[i];
  }

  T * get() const
  {
    return _M_start;
  }

  // implicit conversion to "bool"
  operator bool () const
  {
    return _M_start != 0;
  }

  bool operator! () const
  {
    return _M_start == 0;
  }

  bool unique() const
  {
    return pn.unique();
  }

  long use_count() const
  {
    return pn.use_count();
  }

  void swap(advanced_shared_array<T> & other)
  {
    std::swap(_M_start,other._M_start);
    std::swap(_M_finish,other._M_finish);
    std::swap(_M_end_of_storage,other._M_end_of_storage);
    pn.swap(other.pn);
  }
  

  //return a read/write iterator that points to the first element in the
  //array. Iteration is done in ordinary element order.
  iterator begin()
  { return iterator(_M_start); }

  //return a read-only (constant) iterator that points to the first element
  // in the array. Iteration is done in ordinary element order.

  const_iterator begin() const
  { return const_iterator(_M_start); }

  //return a read/write iterator that points one past the last element in 
  //the array. Iteration is done in ordinary element order.
  iterator end()
  { return iterator(_M_finish); }

  //return a read-only (constant) iterator that points one past the last
  // element in the array. Iteration is done in ordinary element order.
  const_iterator end() const
  { return const_iterator(_M_finish); }

  //return a read/write reverse iterator that points to the last element in 
  //the array. Iteration is done in reverse element order.
  reverse_iterator rbegin()
  { return reverse_iterator(end()); }

  //return a read-only (constant) reverse iterator that points to the last
  //element in the array. Iteration is done in reverse element order.
  const_reverse_iterator rbegin() const
  { return const_reverse_iterator(end()); }

  //return a read/write reverse iterator that points to one before the
  //first element in the array. Iteration is done in reverse element order
  reverse_iterator rend()
  { return reverse_iterator(begin()); }

  //return a read-only (constant) reverse iterator that points to one before
  //the first element in the array. Iteration is done in reverse element 
  //order.
  const_reverse_iterator rend() const
  { return const_reverse_iterator(begin()); }

  //return the number of elements in the array
  size_type size() const
  { return size_type(_M_finish - _M_start); }

  bool empty() const
  { return begin() == end(); }

  //return a read/write reference to the data at the first element
  reference front()
  { return *begin(); }

  //return a read-only (constant) reference to the data at the first element
  const_reference front() const
  { return *begin(); }

  //return a read/write reference to the data at the last element
  reference back()
  { return *(end() -1); }

  //return a read-only (constant) reference to the data at the last element
  const_reference back() const
  { return *(end() -1); }
};

template<class T> inline bool operator==(advanced_shared_array<T> const & a, 
					 advanced_shared_array<T> const & b) 
// never throws
{
    return a.get() == b.get();
}

template<class T> inline bool operator!=(advanced_shared_array<T> const & a, 
					 advanced_shared_array<T> const & b) 
// never throws
{
    return a.get() != b.get();
}

template<class T> inline bool operator<(advanced_shared_array<T> const & a, 
					advanced_shared_array<T> const & b) 
// never throws
{
    return std::less<T*>()(a.get(), b.get());
}

template<class T> void swap(advanced_shared_array<T> & a,
			    advanced_shared_array<T> & b) // never throws
{
    a.swap(b);
}

#endif  // #if defined(BOOST_NO_MEMBER_TEMPLATES) && !defined(BOOST_MSVC6_MEMBER_TEMPLATES)

#endif  // #ifndef ADVANCED_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED
