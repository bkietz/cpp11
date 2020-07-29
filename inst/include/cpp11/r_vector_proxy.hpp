#pragma once

#include <stddef.h>                   // for ptrdiff_t, size_t
#include <algorithm>                  // for max
#include <array>                      // for array
#include <cstdio>                     // for snprintf
#include <exception>                  // for exception
#include <initializer_list>           // for initializer_list
#include <iterator>                   // for forward_iterator_tag, random_ac...
#include <stdexcept>                  // for out_of_range
#include <string>                     // for string, basic_string
#include <type_traits>                // for decay, is_same, enable_if, is_c...
#include <utility>                    // for declval
#include "cpp11/R.hpp"                // for R_xlen_t, SEXP, SEXPREC, Rf_xle...
#include "cpp11/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp11/protect.hpp"          // for protect_sexp, release_protect
#include "cpp11/r_string.hpp"         // for r_string
#include "cpp11/sexp.hpp"             // for sexp

namespace cpp11 {

namespace writable {

// specialized in each header file
// must provide implementations of:
// operator=(T)
// operator T()
template <typename T>
class r_vector_proxy;

template <typename T>
struct r_vector_proxy_base {
  const SEXP data_;
  const R_xlen_t index_;
  T* const p_;
  bool is_altrep_;

  r_vector_proxy_base(SEXP data, const R_xlen_t index, T* const p, bool is_altrep)
      : data_(data), index_(index), p_(p), is_altrep_(is_altrep) {}
};

// mixin with arithmetic operators
template <typename T>
struct r_vector_proxy_arithmetic {
  using proxy = r_vector_proxy<T>;

  proxy& operator+=(const T& rhs) {
    operator=(static_cast<T>(static_cast<proxy&>(*this)) + rhs);
    return *this;
  }
  proxy& operator-=(const T& rhs);
  proxy& operator*=(const T& rhs);
  proxy& operator/=(const T& rhs);

  proxy& operator++(int);
  proxy& operator--(int);
  void operator++();
  void operator--();
};

}  // namespace writable
}  // namespace cpp11
