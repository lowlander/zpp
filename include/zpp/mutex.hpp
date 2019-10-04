///
/// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP__INCLUDE__ZPP__MUTEX_HPP
#define ZPP__INCLUDE__ZPP__MUTEX_HPP

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <mutex>

namespace zpp {

///
/// @brief A recursive mutex class.
///
template<typename Mutex>
class mutex_base
{
protected:
	///
	/// @brief Default contructor
	///
	constexpr mutex_base() noexcept
	{
	}

public:

	///
	/// @brief Lock the mutex. Wait for ever until it is locked.
	///
	/// @return true if successfully locked.
	///
	[[nodiscard]] bool lock() noexcept
	{
		if (k_mutex_lock(native_handle(), K_FOREVER) == 0) {
			return true;
		} else {
			return false;
		}
	}

	///
	/// @brief Try locking the mutex without waiting.
	///
	/// @return true if successfully locked.
	///
	[[nodiscard]] bool try_lock() noexcept
	{
		if (k_mutex_lock(native_handle(), K_NO_WAIT) == 0) {
			return true;
		} else {
			return false;
		}
	}

	///
	/// @brief Try locking the mutex with a timeout.
	///
	/// @param timeout The time to wait before returning
	///
	/// @return true if successfully locked.
	///
	template < class Rep, class Period>
	[[nodiscard]] bool
	try_lock_for(const std::chrono::duration<Rep, Period>& timeout) noexcept
	{
		using namespace std::chrono;

		if (k_mutex_lock(native_handle(),
			duration_cast<milliseconds>(timeout).count()) == 0)
		{
			return true;
		} else {
			return false;
		}
	}

	///
	/// @brief Unlock the mutex.
	///
	void unlock() noexcept
	{
		k_mutex_unlock(native_handle());
	}

	///
	/// @brief get the native zephyr mutex handle.
	///
	/// @return A pointer to the zephyr k_mutex.
	///
	auto native_handle() noexcept
	{
		return static_cast<Mutex*>(this)->native_handle();
	}
public:
	mutex_base(const mutex_base&) = delete;
	mutex_base(mutex_base&&) = delete;
	mutex_base& operator=(const mutex_base&) = delete;
	mutex_base& operator=(mutex_base&&) = delete;
};

///
/// @brief A recursive mutex class.
///
class mutex : public mutex_base<mutex> {
public:
	///
	/// @brief Default contructor
	///
	mutex() noexcept
	{
		k_mutex_init(&m_mutex);
	}

	///
	/// @brief get the native zephyr mutex handle.
	///
	/// @return A pointer to the zephyr k_mutex.
	///
	constexpr auto native_handle() noexcept
	{
		return &m_mutex;
	}
private:
	struct k_mutex m_mutex;
public:
	mutex(const mutex&) = delete;
	mutex(mutex&&) = delete;
	mutex& operator=(const mutex&) = delete;
	mutex& operator=(mutex&&) = delete;
};

///
/// @brief A recursive mutex class borrowing the native mutex.
///
class borrowed_mutex : public mutex_base<borrowed_mutex> {
public:
	///
	/// @brief Contruct a mutex using a native k_mutex*
	///
	/// @param m The k_mutex to use. @a m must already be
	///          initialized and will not be freed.
	///
	explicit constexpr borrowed_mutex(struct k_mutex* m) noexcept
		: m_mutex_ptr(m)
	{
		__ASSERT_NO_MSG(m_mutex_ptr != nullptr);
	}

	///
	/// @brief get the native zephyr mutex handle.
	///
	/// @return A pointer to the zephyr k_mutex.
	///
	constexpr auto native_handle() noexcept
	{
		__ASSERT_NO_MSG(m_mutex_ptr != nullptr);

		return m_mutex_ptr;
	}
private:
	struct k_mutex* m_mutex_ptr{ nullptr };
public:
	borrowed_mutex(const borrowed_mutex&) = delete;
	borrowed_mutex(borrowed_mutex&&) = delete;
	borrowed_mutex& operator=(const borrowed_mutex&) = delete;
	borrowed_mutex& operator=(borrowed_mutex&&) = delete;
};


///
/// @brief std::lock_guard using zpp::mutex as a lock.
///
template <typename Mutex>
class lock_guard {
public:
	explicit lock_guard(Mutex& lock) noexcept
		: m_lock(lock)
	{
		auto res = m_lock.lock();
		__ASSERT_NO_MSG(res != false);
	}

	~lock_guard()
	{
		m_lock.unlock();
	}
private:
	Mutex& m_lock;
};

} // namespace zpp

#endif // ZPP__INCLUDE__ZPP__MUTEX_HPP
