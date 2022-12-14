///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_RESULT_HPP
#define ZPP_INCLUDE_ZPP_RESULT_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <zpp/error_code.hpp>

#include <utility>

namespace zpp {

////////////////////////////////////////////////////////////////////////////////
///
/// @brief helper class for error result
///
////////////////////////////////////////////////////////////////////////////////
template<typename T_Error>
class error_result {
  static_assert(!std::is_void_v<T_Error>);
  static_assert(std::is_copy_constructible_v<T_Error>);
  static_assert(std::is_move_constructible_v<T_Error>);
  static_assert(std::is_copy_assignable_v<T_Error>);
  static_assert(std::is_move_assignable_v<T_Error>);
public:
  error_result() noexcept = delete;

  error_result(error_result&&) noexcept = default;
  error_result(const error_result&) noexcept = default;
  error_result& operator=(error_result&&) noexcept = default;
  error_result& operator=(const error_result&) noexcept = default;

  ///
  /// @brief initialize error_result
  ///
  /// @param rhs the value the use for initialization
  ///
  explicit error_result(const T_Error& rhs) noexcept
    : m_error(rhs)
  {
  }

  ///
  /// @brief initialize error_result
  ///
  /// @param rhs the value the use for initialization
  ///
  explicit error_result(T_Error&& rhs) noexcept
    : m_error(std::move(rhs))
  {
  }

  ///
  /// @brief return error value
  ///
  /// @return the error value as a reference
  ///
  T_Error& error() noexcept {
    return m_error;
  }

  ///
  /// @brief return error value
  ///
  /// @return the error value as a const reference
  ///
  const T_Error& error() const noexcept {
    return m_error;
  }
private:
  T_Error m_error;
};

////////////////////////////////////////////////////////////////////////////////
///
/// @brief result class
///
/// @param T_Ok the type for the OK result
/// @param T_Error the type for the error result
///
////////////////////////////////////////////////////////////////////////////////
template<typename T_Ok, typename T_Error>
class result {
  static_assert(!std::is_void_v<T_Ok>);
  static_assert(std::is_copy_constructible_v<T_Ok>);
  static_assert(std::is_move_constructible_v<T_Ok>);
  static_assert(std::is_copy_assignable_v<T_Ok>);
  static_assert(std::is_move_assignable_v<T_Ok>);

  static_assert(!std::is_void_v<T_Error>);
  static_assert(std::is_copy_constructible_v<T_Error>);
  static_assert(std::is_move_constructible_v<T_Error>);
  static_assert(std::is_copy_assignable_v<T_Error>);
  static_assert(std::is_move_assignable_v<T_Error>);
public:
  ///
  /// @brief default initialization to error state
  ///
  result() noexcept
    : m_is_ok(false)
  {
    new(&m_error_value) T_Error();
  }

  ///
  /// @brief initialization to OK state
  ///
  /// @param rhs the OK value to assign
  ///
  result(const T_Ok& rhs) noexcept
    : m_is_ok(true)
  {
    new(&m_ok_value) T_Ok(rhs);
  }

  ///
  /// @brief initialization to OK state
  ///
  /// @param rhs the OK value to assign
  ///
  result(T_Ok&& rhs) noexcept
    : m_is_ok(true)
  {
    new(&m_ok_value) T_Ok(std::move(rhs));
  }

  ///
  /// @brief initialization to error state
  ///
  /// @param rhs the error value to assign
  ///
  result(const error_result<T_Error>& rhs) noexcept
    : m_is_ok(false)
  {
    new(&m_error_value) T_Error(rhs.error());
  }


  ///
  /// @brief initialization to error state
  ///
  /// @param rhs the error value to assign
  ///
  result(error_result<T_Error>&& rhs) noexcept
    : m_is_ok(false)
  {
    new(&m_error_value) T_Error(std::move(rhs.error()));
  }

  ///
  /// @brief copy contructor
  ///
  /// @param rhs the value to assign
  ///
  result(const result& rhs) noexcept
    : m_is_ok(rhs.m_is_ok)
  {
    if (m_is_ok) {
      new(&m_ok_value) T_Ok(rhs.m_ok_value);
    } else {
      new(&m_error_value) T_Error(rhs.m_error_value);
    }
  }

  ///
  /// @brief move contructor
  ///
  /// @param rhs the value to assign
  ///
  result(result&& rhs) noexcept
    : m_is_ok(rhs.m_is_ok)
  {
    if (m_is_ok) {
      new(&m_ok_value) T_Ok(std::move(rhs.m_ok_value));
    } else {
      new(&m_error_value) T_Error(std::move(rhs.m_error_value));
    }
  }

  ///
  /// @brief destructor
  ///
  ~result() noexcept {
    if (m_is_ok) {
      m_ok_value.~T_Ok();
    } else {
      m_error_value.~T_Error();
    }
  }

