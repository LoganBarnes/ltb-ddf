// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "main_window.hpp"

auto main(int argc, char* argv[]) -> int {
    return ltb::ddf::vis::MainWindow({argc, argv}).exec();
}