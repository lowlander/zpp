//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP__INCLUDE_ZPP_CLOCK_HPP
#define ZPP__INCLUDE_ZPP_CLOCK_HPP

#include <kernel.h>
#include <sys_clock.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>

namespace zpp {

///
/// @brief Clock measuring elapsed time since the system booted.
///
class uptime_clock {
public:
	using rep = int64_t;
	using period = std::nano;
	using duration = std::chrono::duration<rep, period>;
	using time_point = std::chrono::time_point<uptime_clock>;
	static constexpr bool is_steady = false;

	///
	/// @brief Get current uptime.
	///
	/// @return current uptime as time_point.
	///
	static time_point now() noexcept
	{
		return time_point(duration(k_ticks_to_ns_floor64(k_uptime_ticks())));
	}
};


///
/// @brief Clock representing the system’s hardware clock.
///
class cycle_clock {
public:
	using rep = uint64_t;
	using period = std::nano;
	using duration = std::chrono::duration<rep, period>;
	using time_point = std::chrono::time_point<cycle_clock>;
	static constexpr bool is_steady = false;

	///
	/// @brief Get current cycle count.
	///
	/// @return current cycle count as time_point
	///
	static time_point now() noexcept
	{
		return time_point(duration(k_cyc_to_ns_floor64(k_cycle_get_32())));
	}
};

///
/// @brief convert a duration to tick
///
/// @param d the std::chrono::duration to convert
///
/// @return the number of tick @a d represents
///
template< class Rep, class Period >
constexpr k_ticks_t to_tick( const std::chrono::duration<Rep, Period>& d) noexcept
{
	using namespace std::chrono;

	return k_ns_to_ticks_floor64(duration_cast<nanoseconds>(d).count());
}


///
/// @brief convert a duration to tick
///
/// @param d the std::chrono::duration to convert
///
/// @return the number of tick @a d represents
///
template< class Rep, class Period >
constexpr k_timeout_t to_timeout( const std::chrono::duration<Rep, Period>& d) noexcept
{
	return { to_tick(d) };
}

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_CLOCK_HPP
