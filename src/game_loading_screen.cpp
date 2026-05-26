#include "game_loading_screen.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

void GameLoadingScreen::_bind_methods() {
}

GameLoadingScreen::GameLoadingScreen() {
    progress_bar = nullptr;
    status_label = nullptr;
    dynamic_flavor_label = nullptr;

    is_switching = false;
    progress_speed = 40.0;

    current_flavor_index = 0;
    flavor_timer = 0.0f;
    flavor_interval = 1.5f;

    UtilityFunctions::randomize();

    // Your Albularyo custom text lines
    flavor_texts.push_back("Hinahanda na ang mga sangkap...");
    flavor_texts.push_back("Nililinis na ang mesa...");
    flavor_texts.push_back("Pinapakuluan na ang kawa...");
    flavor_texts.push_back("Inaayos ang mahiwagang libro...");
    flavor_texts.push_back("Kinakausap ang mga espiritu...");
}

GameLoadingScreen::~GameLoadingScreen() {}

void GameLoadingScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    set_process(true);

    // Node target map linking
    progress_bar = Object::cast_to<ProgressBar>(find_child("ProgressBar", true, false));
    status_label = Object::cast_to<Label>(find_child("StatusLabel", true, false));
    dynamic_flavor_label = Object::cast_to<Label>(find_child("TextLines", true, false));

    if (progress_bar == nullptr) {
        UtilityFunctions::printerr("Error: Could not find ProgressBar node!");
    } else {
        progress_bar->set_value(0.0);
    }

    if (status_label != nullptr) {
        status_label->set_text("LOADING... 0 / 100");
    }

    if (dynamic_flavor_label && flavor_texts.size() > 0) {
        current_flavor_index = UtilityFunctions::randi() % flavor_texts.size();
        dynamic_flavor_label->set_text(flavor_texts[current_flavor_index]);
    }
}

void GameLoadingScreen::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint() || is_switching) {
        return;
    }

    flavor_timer += delta;
    if (flavor_timer >= flavor_interval) {
        flavor_timer = 0.0f;
        change_flavor_text();
    }

    if (progress_bar != nullptr) {
        double current_val = progress_bar->get_value();

        if (current_val < 100.0) {
            double next_val = current_val + (progress_speed * delta);
            progress_bar->set_value(next_val);

            if (status_label != nullptr) {
                int display_pct = static_cast<int>(next_val);
                if (display_pct > 100) display_pct = 100;
                status_label->set_text("LOADING... " + String::num(display_pct, 0) + " / 100");
            }

        } else {
            is_switching = true;
            UtilityFunctions::print("C++: Loading Complete! Transitioning to main gameplay...");

            SceneTree* tree = get_tree();
            if (tree != nullptr) {
                tree->change_scene_to_file("res://scenes/main_screen.tscn");
            }
        }
    }
}

void GameLoadingScreen::change_flavor_text() {
    if (flavor_texts.size() <= 1 || !dynamic_flavor_label) return;

    int next_index = current_flavor_index;
    while (next_index == current_flavor_index) {
        next_index = UtilityFunctions::randi() % flavor_texts.size();
    }

    current_flavor_index = next_index;
    dynamic_flavor_label->set_text(flavor_texts[current_flavor_index]);
}

}