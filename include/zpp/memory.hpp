//
// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_MEMORY_HPP
#define ZPP_INCLUDE_ZPP_MEMORY_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <new>

[[nodiscard]] void* operator new(std::size_t) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new[](std::size_t) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new(std::size_t, std::align_val_t) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new[](std::size_t, std::align_val_t) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new(std::size_t, const std::nothrow_t&) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new[](std::size_t, const std::nothrow_t&) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new(std::size_t, std::align_val_t, const std::nothrow_t&) noexcept
{
  return nullptr;
}

[[nodiscard]] void* operator new[](std::size_t, std::align_val_t, const std::nothrow_t&) noexcept
{
  return nullptr;
}

#endif // ZPP_INCLUDE_ZPP_MEMORY_HPP
