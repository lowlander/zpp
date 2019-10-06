//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP__INCLUDE__ZPP__POLL_EVENT_HPP
#define ZPP__INCLUDE__ZPP__POLL_EVENT_HPP

#ifdef CONFIG_POLL

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>
#include <optional>
#include <array>

#include <zpp/sem.hpp>
#include <zpp/fifo.hpp>

namespace zpp {

///
/// @brief wrapper class around a k_poll_event
///
class poll_event {
public:
	///
	/// @brief type of event sources
	///
	enum class type_tag : u8_t {
		type_unknown,
		type_sem,
		type_fifo,
		type_signal,
		type_ignore,
	};

	///
	/// @brief wrap a k_poll_event pointer
	///
	/// @param event the event to wrap
	///
	poll_event(k_poll_event* event) noexcept
		: m_event(event)
	{
		__ASSERT_NO_MSG(m_event != nullptr);
	}

	///
	/// @brief assign a null event (that will be ignored when polling)
	///
	void assign(std::nullptr_t) noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);

		m_event->obj = nullptr;
		m_event->state = K_POLL_STATE_NOT_READY;
		m_event->mode = K_POLL_MODE_NOTIFY_ONLY;
		m_event->type = K_POLL_TYPE_IGNORE;
		m_event->tag = (int)type_tag::type_ignore;
	}

	///
	/// @brief assign a semaphore to this event
	///
	/// @param s the semaphore to poll
	///
	template<typename SemType>
	void assign(sem_base<SemType>* s) noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		k_poll_event_init(m_event,
			K_POLL_TYPE_SEM_AVAILABLE,
			K_POLL_MODE_NOTIFY_ONLY,
			s->native_handle());
		m_event->tag = (int)type_tag::type_sem;
	}

	///
	/// @brief assign a fifo to this event
	///
	/// @param f the fifo to poll
	///
	template<typename FifoType, typename ItemType>
	void assign(fifo_base<FifoType, ItemType>* f) noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		k_poll_event_init(m_event,
			K_POLL_TYPE_FIFO_DATA_AVAILABLE,
			K_POLL_MODE_NOTIFY_ONLY,
			f->native_handle());
		m_event->tag = (int)type_tag::type_fifo;
	}

	///
	/// @brief assign a signal to this event
	///
	/// @param s the signal to poll
	///
	template<typename SignalType>
	void assign(poll_signal_base<SignalType>* s) noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		k_poll_event_init(m_event,
			K_POLL_TYPE_SIGNAL,
			K_POLL_MODE_NOTIFY_ONLY,
			s->native_handle());
		m_event->tag = (int)type_tag::type_signal;
	}

	///
	/// @brief check if this event is ready
	///
	/// @return true if the event is ready
	///
	bool is_ready() noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		__ASSERT_NO_MSG(m_event->tag != (int)type_tag::type_unknown);

		if ((m_event->state & K_POLL_STATE_SIGNALED) ||
		    (m_event->state & K_POLL_STATE_SEM_AVAILABLE) ||
		    (m_event->state & K_POLL_STATE_DATA_AVAILABLE) ||
		    (m_event->state & K_POLL_STATE_FIFO_DATA_AVAILABLE)) {
			return true;
		} else {
			return false;
		}
	}

	///
	/// @brief reset event to non ready state
	///
	void reset() noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);

		m_event->state = K_POLL_STATE_NOT_READY;
	}

	///
	/// @brief check if this event is cancelled
	///
	/// @return true if the event is cancelled
	///
	bool is_cancelled() noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		__ASSERT_NO_MSG(m_event->tag != (int)type_tag::type_unknown);

		if (m_event->state & K_POLL_STATE_CANCELLED) {
			return true;
		} else {
			return false;
		}
	}

	///
	/// @brief get access to the fifo of the event
	///
	/// @warning the event must be an fifo event and the fifo ItemType
	///          must match with the registerred ItemType
	///
	/// @return a borrowed_fifo that points to the registered fifo
	///
	template<typename ItemType>
	auto fifo() noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		__ASSERT_NO_MSG(m_event->tag == (int)type_tag::type_fifo);
		__ASSERT_NO_MSG(m_event->fifo != nullptr);

		return borrowed_fifo<ItemType>(m_event->fifo);
	}

	///
	/// @brief get access to the sem of the event
	///
	/// @warning the event must be a sem event
	///
	/// @return a borrowed_sem that points to the registered sem
	///
	auto sem() noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		__ASSERT_NO_MSG(m_event->tag == (int)type_tag::type_sem);
		__ASSERT_NO_MSG(m_event->sem != nullptr);

		return borrowed_sem(m_event->sem);
	}

	///
	/// @brief get access to the signal of the event
	///
	/// @warning the event must be a signal event
	///
	/// @return a borrowed_poll_signal that points to the registered signal
	///
	auto signal() noexcept
	{
		__ASSERT_NO_MSG(m_event != nullptr);
		__ASSERT_NO_MSG(m_event->tag == (int)type_tag::type_signal);
		__ASSERT_NO_MSG(m_event->signal != nullptr);

		return borrowed_poll_signal(m_event->signal);
	}
private:
	k_poll_event* m_event{ nullptr };
public:
	poll_event(const poll_event&) = delete;
	poll_event(poll_event&&) = delete;
	poll_event& operator=(const poll_event&) = delete;
	poll_event& operator=(poll_event&&) = delete;
};

} // namespace zpp

#endif // CONFIG_POLL

#endif // ZPP__INCLUDE__ZPP__POLL_EVENT_HPP
