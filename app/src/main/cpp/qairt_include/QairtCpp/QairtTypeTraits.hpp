//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once
#include <string_view>
#include <tuple>
#include <type_traits>

// Required because MISRACPP checkers sometimes fail to resolve types
#define QAIRT_NO_RETURN          (void)
#define QAIRT_TRIVIAL            static_cast<void>(0);
#define QAIRT_STATIC_ASSERT(...) static_assert((__VA_ARGS__) != 0)

namespace qairt::detail {

namespace detail {

// e.g. get_type_string<decltype(10)> = "int"
template <typename T>
inline constexpr std::string_view get_type_string() noexcept {
#ifdef _WIN32
  constexpr std::string_view sv{__FUNCSIG__}, openTag{"get_type_string<"}, closeTag{">(void)"};
#else
  constexpr std::string_view sv{static_cast<const char*>(__PRETTY_FUNCTION__)}, openTag{" = "},
      closeTag{"]"};
#endif
  constexpr auto pos = sv.find(openTag), rpos = sv.rfind(closeTag);
  static_assert(pos != std::string_view::npos && rpos != std::string_view::npos &&
                    rpos > pos + openTag.size(),
                "Tags not found in __FUNCSIG__/__PRETTY_FUNCTION__. "
                "Update openTag/closeTag for this compiler.");
  return sv.substr(pos + openTag.size(), rpos - (pos + openTag.size()));
}
}  // namespace detail

template <typename T>
inline constexpr std::string_view type_string_v = detail::get_type_string<T>();

template <typename T>
inline constexpr std::string_view type_string(T&&) {
  return type_string_v<T&&>;
}

}  // namespace qairt::detail

