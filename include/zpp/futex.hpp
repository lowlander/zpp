///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_FUTEX_HPP
#define ZPP_INCLUDE_ZPP_FUTEX_HPP

#ifdef CONFIG_USERSPACE

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>

namespace zpp {

///
/// @brief A CRTP futex base class.
///
template<typename T_Futex>
class futex_base
{
public:
  using native_type = struct k_futex;
  using native_pointer = native_type *;
  using native_const_pointer = native_type const *;
protected:
  ///
  /// @brief Default constructor
  ///
  constexpr futex_base() noexcept
  {
  }

public:
  ///
  /// @brief Wait for the futex.
  ///
  /// @param expected the expected value
  ///
  /// @return true if successfull
  ///
  [[nodiscard]] bool wait(int expected) noexcept
  {
    if (futex_wait(native_handle(), expected, K_FOREVER) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Try wait for the futex.
  ///
  /// @param expected the expected value
  ///
  /// @return true if successfull
  ///
  [[nodiscard]] bool try_wait(int expected) noexcept
  {
    if (futex_wait(native_handle(), expected, K_NO_WAIT) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Wait for the futex.
  ///
  /// @param expected the expected value
  /// @param timeout the timeout before returning
  ///
  /// @return true if successfull
  ///
  template<class T_Rep, class T_Period>
  [[nodiscard]] bool
  try_wait_for(int expected, const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    using namespace std::chrono;

    if (futex_wait(native_handle(), expected, to_timeout(timeout)) == 0)
    {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Wakeup one waiting thread
  ///
  void wake_one() noexcept
  {
    futex_wake(native_handle(), false);
  }

  ///
  /// @brief Wakeup all waiting threads
  ///
  void wake_all() noexcept
  {
    futex_wake(native_handle(), true);
  }

  ///
  /// @brief get the native zephyr futex handle.
  ///
  /// @return A pointer to the zephyr k_futex.
  ///
  auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_Futex*>(this)->native_handle();
  }

  ///
  /// @brief get the native zephyr futex handle.
  ///
  /// @return A pointer to the zephyr k_futex.
  ///
  auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_Futex*>(this)->native_handle();
  }
public:
  futex_base(const futex_base&) = delete;
  futex_base(futex_base&&) = delete;
  futex_base& operator=(const futex_base&) = delete;
  futex_base& operator=(futex_base&&) = delete;
};

///
/// @brief A futex class.
///
class futex : public futex_base<futex> {
public:
  ///
  /// @brief get the native zephyr futex handle.
  ///
  /// @return A pointer to the zephyr k_futex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_futex;
  }

  ///
  /// @brief get the native zephyr futex handle.
  ///
  /// @return A pointer to the zephyr k_futex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_futex;
  }
private:
  native_type m_futex{};
public:
  futex(const futex&) = delete;
  futex(futex&&) = delete;
  futex& operator=(const futex&) = delete;
  futex& operator=(futex&&) = delete;
};

///
/// @brief A futex class referencing another futex object.
///
class futex_ref : public futex_base<futex_ref> {
public:
  ///
  /// @brief Construct a futex using a native k_futex*
  ///
  /// @param f The k_futex to use. @a f must already be
  ///          initialized and will not be freed.
  ///
  explicit constexpr futex_ref(native_pointer f) noexcept
    : m_futex_ptr(f)
  {
    __ASSERT_NO_MSG(m_futex_ptr != nullptr);
  }

  ///
  /// @brief Construct a futex using another futex object
  ///
  /// @param f The futex to use. @a f must already be
  ///          initialized and will not be freed.
  ///
  template<typename T_Futex>
  explicit constexpr futex_ref(T_Futex& f) noexcept
    : m_futex_ptr(f.native_handle())
  {
    __ASSERT_NO_MSG(m_futex_ptr != nullptr);
  }

  ///
  /// @brief copy operator
  ///
  /// @param f The k_futex to use. @a f must already be
  ///          initialized and will not be freed.
  ///
  /// @return Reference to this object
  ///
  constexpr futex_ref& operator=(native_pointer f) noexcept
  {
    m_futex_ptr = f;
    __ASSERT_NO_MSG(m_futex_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief copy operator
  ///
  /// @param f The futex object to use. @a f must already be
  ///          initialized and will not be freed.
  ///
  /// @return Reference to this object
  ///
  template<typename T_Futex>
  constexpr futex_ref& operator=(T_Futex& f) noexcept
  {
    m_futex_ptr = f.native_handle();
    __ASSERT_NO_MSG(m_futex_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief get the native zephyr futex handle.
  ///
  /// @return A pointer to the zephyr k_futex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return m_futex_ptr;
  }

  ///
  /// @brief get the native zephyr futex handle.
  ///
  /// @return A pointer to the zephyr k_futex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return m_futex_ptr;
  }
private:
  native_pointer m_futex_ptr{ nullptr };
public:
  futex_ref() = delete;
};

} // namespace zpp

#endif // CONFIG_USERSPACE

#endif // ZPP_INCLUDE_ZPP_FUTEX_HPP
