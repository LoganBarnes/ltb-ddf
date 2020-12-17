// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <chrono>

namespace ltb::util {

using Duration = std::chrono::steady_clock::duration;

namespace detail {

template <typename IntegralType, typename PeriodType>
constexpr auto time_to_integral(const Duration& duration) -> IntegralType {
    return std::chrono::duration_cast<std::chrono::duration<IntegralType, typename PeriodType::period>>(duration)
        .count();
}

template <typename PeriodType, typename IntegralType>
constexpr auto time_from_integral(IntegralType value) -> Duration {
    return std::chrono::duration_cast<Duration>(
        std::chrono::duration<IntegralType, typename PeriodType::period>(value));
}

} // namespace detail

template <typename TargetType = float>
constexpr auto to_hours(const Duration& duration) -> TargetType {
    return detail::time_to_integral<TargetType, std::chrono::hours>(duration);
}

template <typename TargetType = float>
constexpr auto to_minutes(const Duration& duration) -> TargetType {
    return detail::time_to_integral<TargetType, std::chrono::minutes>(duration);
}

template <typename TargetType = float>
constexpr auto to_seconds(const Duration& duration) -> TargetType {
    return detail::time_to_integral<TargetType, std::chrono::seconds>(duration);
}

template <typename TargetType = float>
constexpr auto to_millis(const Duration& duration) -> TargetType {
    return detail::time_to_integral<TargetType, std::chrono::milliseconds>(duration);
}

template <typename TargetType = float>
constexpr auto to_micros(const Duration& duration) -> TargetType {
    return detail::time_to_integral<TargetType, std::chrono::microseconds>(duration);
}

template <typename TargetType = float>
constexpr auto to_nanos(const Duration& duration) -> TargetType {
    return detail::time_to_integral<TargetType, std::chrono::nanoseconds>(duration);
}

template <typename Input>
constexpr auto duration_hours(Input value) -> Duration {
    return detail::time_from_integral<std::chrono::hours>(value);
}

template <typename Input>
constexpr auto duration_minutes(Input value) -> Duration {
    return detail::time_from_integral<std::chrono::minutes>(value);
}

template <typename Input>
constexpr auto duration_seconds(Input value) -> Duration {
    return detail::time_from_integral<std::chrono::seconds>(value);
}

template <typename Input>
constexpr auto duration_millis(Input value) -> Duration {
    return detail::time_from_integral<std::chrono::milliseconds>(value);
}

template <typename Input>
constexpr auto duration_micros(Input value) -> Duration {
    return detail::time_from_integral<std::chrono::microseconds>(value);
}

template <typename Input>
constexpr auto duration_nanos(Input value) -> Duration {
    return detail::time_from_integral<std::chrono::nanoseconds>(value);
}

} // namespace ltb::util
