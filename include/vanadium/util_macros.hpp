#ifndef INCLUDE_VANADIUM_UTIL_MACROS_HPP_
#define INCLUDE_VANADIUM_UTIL_MACROS_HPP_

#define SET_HAS(SET, ELEMENT) (SET.find(ELEMENT) != SET.end())
#define VEC_HAS(VEC, ELEMENT)                                                  \
  (std::find(VEC.begin(), VEC.end(), ELEMENT) != VEC.end())

#include <stdexcept>

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

#define TODO(msg)                                                              \
  throw std::runtime_error("TODO at line " STR(__LINE__) ": " msg)

#endif // INCLUDE_VANADIUM_UTIL_MACROS_HPP_
