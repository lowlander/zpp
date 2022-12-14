///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_CONDITION_VARIABLE_HPP
#define ZPP_INCLUDE_ZPP_CONDITION_VARIABLE_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>

#include <zpp/mutex.hpp>
#include <zpp/utils.hpp>
#include <zpp/result.hpp>
#include <zpp/error_code.hpp>

namespace zpp {

///
/// @brief A condition variable CRTP base class.
///
template<typename T_ConditionVariable>
class condition_variable_base
{
public:
  using native_type = struct k_condvar;
  using native_pointer = native_type*;
  using native_const_pointer = native_type const *;
protected:
  ///
  /// @brief Protected default constructor so only derived classes can be created
  ///
  constexpr condition_variable_base() noexcept = default;
public:

  ///
  /// @brief Notify one waiter.
  ///
  /// @return true if successfull.
  ///
  [[nodiscard]] auto notify_one() noexcept
  {
    result<void, error_code> res;

    auto rc = k_condvar_signal(native_handle());
    if (rc == 0) {
      res.assign_value();
    } else {
      res.assign_error(to_error_code(-rc));
    }

    return res;
  }

  ///
  /// @brief Notify all waiters.
  ///
  /// @return true if successfull.
  ///
  [[nodiscard]] auto notify_all() noexcept
  {
    result<void, error_code> res;

    auto rc = k_condvar_broadcast(native_handle());
    if (rc == 0) {
      res.assign_value();
    } else {
      res.assign_error(to_error_code(-rc));
    }

    return res;
  }

  ///
  /// @brief wait for ever until the variable is signaled.
  ///
  /// @param m The mutex to use
  ///
  /// @return true if successfull.
  ///
  template<class T_Mutex>
  [[nodiscard]] auto wait(T_Mutex& m) noexcept
  {
    result<void, error_code> res;

    auto h = m.native_handle();
    if (h == nullptr) {
      res.assign_error(error_code::k_inval);
    } else {
      auto rc = k_condvar_wait(native_handle(), h, K_FOREVER);
      if (rc == 0) {
        res.assign_value();
      } else {
        res.assign_error(to_error_code(-rc));
      }
    }

    return res;
  }

  ///
  /// @brief Try waiting with a timeout to see if the variable is signaled.
  ///
  /// @param m The mutex to use
  /// @param timeout The time to wait before returning
  ///
  /// @return true if successfull.
  ///
  template <class T_Mutex, class T_Rep, class T_Period>
  [[nodiscard]] auto
  try_wait_for(T_Mutex& m, const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    using namespace std::chrono;

    result<void, error_code> res;

    auto h = m.native_handle();
    if (h == nullptr) {
      res.assign_error(error_code::k_inval);
    } else {
      auto rc = k_condvar_wait(native_handle(), h, to_timeout(timeout));
      if (rc == 0) {
        res.assign_value();
      } else {
        res.assign_error(to_error_code(-rc));
      }
    }

    return res;
  }


