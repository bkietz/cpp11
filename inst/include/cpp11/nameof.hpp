#pragma once

#include <string>

namespace cpp11 {
namespace detail {

#ifdef _MSC_VER
#define CPP11_PRETTY_FUNCTION __FUNCSIG__
#else
#define CPP11_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

template <typename T>
constexpr const char* raw() {
  return CPP11_PRETTY_FUNCTION;
}

template <typename T>
constexpr size_t raw_sizeof() {
  return sizeof(CPP11_PRETTY_FUNCTION);
}

#undef CPP11_PRETTY_FUNCTION

constexpr bool starts_with(char const* haystack, char const* needle) {
  return needle[0] == '\0' ||
         (haystack[0] == needle[0] && starts_with(haystack + 1, needle + 1));
}

constexpr std::size_t search(char const* haystack, char const* needle) {
  return haystack[0] == '\0' || starts_with(haystack, needle)
             ? 0
             : search(haystack + 1, needle) + 1;
}

constexpr auto typename_prefix = search(raw<void>(), "void");

template <typename T>
constexpr size_t struct_class_prefix() {
#ifdef _MSC_VER
  return starts_with(raw<T>() + typename_prefix, "struct ")
             ? 7
             : starts_with(raw<T>() + typename_prefix, "class ") ? 6 : 0;
#else
  return 0;
#endif
}

template <typename T>
constexpr size_t typename_length() {
  // raw_sizeof<T>() - raw_sizeof<void>() == (length of T's name) - strlen("void")
  // (length of T's name) == raw_sizeof<T>() - raw_sizeof<void>() + strlen("void")
  return raw_sizeof<T>() - raw_sizeof<void>() + 4;
}

template <typename T>
constexpr const char* typename_begin() {
  return raw<T>() + struct_class_prefix<T>() + typename_prefix;
}

}  // namespace detail

template <typename T>
std::string nameof() {
  return {detail::typename_begin<T>(), detail::typename_length<T>()};
}

}  // namespace cpp11
