// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#if 0

#include "ddf/core/ddf.hpp"

using namespace ltb;

auto main() -> int {
    ddf::ddf ddf;
    return 0;
}

#elif 0

#include <flexcore/pure/pure_ports.hpp>
#include <iostream>
#include <string>

using namespace fc;

std::string var;

int main() {
    pure::state_source<std::string> src{[] { return var; }};
    pure::state_sink<std::string>   sink;
    src >> sink;

    std::cout << "'" << sink.get() << "'" << std::endl;
    var = "Hello!";
    std::cout << "'" << sink.get() << "'" << std::endl;
}

#elif 0

#include <flexcore/extended/base_node.hpp>
#include <flexcore/infrastructure.hpp>
#include <iostream>

auto get_temperature() -> int {
    return 42;
}

auto print_temperature(int T) -> void {
    std::cout << "Current temperature is: " << T << std::endl;
}

struct MyNode : fc::tree_base_node {
    explicit MyNode(const fc::node_args& args)
        : tree_base_node(args),
          trigger(this, [this] { this->work(); }),
          curr_temp(this),
          temp_out(this, get_temperature) {}

    void work() { curr_temp.fire(temp_out()); }

    fc::event_sink<void>  trigger;
    fc::event_source<int> curr_temp;
    fc::state_source<int> temp_out;
};

auto main() -> int {
    fc::pure::event_source<void> go_signal;
    fc::pure::event_sink<int>    sink{print_temperature};

    fc::infrastructure infra;

    auto& node = infra.node_owner().make_child_named<MyNode>("my node");

    go_signal >> node.trigger;
    node.curr_temp >> sink;

    go_signal.fire();
}

#elif 1

#include <flexcore/extended/base_node.hpp>
#include <flexcore/infrastructure.hpp>
#include <iostream>

struct MyRegionNode : fc::tree_base_node {
    explicit MyRegionNode(const fc::node_args& args)
        : tree_base_node(args),
          i(0),
          src(this, [this] { return i++; }),
          sink(this),
          do_work(this, [this, name = this->name()] { work(name); }) {}

    void work(const std::string& name) {
        std::stringstream ss;
        ss << name << " " << sink.get() << std::endl;
        std::cout << ss.str();
    }

    int i;

    fc::state_source<int> src;
    fc::state_sink<int>   sink;
    fc::event_sink<void>  do_work;
};

auto main() -> int {
    fc::infrastructure infra;

    auto region_a = infra.add_region("Region A", fc::thread::cycle_control::medium_tick);
    auto region_b = infra.add_region("Region B", fc::thread::cycle_control::fast_tick);

    auto& node_a = infra.node_owner().make_child_named<MyRegionNode>(region_a, "Node A");
    auto& node_b = infra.node_owner().make_child_named<MyRegionNode>(region_b, "Node B");

    node_a.src >> node_b.sink;
    node_b.src >> node_a.sink;
    region_a->work_tick() >> node_a.do_work;
    region_b->work_tick() >> node_b.do_work;

    infra.start_scheduler();
    infra.iterate_main_loop();
    infra.stop_scheduler();
}

#endif