  ///
  /// @brief wait for ever until the variable is signaled.
  ///
  /// @param m The mutex to use
  /// @param pred The predecate that must be true before the wait returns
  ///
  /// @return true if successfull.
  ///
  template<class T_Mutex, class T_Predecate>
  [[nodiscard]] auto wait(T_Mutex& m, T_Predecate pred) noexcept
  {
    result<void, error_code> res;

    auto h = m.native_handle();
    if (h == nullptr) {
      res.assign_error(error_code::k_inval);
    } else {
      while (pred() == false) {
        auto rc = k_condvar_wait(native_handle(), h, K_FOREVER);
        if (rc != 0) {
          res.assign_error(to_error_code(-rc));
          return res;
        }
      }

      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief Try waiting with a timeout to see if the variable is signaled.
  ///
  /// @param m The mutex to use
  /// @param timeout The time to wait before returning
  /// @param pred The predecate that must be true before the wait returns
  ///
  /// @return true if successfull.
  ///
  template <class T_Mutex, class T_Rep, class T_Period, class T_Predecate>
  [[nodiscard]] auto
  try_wait_for(T_Mutex& m, const std::chrono::duration<T_Rep, T_Period>& timeout, T_Predecate pred) noexcept
  {
    using namespace std::chrono;

    result<void, error_code> res;

    auto h = m.native_handle();
    if (h == nullptr) {
      res.assign_error(error_code::k_inval);
    } else {
      while(pred() == false) {
        auto rc = k_condvar_wait(native_handle(), h, to_timeout(timeout));
        if (rc != 0) {
          res.assign_error(to_error_code(-rc));
          return res;
        }

        // TODO update timeout
      }

      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief get the native zephyr k_condvar pointer.
  ///
  /// @return A pointer to the zephyr k_condvar pointer.
  ///
  [[nodiscard]] auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_ConditionVariable*>(this)->native_handle();
  }

  ///
  /// @brief get the native zephyr k_condvar pointer.
  ///
  /// @return A pointer to the zephyr k_condvar pointer.
  ///
  auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_ConditionVariable*>(this)->native_handle();
  }
public:
  condition_variable_base(const condition_variable_base&) = delete;
  condition_variable_base(condition_variable_base&&) = delete;
  condition_variable_base& operator=(const condition_variable_base&) = delete;
  condition_variable_base& operator=(condition_variable_base&&) = delete;
};

///
/// @brief A condition variable class.
///
class condition_variable
  : public condition_variable_base<condition_variable> {
public:
  ///
  /// @brief Default constructor
  ///
  condition_variable() noexcept
  {
    k_condvar_init(&m_condvar);
  }

  ///
  /// @brief get the native zephyr condition variable handle.
  ///
  /// @return A pointer to the zephyr k_condvar pointer.
  ///
  [[nodiscard]] constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_condvar;
  }
  ///
  /// @brief get the native zephyr condition variable handle.
  ///
  /// @return A pointer to the zephyr k_condvar pointer.
  ///
  [[nodiscard]] constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_condvar;
  }
private:
  native_type m_condvar{};
public:
  condition_variable(const condition_variable&) = delete;
  condition_variable(condition_variable&&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;
  condition_variable& operator=(condition_variable&&) = delete;
};

///
/// @brief A class using a reference to another native condition variable
///        or zpp::condition_variable.
///
/// @warning The condition variable that is referenced must be valid for the
///          lifetime of this object.
///
class condition_variable_ref
  : public condition_variable_base<condition_variable_ref> {
public:
  ///
  /// @brief Construct a condition variable using a native k_condvar*
  ///
  /// @param cv The k_condvar to use. @a cv must already be
  ///           initialized and will not be freed.
  ///
  /// @warning cv must be valid for the lifetime of this object.
  ///
  explicit constexpr condition_variable_ref(native_pointer cv) noexcept
    : m_condvar_ptr(cv)
  {
    __ASSERT_NO_MSG(m_condvar_ptr != nullptr);
  }

  ///
  /// @brief Construct a condition variable using another condition variable
  ///
  /// @param cv The condition variable to reference to. @a cv must already be
  ///           initialized and will not be freed.
  ///
  /// @warning cv must be valid for the lifetime of this object.
  ///
  template<class T_ContitionVariable>
  explicit constexpr condition_variable_ref(const T_ContitionVariable& cv) noexcept
    : m_condvar_ptr(cv.native_handle())
  {
    __ASSERT_NO_MSG(m_condvar_ptr != nullptr);
  }

  ///
  /// @brief copy a condition variable using another condition variable
  ///
  /// @param cv The condition variable to reference to. @a cv must already be
  ///           initialized and will not be freed.
  ///
  /// @warning cv must be valid for the lifetime of this object.
  ///
  condition_variable_ref& operator=(native_pointer cv) noexcept
  {
    m_condvar_ptr = cv;
    __ASSERT_NO_MSG(m_condvar_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief copy a condition variable using another condition variable
  ///
  /// @param cv The condition variable to reference to. @a cv must already be
  ///           initialized and will not be freed.
  ///
  /// @warning cv must be valid for the lifetime of this object.
  ///
  template<class T_ContitionVariable>
  condition_variable_ref& operator=(const T_ContitionVariable& cv) noexcept
  {
    m_condvar_ptr = cv.native_handle();
    __ASSERT_NO_MSG(m_condvar_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief get the native zephyr condition variable handle.
  ///
  /// @return A pointer to the zephyr k_condvar pointer.
  ///
  [[nodiscard]] constexpr auto native_handle() noexcept -> native_pointer
  {
    __ASSERT_NO_MSG(m_condvar_ptr != nullptr);

    return m_condvar_ptr;
  }

  ///
  /// @brief get the native zephyr condition variable handle.
  ///
  /// @return A pointer to the zephyr k_condvar pointer.
  ///
  [[nodiscard]] constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    __ASSERT_NO_MSG(m_condvar_ptr != nullptr);

    return m_condvar_ptr;
  }
private:
  native_pointer m_condvar_ptr{ nullptr };
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_CONDITION_VARIABLE_HPP