  void assign_value(const T_Ok& v) noexcept {
    if (m_is_ok) {
      m_ok_value.~T_Ok();
    } else {
      m_error_value.~T_Error();
    }

    m_is_ok = true;
    new(&m_ok_value) T_Ok(v);
  }

  void assign_value(T_Ok&& v) noexcept {
    if (m_is_ok) {
      m_ok_value.~T_Ok();
    } else {
      m_error_value.~T_Error();
    }

    m_is_ok = true;
    new(&m_ok_value) T_Ok(std::move(v));
  }

  void assign_error(const T_Error& e) noexcept {
    if (m_is_ok) {
      m_ok_value.~T_Ok();
    } else {
      m_error_value.~T_Error();
    }

    m_is_ok = false;
    new(&m_error_value) T_Error(e);
  }

  void assign_error(T_Error&& e) noexcept {
    if (m_is_ok) {
      m_ok_value.~T_Ok();
    } else {
      m_error_value.~T_Error();
    }

    m_is_ok = false;
    new(&m_error_value) T_Error(std::move(e));
  }


  ///
  /// @brief copy operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(const result& rhs) noexcept
  {
    if (rhs.m_is_ok) {
      assign_value(rhs.m_ok_value);
    } else {
      assign_error(rhs.m_error_value);
    }

    return *this;
  }

  ///
  /// @brief move operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(result&& rhs) noexcept
  {
    if (rhs.m_is_ok) {
      assign_value(std::move(rhs.m_ok_value));
    } else {
      assign_error(std::move(rhs.m_error_value));
    }

    return *this;
  }


  ///
  /// @brief copy operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(const T_Ok& rhs) noexcept
  {
    assign_value(rhs);
    return *this;
  }

  ///
  /// @brief move operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(T_Ok&& rhs) noexcept
  {
    assign_value(std::move(rhs));
    return *this;
  }

  ///
  /// @brief copy operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(const error_result<T_Error>& rhs) noexcept
  {
    assign_error(rhs.error());
    return *this;
  }

  ///
  /// @brief move operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(error_result<T_Error>&& rhs) noexcept
  {
    assign_error(std::move(rhs.error()));
    return *this;
  }


  ///
  /// @brief return a reference to the OK value
  ///
  /// @return T_Ok reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  T_Ok& value() noexcept {
    if (!m_is_ok) {
      // unhandeld error
    }

    return m_ok_value;
  }

  ///
  /// @brief return a const reference to the result value
  ///
  /// @return T_Ok const reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  const T_Ok& value() const noexcept {
    if (!m_is_ok) {
      // unhandeld error
    }

    return m_ok_value;
  }


  ///
  /// @brief return a reference to the OK value
  ///
  /// @return T_Ok reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  T_Error& error() noexcept {
    if (m_is_ok) {
      // unhandeld error
    }

    return m_error_value;
  }

  ///
  /// @brief return a const reference to the result value
  ///
  /// @return T_Ok const reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  const T_Error& error() const noexcept {
    if (m_is_ok) {
      // unhandeld error
    }

    return m_error_value;
  }

  ///
  /// @brief return a reference to the result value
  ///
  /// @return T_Ok reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  T_Ok& operator*() noexcept {
    return value();
  }

  ///
  /// @brief return a const reference to the result value
  ///
  /// @return T_Ok const reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  const T_Ok& operator*() const noexcept {
    return value();
  }

  ///
  /// @brief return a pointer to the result value
  ///
  /// @return T_Ok pointer
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  T_Ok* operator->() noexcept {
    if (!m_is_ok) {
      // unhandeld error
    }

    return &m_ok_value;
  }

  ///
  /// @brief return a const pointer to the result value
  ///
  /// @return T_Ok const pointer
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  const T_Ok* operator->() const noexcept {
    if (!m_is_ok) {
      // unhandeld error
    }

    return &m_ok_value;
  }

  ///
  /// @brief convert the result to a bool
  ///
  /// @return true if the result is valid
  ///
  constexpr bool has_value() const noexcept {
    return m_is_ok;
  }

  ///
  /// @brief convert the result to a bool
  ///
  /// @return true if the result is valid
  ///
  constexpr explicit operator bool() const noexcept {
    return m_is_ok;
  }
private:
  bool m_is_ok{false};
  union {
    T_Error m_error_value;
    T_Ok    m_ok_value;
  };
};

////////////////////////////////////////////////////////////////////////////////
///
/// @brief result class
///
/// @param T_Ok the type for the OK result
/// @param T_Error the type for the error result
///
////////////////////////////////////////////////////////////////////////////////
template<typename T_Error>
class result<void, T_Error> {
  static_assert(!std::is_void_v<T_Error>);
  static_assert(std::is_copy_constructible_v<T_Error>);
  static_assert(std::is_move_constructible_v<T_Error>);
  static_assert(std::is_copy_assignable_v<T_Error>);
  static_assert(std::is_move_assignable_v<T_Error>);
public:
  ///
  /// @brief default initialization to error state
  ///
  result() noexcept = default;

