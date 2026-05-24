#include "loading_screen.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

namespace godot {

void LoadingScreen::_bind_methods() {}

LoadingScreen::LoadingScreen() {
    loading_bar = nullptr;
    percent_label = nullptr;
    is_switching = false;
}

LoadingScreen::~LoadingScreen() {}

void LoadingScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Look for the nodes sitting right under the root node
    loading_bar = get_node<ProgressBar>("LoadingBar");
    percent_label = get_node<Label>("PercentLabel");

    if (loading_bar == nullptr) {
        UtilityFunctions::printerr("Error: Could not find LoadingBar node!");
    } else {
        loading_bar->set_value(0.0);
    }

    if (percent_label != nullptr) {
        percent_label->set_text("LOADING... 0");
    }
}

void LoadingScreen::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint() || is_switching) {
        return;
    }

    if (loading_bar != nullptr) {
        double current_val = loading_bar->get_value();

        if (current_val < 100.0) {
            // Incremental fill step
            double next_val = current_val + (progress_speed * delta);
            loading_bar->set_value(next_val);

            // Update static label to read "LOADING... X"
            if (percent_label != nullptr) {
                int display_pct = static_cast<int>(next_val);
                if (display_pct > 100) display_pct = 100;

                percent_label->set_text("LOADING... " + String::num(display_pct));
            }

        } else {
            // Trigger target scene load
            is_switching = true;
            UtilityFunctions::print("C++: Loading Complete! Switching to Main Menu...");

            SceneTree* tree = get_tree();
            if (tree != nullptr) {
                tree->change_scene_to_file("res://main_menu.tscn");
            }
        }
    }
}

} // namespace godot