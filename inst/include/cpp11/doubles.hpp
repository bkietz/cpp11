#pragma once

#include <algorithm>            // for min
#include <array>                // for array
#include <initializer_list>     // for initializer_list
#include "R_ext/Arith.h"        // for ISNA
#include "cpp11/R.hpp"          // for SEXP, SEXPREC, Rf_allocVector, REAL
#include "cpp11/as.hpp"         // for as_sexp
#include "cpp11/named_arg.hpp"  // for named_arg
#include "cpp11/protect.hpp"    // for SEXP, SEXPREC, REAL_ELT, R_Preserve...
#include "cpp11/r_vector.hpp"   // for vector, vector<>::proxy, vector<>::...
#include "cpp11/sexp.hpp"       // for sexp
// Specializations for doubles

namespace cpp11 {

template <>
inline SEXP r_vector<double>::valid_type(SEXP data) {
  if (TYPEOF(data) != REALSXP) {
    throw type_error(REALSXP, TYPEOF(data));
  }
  return data;
}

template <>
inline const double r_vector<double>::operator[](const R_xlen_t pos) const {
  // NOPROTECT: likely too costly to unwind protect every elt
  return is_altrep_ ? REAL_ELT(data_, pos) : data_p_[pos];
}

template <>
inline double* r_vector<double>::get_p(bool is_altrep, SEXP data) {
  if (is_altrep) {
    return nullptr;
  } else {
    return REAL(data);
  }
}

template <>
inline void r_vector<double>::const_iterator::fill_buf(R_xlen_t pos) {
  length_ = std::min(64_xl, data_->size() - pos);
  REAL_GET_REGION(data_->data_, pos, length_, buf_.data());
  block_start_ = pos;
}

typedef r_vector<double> doubles;

namespace writable {

template <>
class r_vector_proxy<double> : protected r_vector_proxy_base<double>,
                               // import operators as needed
                               public r_vector_proxy_arithmetic<double> {
 public:
  using proxy = r_vector_proxy<double>;

  proxy& operator=(double b) {
    // double specific assignment
    auto& self = static_cast<proxy&>(*this);
    if (self.is_altrep_) {
      // NOPROTECT: likely too costly to unwind protect every set elt
      SET_REAL_ELT(self.data_, self.index_, rhs);
    } else {
      *self.p_ = rhs;
    }
    return self;
  }
  operator double() const {
    // double specific access
    auto& self = static_cast<const proxy&>(*this);
    if (self.p_ == nullptr) {
      // NOPROTECT: likely too costly to unwind protect every elt
      return REAL_ELT(self.data_, self.index_);
    } else {
      return *self.p_;
    }
  }
};

template <>
inline r_vector<double>::r_vector(std::initializer_list<double> il)
    : cpp11::r_vector<double>(as_sexp(il)), capacity_(il.size()) {}

template <>
inline r_vector<double>::r_vector(std::initializer_list<named_arg> il)
    : cpp11::r_vector<double>(safe[Rf_allocVector](REALSXP, il.size())),
      capacity_(il.size()) {
  try {
    unwind_protect([&] {
      protect_ = protect_sexp(data_);
      Rf_setAttrib(data_, R_NamesSymbol, Rf_allocVector(STRSXP, capacity_));
      sexp names(Rf_getAttrib(data_, R_NamesSymbol));
      auto it = il.begin();
      for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
        data_p_[i] = doubles(it->value())[0];
        SET_STRING_ELT(names, i, Rf_mkCharCE(it->name(), CE_UTF8));
      }
    });
  } catch (const unwind_exception& e) {
    release_protect(protect_);
    throw e;
  }
}

template <>
inline void r_vector<double>::reserve(R_xlen_t new_capacity) {
  data_ = data_ == R_NilValue ? safe[Rf_allocVector](REALSXP, new_capacity)
                              : safe[Rf_xlengthgets](data_, new_capacity);
  SEXP old_protect = protect_;
  protect_ = protect_sexp(data_);
  release_protect(old_protect);

  data_p_ = REAL(data_);
  capacity_ = new_capacity;
}

template <>
inline void r_vector<double>::push_back(double value) {
  while (length_ >= capacity_) {
    reserve(capacity_ == 0 ? 1 : capacity_ *= 2);
  }
  if (is_altrep_) {
    SET_REAL_ELT(data_, length_, value);
  } else {
    data_p_[length_] = value;
  }
  ++length_;
}

typedef r_vector<double> doubles;

}  // namespace writable

inline bool is_na(double x) { return ISNA(x); }
}  // namespace cpp11
