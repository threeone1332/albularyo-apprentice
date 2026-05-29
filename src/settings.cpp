#include "settings.h"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Settings::Settings() {
    is_paused = false;
}

Settings::~Settings() {}

void Settings::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_resume_pressed"), &Settings::_on_resume_pressed);
    ClassDB::bind_method(D_METHOD("_on_main_menu_pressed"), &Settings::_on_main_menu_pressed);

    // Updated method binding registration name
    ClassDB::bind_method(D_METHOD("_on_restart_pressed"), &Settings::_on_restart_pressed);

    ClassDB::bind_method(D_METHOD("_on_music_slider_value_changed", "value"), &Settings::_on_music_slider_value_changed);
    ClassDB::bind_method(D_METHOD("_on_sfx_slider_value_changed", "value"), &Settings::_on_sfx_slider_value_changed);
}

void Settings::_ready() {
    set_process_unhandled_input(true);
    set_process_mode(PROCESS_MODE_ALWAYS);
    set_visible(false);
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
    get_tree()->set_pause(false);
    get_tree()->change_scene_to_file("res://scenes/main_menu.tscn");
}

// Renamed implementation function to match restart logic behavior
void Settings::_on_restart_pressed() {
    // 1. Unpause the engine processing loop first!
    get_tree()->set_pause(false);

    // 2. Hide the pause menu layer interface
    set_paused(false);

    // 3. Reload the currently active world map file from frame 0
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