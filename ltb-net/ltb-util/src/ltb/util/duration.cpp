// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "duration.hpp"

// external
#include <doctest/doctest.h>

namespace {

using namespace std::chrono_literals;
using namespace ltb;

TEST_CASE("[ltb][util][time][duration] duration time types evaluate to expected values") {
    CHECK(util::to_seconds(27s) == doctest::Approx(27.f));
    CHECK(util::to_seconds(27s + 500ms) == doctest::Approx(27.5f));
    CHECK(util::to_seconds<double>(27s + 500ms) == doctest::Approx(27.5));
    CHECK(util::to_seconds<int>(27s + 500ms) == 27);

    CHECK(util::to_hours(1h) == doctest::Approx(1.f));
    CHECK(util::to_minutes(1h) == doctest::Approx(60.f));
    CHECK(util::to_seconds(1h) == doctest::Approx(3600.f));
    CHECK(util::to_millis(1h) == doctest::Approx(3'600'000.f));
    CHECK(util::to_micros(1h) == doctest::Approx(3'600'000'000.f));
    CHECK(util::to_nanos(1h) == doctest::Approx(3'600'000'000'000.f));

    CHECK(util::duration_seconds(30) == 30s);
    CHECK(util::to_seconds(util::duration_seconds(30.f)) == doctest::Approx(30.f));

    CHECK(util::to_seconds(util::duration_hours(0.5f)) == doctest::Approx(1800.f));
    CHECK(util::to_seconds(util::duration_minutes(0.5f)) == doctest::Approx(30.f));
    CHECK(util::to_seconds(util::duration_seconds(0.5f)) == doctest::Approx(.5f));
    CHECK(util::to_seconds(util::duration_millis(0.5f)) == doctest::Approx(.000'5f));
    CHECK(util::to_seconds(util::duration_micros(0.5f)) == doctest::Approx(.000'000'5f));
    CHECK(util::to_seconds(util::duration_nanos(500.f)) == doctest::Approx(.000'000'5f));
}

} // namespace