namespace qairt {

namespace detail {

template <typename T>
inline constexpr bool resolveBool(T&& t) noexcept {
  using U = std::decay_t<T>;
  if constexpr (std::is_same_v<U, bool>) {
    return t;
  } else if constexpr (std::is_pointer_v<U>) {
    return t == nullptr;
  } else if constexpr (std::is_arithmetic_v<U>) {
    return t != U{};
  } else {
    return static_cast<bool>(std::forward<T>(t));
  }
}

inline constexpr uint8_t convertBool(bool b) noexcept { return b ? uint8_t{1} : uint8_t{0}; }

inline int test_getter(void* handle, const char** str) {
  (void)handle;
  *str = "returned string";
  return 0;
}

inline int test_setter(void* handle, const char* str) {
  (void)std::forward_as_tuple(handle, str);
  return 0;
}

template <size_t I, typename T>
struct function_arg_type {
  using type = void;
};

template <typename T, typename... Ts>
struct is_one_of : std::disjunction<std::is_same<T, Ts>...> {};

template <typename T, typename... Ts>
inline constexpr bool is_one_of_v = is_one_of<T, Ts...>::value;

// Check if all the provided types are the same type
template <typename T, typename... Ts>
struct all_are_same : std::bool_constant<(std::is_same_v<T, Ts> && ...)> {};
template <>
struct all_are_same<void> : std::true_type {};

// Check if all the provided types are the same type
template <typename... Ts>
inline constexpr bool all_are_same_v = all_are_same<Ts...>::value;

// Tools to inspect pointers to members

template <typename T>
struct member_pointer_type_ {
  using type = void;
  QAIRT_STATIC_ASSERT(std::is_member_pointer_v<T>);
};
template <>
struct member_pointer_type_<std::nullptr_t> {
  using type = void;
};
template <typename T, typename U>
struct member_pointer_type_<T U::*> {
  using type = T;
};
template <typename T>
struct member_pointer_type : member_pointer_type_<std::remove_cv_t<T>> {};

// Given T = T_Member T_Parent::*, provide T_Member
template <typename T>
using p2m_member_type_t = typename member_pointer_type<T>::type;

// Given a pointer to member, provide the type of the pointed-to member
template <auto P2m>
using p2mv_member_type_t = p2m_member_type_t<decltype(P2m)>;

template <auto P2m>
struct p2mv_member_type {
  using type = p2mv_member_type_t<P2m>;
};

template <typename T>
struct p2m_parent_type_ {
  using type = void;
  QAIRT_STATIC_ASSERT(std::is_member_pointer_v<T>);
};
template <typename T, typename U>
struct p2m_parent_type_<T U::*> {
  using type = U;
};
template <typename T>
struct p2m_parent_type : p2m_parent_type_<std::remove_cv_t<T>> {};

// Given T = T_Member T_Parent::*, provide T_Parent
template <typename T>
using p2m_parent_type_t = typename p2m_parent_type<T>::type;

template <auto P2m>
struct p2mv_parent_type : p2m_parent_type<decltype(P2m)> {};
// Given a pointer to member, provide the type the pointer points into
template <auto P2m>
using p2mv_parent_type_t = p2m_parent_type_t<decltype(P2m)>;

template <auto P2m>
inline constexpr bool points_to_pointer_v = std::is_pointer_v<p2m_member_type_t<decltype(P2m)>>;
template <auto P2m>
struct points_to_pointer : std::bool_constant<points_to_pointer_v<P2m>> {};

template <auto A, auto B>
struct p2mv_are_equal {
  static constexpr bool get_value() {
    if constexpr (std::is_same_v<decltype(A), decltype(B)>) {
      return A == B;
    } else {
      return false;
    }
  }
  static constexpr bool value = get_value();
};

template <auto A, auto B>
inline constexpr bool p2mv_are_equal_v = p2mv_are_equal<A, B>::value;

template <size_t I, typename T_Ret, typename... T_Args>
struct function_arg_type<I, T_Ret(T_Args...)> {
  using type = std::tuple_element_t<I, std::tuple<T_Args...>>;
};

template <size_t I, typename T>
using function_arg_type_t = typename function_arg_type<I, T>::type;

//
template <typename T>
struct invalid_type {
  template <typename U>
  static inline constexpr bool dependent_false = std::is_same_v<U, void************>;
  QAIRT_STATIC_ASSERT(dependent_false<T>);
  using type = void;
};

template <typename T>
using invalid_type_t = typename invalid_type<T>::type;

template <typename T>
struct getter_out_type {
  using type = invalid_type_t<T>;
  //  using type = void;
};

template <typename T, typename U, typename V>
struct getter_out_type<T(U, V)> {
  using type = std::remove_pointer_t<V>;
};
template <typename T, typename U, typename... Vs>
struct getter_out_type<T(U, Vs...)> {
  static_assert(sizeof...(Vs) > 1);
  using type = std::tuple<std::remove_pointer_t<Vs>...>;
};

template <typename T, typename U, typename... Vs>
struct getter_out_type<T (*)(U, Vs...)> : getter_out_type<T(U, Vs...)> {};

template <typename... Ts>
using getter_out_type_t = typename getter_out_type<Ts...>::type;

template <auto P2m>
struct getterv_out_type : getter_out_type<decltype(P2m)> {};

template <auto P2m>
using getterv_out_type_t = getter_out_type_t<decltype(P2m)>;

template <typename T>
struct setter_in_type {
  using type = invalid_type_t<T>;
};
template <>
struct setter_in_type<std::nullptr_t> {
  using type = std::nullptr_t;
};
template <>
struct setter_in_type<void> {
  using type = std::nullptr_t;
};

template <typename T, typename U, typename V>
struct setter_in_type<T(U, V)> {
  using type = V;
};
template <typename T, typename U, typename... Vs>
struct setter_in_type<T(U, Vs...)> {
  static_assert(sizeof...(Vs) > 1);
  using type = std::tuple<Vs...>;
};

template <typename T, typename U, typename... Vs>
struct setter_in_type<T (*)(U, Vs...)> : setter_in_type<T(U, Vs...)> {};

template <typename... Ts>
using setter_in_type_t = typename setter_in_type<Ts...>::type;

template <auto P2m>
struct setterv_out_type : setter_in_type<decltype(P2m)> {};

template <auto P2m>
using setterv_out_type_t = setter_in_type_t<decltype(P2m)>;

template <typename Getter, typename Setter>
struct getter_matches_setter
    : std::bool_constant<std::is_same_v<getter_out_type_t<Getter>, setter_in_type_t<Setter>>> {};

template <typename Getter, typename Setter>
inline constexpr bool getter_matches_setter_v = getter_matches_setter<Getter, Setter>::value;

template <auto P2mGetter, auto P2mSetter>
struct getterv_matches_setterv : getter_matches_setter<decltype(P2mGetter), decltype(P2mSetter)> {};

template <auto P2mGetter, auto P2mSetter>
inline constexpr bool getterv_matches_setterv_v =
    getterv_matches_setterv<P2mGetter, P2mSetter>::value;

static_assert(getter_matches_setter_v<decltype(test_getter), decltype(test_setter)>);

template <typename T>
struct getter_gets_fundamental : std::bool_constant<std::is_fundamental_v<getter_out_type_t<T>>> {};

// input params
template <typename T_P2m>
struct p2m_getter_type : getter_out_type<p2m_member_type_t<T_P2m>> {};
template <auto P2m>
struct p2mv_getter_type : p2m_getter_type<decltype(P2m)> {};

template <typename T_P2m>
using p2m_getter_type_t = typename p2m_getter_type<T_P2m>::type;
template <auto P2m>
using p2mv_getter_type_t = typename p2mv_getter_type<P2m>::type;

template <typename T_P2m>
struct p2m_setter_type : setter_in_type<p2m_member_type_t<T_P2m>> {};
template <auto P2m>
struct p2mv_setter_type : p2m_setter_type<decltype(P2m)> {};

template <typename T_P2m>
using p2m_setter_type_t = typename p2m_setter_type<T_P2m>::type;
template <auto P2m>
using p2mv_setter_type_t = typename p2mv_setter_type<P2m>::type;

template <auto P2m_Setter>
struct is_vector_setter {
  static constexpr bool value =
      std::is_same_v<std::tuple_element_t<1, p2mv_setter_type_t<P2m_Setter>>, uint32_t> &&
      std::is_pointer_v<std::tuple_element_t<0, p2mv_setter_type_t<P2m_Setter>>>;
};

template <>
struct is_vector_setter<nullptr> {
  static constexpr bool value = true;
};

template <auto P2m_Setter>
inline constexpr bool is_vector_setter_v = is_vector_setter<P2m_Setter>::value;

// Figure out the param type even if the getter is nullptr
template <auto P2mGetter, auto>
struct p2mv_getset_type {
  using type = getter_out_type_t<p2mv_member_type_t<P2mGetter>>;
};

template <auto P2mSetter>
struct p2mv_getset_type<nullptr, P2mSetter> {
  using type = setter_in_type_t<p2mv_member_type_t<P2mSetter>>;
};

template <auto P2mGetter, auto P2mSetter>
using p2mv_getset_type_t = typename p2mv_getset_type<P2mGetter, P2mSetter>::type;

// Check if a given type is a tuple
template <typename... Ts>
struct is_tuple : std::false_type {};

template <typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template <typename... Ts>
inline constexpr bool is_tuple_v = is_tuple<Ts...>::value;

template <typename T>
struct tuple_size : std::integral_constant<size_t, 1> {};

template <typename... Ts>
struct tuple_size<std::tuple<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};

template <typename T, typename U>
struct tuple_size<std::pair<T, U>> : std::integral_constant<size_t, 2> {};

template <typename... Ts>
inline constexpr size_t tuple_size_v = tuple_size<Ts...>::value;

template <typename T>
struct tuple_as_pair {
  using type = std::pair<void, void>;
};

template <typename T, typename U>
struct tuple_as_pair<std::tuple<T, U>> {
  using type = std::pair<T, U>;
};

template <typename T>
using tuple_as_pair_t = typename tuple_as_pair<T>::type;

template <typename T>
struct tuple_as_ref_pair {
  using type = std::pair<void, void>;
};

template <typename T, typename U>
struct tuple_as_ref_pair<std::tuple<T, U>> {
  using type = std::pair<T&, U&>;
};

template <typename T>
using tuple_as_ref_pair_t = typename tuple_as_ref_pair<T>::type;

// Allow nullptr and just resolve to void
template <auto P2m>
struct p2mv_parent_type_an : p2mv_parent_type<P2m> {};

template <>
struct p2mv_parent_type_an<nullptr> {
  using type = void;
};

template <auto P2m>
using p2mv_parent_type_an_t = typename p2mv_parent_type_an<P2m>::type;

template <auto P2m>
struct p2mv_member_type_an : p2mv_member_type<P2m> {};

template <>
struct p2mv_member_type_an<nullptr> {
  using type = void;
};

template <auto P2m>
using p2mv_member_type_an_t = typename p2mv_member_type_an<P2m>::type;

template <typename T, typename...>
struct first_type {
  using type = T;
};
template <typename T, typename...>
using first_type_t = T;

template <auto V, auto...>
struct first_param {
  static constexpr auto value = V;
};
template <auto V, auto...>
inline constexpr auto first_param_v = V;

// Check if a type has a uint64_t size member
template <typename, typename = void>
struct has_size_member : std::false_type {};

template <typename T>
struct has_size_member<T, std::void_t<decltype(&T::size)>>
    : std::is_same<p2mv_member_type_t<&T::size>, uint64_t> {};

template <typename T>
inline constexpr bool has_size_member_v = has_size_member<T>::value;

template <typename, typename = void>
struct has_create_member : std::false_type {};

template <typename T>
struct has_create_member<T, std::void_t<decltype(&T::create)>> : std::true_type {};

template <typename T>
inline constexpr bool has_create_member_v = has_create_member<T>::value;

template <typename, typename = void>
struct has_free_member : std::false_type {};

template <typename T>
struct has_free_member<T, std::void_t<decltype(&T::free)>> : std::true_type {};

template <typename T>
inline constexpr bool has_free_member_v = has_free_member<T>::value;

template <auto P2m>
struct p2mv_is_free_member {
  using parent_type = p2mv_parent_type_t<P2m>;

