// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <type_traits>

namespace ltb::util {

/**
 * @brief Converts incremental enums to bit values
 *
 * enum class Enum : uint32_t {
 *     first,
 *     second,
 *     third
 * }
 *
 * to_bits(Enum::first)  -> 0b0001
 * to_bits(Enum::second) -> 0b0010
 * to_bits(Enum::third)  -> 0b0100
 *
 * to_bits(Enum::first, Enum::second) -> 0b0011
 * to_bits(Enum::first, Enum::third)  -> 0b0101
 */
template <typename Enum>
constexpr auto to_bits(Enum type) -> typename std::underlying_type<Enum>::type {
    using BitType = typename std::underlying_type<Enum>::type;
    return BitType(1) << static_cast<BitType>(type);
}

template <typename Enum, typename... Enums>
constexpr auto to_bits(Enum type, Enums... types) -> typename std::underlying_type<Enum>::type {
    return to_bits<Enum>(type) | to_bits<Enum>(types...);
}

template <typename Enum>
constexpr auto toggle_flag(const typename std::underlying_type<Enum>::type& flags, Enum flag) ->
    typename std::underlying_type<Enum>::type {
    return flags ^ to_bits(flag);
}

template <typename Enum>
constexpr auto has_flag(const typename std::underlying_type<Enum>::type& flags, Enum flag) -> bool {
    return static_cast<bool>(flags & to_bits(flag));
}

template <typename Enum>
constexpr auto all_bits() -> typename std::underlying_type<Enum>::type {
    return ~(typename std::underlying_type<Enum>::type(0));
}

} // namespace ltb::util
