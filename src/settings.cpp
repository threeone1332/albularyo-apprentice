#include "settings.h"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/base_button.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Settings::Settings() {
    is_paused = false;
    bound_scene = nullptr;
}

Settings::~Settings() {}

void Settings::_bind_methods() {
    ClassDB::bind_method(D_METHOD("open_settings_menu"), &Settings::open_settings_menu);

    // Regular UI Bindings
    ClassDB::bind_method(D_METHOD("_on_resume_pressed"), &Settings::_on_resume_pressed);
    ClassDB::bind_method(D_METHOD("_on_main_menu_pressed"), &Settings::_on_main_menu_pressed);
    ClassDB::bind_method(D_METHOD("_on_restart_pressed"), &Settings::_on_restart_pressed);
    ClassDB::bind_method(D_METHOD("_on_music_slider_value_changed", "value"), &Settings::_on_music_slider_value_changed);
    ClassDB::bind_method(D_METHOD("_on_sfx_slider_value_changed", "value"), &Settings::_on_sfx_slider_value_changed);
}

void Settings::_ready() {
    set_process_unhandled_input(true);
    set_process_mode(PROCESS_MODE_ALWAYS);
    set_visible(false);

    bound_scene = nullptr;
    set_process(true);
}

void Settings::_process(double delta) {
    SceneTree *tree = get_tree();
    if (!tree) return;

    Node *current_scene = tree->get_current_scene();
    if (!current_scene) return;

    // FIX: Only run the heavy find_child lookup when the scene actually changes!
    if (bound_scene != current_scene) {
        try_bind_settings_button(current_scene);
    }
}

void Settings::try_bind_settings_button(Node *current_scene) {
    BaseButton *btn = cast_to<BaseButton>(current_scene->find_child("SettingsButton", true, false));

    if (btn) {
        if (!btn->is_connected("pressed", Callable(this, "open_settings_menu"))) {
            btn->connect("pressed", Callable(this, "open_settings_menu"));
            UtilityFunctions::print("C++ Safely Hooked TextureButton inside: ", current_scene->get_name());
        }
        bound_scene = current_scene;
    }
}

void Settings::open_settings_menu() {
    set_paused(true);
}

void Settings::_unhandled_input(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_cancel")) {
        set_paused(!is_paused);
        get_viewport()->set_input_as_handled();
    }
}

void Settings::set_paused(bool p_state) {
    is_paused = p_state;
    set_visible(is_paused);
    get_tree()->set_pause(is_paused);
}

void Settings::_on_resume_pressed() {
    set_paused(false);
}

void Settings::_on_main_menu_pressed() {
    // FIX: Forcefully clear the pause state and hide the menu overlay completely before moving
    set_paused(false);
    bound_scene = nullptr; // Reset the binding cache layer so the next scene re-scans smoothly

    get_tree()->change_scene_to_file("res://scenes/main_menu.tscn");
}

void Settings::_on_restart_pressed() {
    set_paused(false);
    get_tree()->reload_current_scene();
}

void Settings::_on_music_slider_value_changed(double value) {
    int bus_idx = AudioServer::get_singleton()->get_bus_index("Music");
    if (bus_idx != -1) {
        float db = UtilityFunctions::linear_to_db(value);
        AudioServer::get_singleton()->set_bus_volume_db(bus_idx, db);
    } else {
        UtilityFunctions::print("Music Bus not found yet. Current Slider Value: ", value);
    }
}

void Settings::_on_sfx_slider_value_changed(double value) {
    int bus_idx = AudioServer::get_singleton()->get_bus_index("SFX");
    if (bus_idx != -1) {
        float db = UtilityFunctions::linear_to_db(value);
        AudioServer::get_singleton()->set_bus_volume_db(bus_idx, db);
    } else {
        UtilityFunctions::print("SFX Bus not found yet. Current Slider Value: ", value);
    }
}