  ///
  /// @brief initialization to error state
  ///
  /// @param rhs the error value to assign
  ///
  result(const error_result<T_Error>& rhs) noexcept
    : m_is_ok(false)
    , m_error_value( rhs.error() )
  {
  }

  result(error_result<T_Error>&& rhs) noexcept
    : m_is_ok(false)
    , m_error_value( std::move(rhs.error()) )
  {
  }

  ///
  /// @brief copy contructor
  ///
  /// @param rhs the value to assign
  ///
  result(const result& rhs) noexcept = default;

  ///
  /// @brief move contructor
  ///
  /// @param rhs the value to assign
  ///
  result(result&& rhs) noexcept = default;

  ///
  /// @brief destructor
  ///
  ~result() noexcept = default;

  ///
  /// @brief copy operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(const result& rhs) noexcept = default;

  ///
  /// @brief move operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(result&& rhs) noexcept = default;


  void assign_value() noexcept {
    m_is_ok = true;
  }

  void assign_error(const T_Error& e) noexcept {
    m_is_ok = false;
    m_error_value = T_Error(e);
  }

  void assign_error(T_Error&& e) noexcept {
    m_is_ok = false;
    m_error_value = T_Error(std::move(e));
  }

  ///
  /// @brief copy operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(const error_result<T_Error>& rhs) noexcept
  {
    assign_error(rhs.error());
    return *this;
  }

  ///
  /// @brief move operator
  ///
  /// @param rhs the value to assign
  ///
  result& operator=(error_result<T_Error>&& rhs) noexcept
  {
    assign_error(std::move(rhs.error()));
    return *this;
  }

  ///
  /// @brief return a reference to the OK value
  ///
  /// @return T_Ok reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  void value() noexcept {
    if (!m_is_ok) {
      // unhandeld error
    }
  }

  ///
  /// @brief return a const reference to the result value
  ///
  /// @return T_Ok const reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  void value() const noexcept {
    if (!m_is_ok) {
      // unhandeld error
    }
  }

  ///
  /// @brief return a reference to the OK value
  ///
  /// @return T_Ok reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  T_Error& error() noexcept {
    if (m_is_ok) {
      // unhandeld error
    }

    return m_error_value;
  }

  ///
  /// @brief return a const reference to the result value
  ///
  /// @return T_Ok const reference
  ///
  /// @warning when the result is in error state the the thread will terminate
  ///
  const T_Error& error() const noexcept {
    if (m_is_ok) {
      // unhandeld error
    }

    return m_error_value;
  }

  ///
  /// @brief convert the result to a bool
  ///
  /// @return true if the result is valid
  ///
  constexpr bool has_value() const noexcept {
    return m_is_ok;
  }

  ///
  /// @brief convert the result to a bool
  ///
  /// @return true if the result is valid
  ///
  constexpr explicit operator bool() const noexcept {
    return m_is_ok;
  }
private:
  bool m_is_ok{false};
  T_Error m_error_value;
};

////////////////////////////////////////////////////////////////////////////////

///
/// @brief compare result with bool
///
/// @param lhs the result value
/// @param rhs the bool value
///
/// @return true if lhs is equal to rhs
///
template<typename T_Ok, typename T_Error>
constexpr bool operator==(const result<T_Ok, T_Error>& lhs, bool rhs) noexcept {
  return bool(lhs) == bool(rhs);
}

///
/// @brief compare result with bool
///
/// @param lhs the bool value
/// @param rhs the result value
///
/// @return true if lhs is equal to rhs
///
template<typename T_Ok, typename T_Error>
constexpr bool operator==(bool lhs, const result<T_Ok, T_Error>& rhs) noexcept {
  return bool(lhs) == bool(rhs);
}

///
/// @brief compare result with bool
///
/// @param lhs the result value
/// @param rhs the bool value
///
/// @return true if lhs is not equal to rhs
///
template<typename T_Ok, typename T_Error>
constexpr bool operator!=(const result<T_Ok, T_Error>& lhs, bool rhs) noexcept {
  return bool(lhs) != bool(rhs);
}

///
/// @brief compare result with bool
///
/// @param lhs the bool value
/// @param rhs the result value
///
/// @return true if lhs is not equal to rhs
///
template<typename T_Ok, typename T_Error>
constexpr bool operator!=(bool lhs, const result<T_Ok, T_Error>& rhs) noexcept {
  return bool(lhs) != bool(rhs);
}

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_RESULT_HPP
