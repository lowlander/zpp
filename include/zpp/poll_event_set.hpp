//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP__INCLUDE__ZPP__POLL_EVENT_SET_HPP
#define ZPP__INCLUDE__ZPP__POLL_EVENT_SET_HPP

#ifdef CONFIG_POLL

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>
#include <optional>
#include <array>

#include <zpp/sem.hpp>
#include <zpp/fifo.hpp>

#include <zpp/poll_event.hpp>
#include <zpp/poll_signal.hpp>

namespace zpp {

///
/// @brief A set of poll events
///
/// @param Size the size of the set
///
template<int Size>
class poll_event_set
{
public:
	auto begin() noexcept { return m_events.begin(); }
	auto end() noexcept { return m_events.end(); }
public:
	///
	/// @brief default constructor
	///
	poll_event_set() noexcept
	{
	}

	///
	/// @brief constuctor that takes argments for initialization
	///
	/// @param t the arguments to use for initialization
	///
	template <class... T>
	poll_event_set(T&&... t) noexcept
	{
		assign(0, t...);
	}

	///
	/// @brief access an element of the set
	///
	/// @param idx the index of the event to get
	///
	/// @return a poll_event that has a reference to the indexed event
	///
	auto operator[](size_t idx) noexcept
	{
		__ASSERT_NO_MSG(idx < Size);
		return poll_event(&m_events[idx]);
	}

	///
	/// @brief poll events waiting for ever
	///
	/// @return false on error
	///
	auto poll() noexcept
	{
		return poll(K_FOREVER);
	}

	///
	/// @brief try poll events without waiting
	///
	/// @return false on error
	///
	auto try_poll() noexcept
	{
		return poll(K_NO_WAIT);
	}

	///
	/// @brief try poll events waiting for e certain time
	///
	/// @param timeout the time to wait
	///
	/// @return false on error
	///
	template < class Rep, class Period>
	auto try_poll_for(const std::chrono::duration<Rep, Period>&
						timeout) noexcept
	{
		using namespace std::chrono;

		return poll(duration_cast<milliseconds>(timeout).count());
	}
private:
	void assign(int index) noexcept
	{
		__ASSERT_NO_MSG(index == Size);
	}

	template <class First, class... T>
	void assign(int index, First&& f, T&&... t) noexcept
	{
		poll_event(&m_events[index]).assign(f);
		assign(index+1, t...);
	}

	auto poll(s32_t timeout) noexcept
	{
		for (auto& e: m_events) {
			e.state = K_POLL_STATE_NOT_READY;
			if (e.tag == (int)poll_event::type_tag::type_signal) {
				__ASSERT_NO_MSG(e.signal != nullptr);
				e.signal->signaled = 0;
			}
		}

		auto rc = k_poll(m_events.data(), m_events.size(), timeout);

		if (rc == 0) {
			return true;
		} else {
			return false;
		}
	}
private:
	std::array<struct k_poll_event, Size> m_events;
};

template <class... T>
poll_event_set(T&&... t) noexcept -> poll_event_set<sizeof...(T)>;

} // namespace zpp

#endif // CONFIG_POLL

#endif // ZPP__INCLUDE__ZPP__POLL_EVENT_SET_HPP
