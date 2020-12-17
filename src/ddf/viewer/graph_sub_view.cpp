// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2020 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#include "graph_sub_view.hpp"

// project
#include "ltb/util/generic_guard.hpp"

namespace ltb::ddf::vis {

GraphSubView::GraphSubView() {
    ed::Config config;
    config.SettingsFile = "DataflowGraph.json";
    node_context_       = std::shared_ptr<ed::EditorContext>(ed::CreateEditor(&config),
                                                       [](ed::EditorContext* p) { ed::DestroyEditor(p); });
}

GraphSubView::~GraphSubView() = default;

auto GraphSubView::title() -> std::string {
    return "Graph Editor";
}

auto GraphSubView::configure_gui() -> void {
    auto node_editor = ltb::util::make_guard([this] { ed::SetCurrentEditor(node_context_.get()); },
                                             [] { ed::SetCurrentEditor(nullptr); });

    // Start interaction with editor.
    ed::Begin("Dataflow Graph Editor", ImVec2{0.0, 0.0f});
    {
        // display_existing_nodes();
        // handle_interactions(cursor_top_left);
        // maybe_show_node_creation_popup();
    }
    ed::End();

    ed::SetCurrentEditor(nullptr);
}

auto GraphSubView::settings() const -> gvs::SubViewSettings {
    return {{100, 100}};
}

} // namespace ltb::ddf::vis