  // We cannot compare two p2ms with different types
  inline static constexpr bool get_value() {
    if constexpr (has_free_member_v<parent_type>) {
      if constexpr (std::is_same_v<decltype(&parent_type::free), decltype(P2m)>) {
        return &parent_type::free == P2m;
      }
    }
    return false;
  }

  inline static constexpr bool value = get_value();
};

template <>
struct p2mv_is_free_member<nullptr> : std::false_type {};

// Check if a P2m points to a member named "free"
template <auto P2m>
inline constexpr bool p2mv_is_free_member_v = p2mv_is_free_member<P2m>::value;

// Check if a type has a handle_type typedef
template <typename, typename = void>
struct has_handle_type : std::false_type {};

template <typename T>
struct has_handle_type<T, std::void_t<typename T::handle_type>> : std::true_type {};

template <typename T>
inline constexpr bool has_handle_type_v = has_handle_type<T>::value;

namespace tests {

struct has_size {
  uint64_t size;
  uint64_t (*func)(void*, int);
};
struct has_no_size {
  uint64_t (*func)(void*, int);
};
template <typename T_Size>
struct has_other_size {
  T_Size size;
  uint64_t (*func)(void*, int);
};
static_assert(has_size_member_v<has_size>);
static_assert(!has_size_member_v<has_no_size>);
static_assert(!has_size_member_v<has_other_size<int32_t>>);
static_assert(!has_size_member_v<has_other_size<uint32_t>>);
static_assert(!has_size_member_v<has_other_size<double>>);

struct has_free_and_create {
  uint64_t size;
  uint64_t (*create)(void**);
  uint64_t (*free)(void*);
};

static_assert(has_size_member_v<has_free_and_create>);
static_assert(has_create_member_v<has_free_and_create>);
static_assert(has_free_member_v<has_free_and_create>);

static_assert(!has_create_member_v<has_size>);
static_assert(!has_free_member_v<has_size>);

}  // namespace tests

}  // namespace detail
}  // namespace qairt
