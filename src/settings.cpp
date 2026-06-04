#include "settings.h"
#include "Gamestate.h"

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/base_button.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Settings::Settings() {
    is_paused = false;
    bound_scene = nullptr;
    pending_action_type = 0;
}

Settings::~Settings() {}

void Settings::_bind_methods() {
    ClassDB::bind_method(D_METHOD("open_settings_menu"), &Settings::open_settings_menu);

    // Layout Infrastructure Connections
    ClassDB::bind_method(D_METHOD("_on_resume_pressed"), &Settings::_on_resume_pressed);
    ClassDB::bind_method(D_METHOD("_on_main_menu_pressed"), &Settings::_on_main_menu_pressed);
    ClassDB::bind_method(D_METHOD("_on_restart_pressed"), &Settings::_on_restart_pressed);
    ClassDB::bind_method(D_METHOD("_on_music_slider_value_changed", "value"), &Settings::_on_music_slider_value_changed);
    ClassDB::bind_method(D_METHOD("_on_sfx_slider_value_changed", "value"), &Settings::_on_sfx_slider_value_changed);

    // Modal Action Bindings
    ClassDB::bind_method(D_METHOD("_on_confirm_yes_pressed"), &Settings::_on_confirm_yes_pressed);
    ClassDB::bind_method(D_METHOD("_on_confirm_no_pressed"), &Settings::_on_confirm_no_pressed);
}

void Settings::_ready() {
    set_process_unhandled_input(true);
    set_process_mode(PROCESS_MODE_ALWAYS);
    set_visible(false);

    bound_scene = nullptr;
    set_process(true);

    // Target verified scene nodes layout
    confirmation_panel = cast_to<Control>(get_node_or_null("ConfirmationPanel"));
    click_blocker = cast_to<BaseButton>(get_node_or_null("ConfirmationPanel/ClickBlocker"));
    check_button = cast_to<BaseButton>(get_node_or_null("ConfirmationPanel/BackgroundRect/MarginContainer/VBoxContainer/ButtonContainer/CheckButton"));
    x_button = cast_to<BaseButton>(get_node_or_null("ConfirmationPanel/BackgroundRect/MarginContainer/VBoxContainer/ButtonContainer/XButton"));

    // Hook internal call nodes safely
    if (check_button && !check_button->is_connected("pressed", Callable(this, "_on_confirm_yes_pressed"))) {
        check_button->connect("pressed", Callable(this, "_on_confirm_yes_pressed"));
    }
    if (x_button && !x_button->is_connected("pressed", Callable(this, "_on_confirm_no_pressed"))) {
        x_button->connect("pressed", Callable(this, "_on_confirm_no_pressed"));
    }
    if (click_blocker && !click_blocker->is_connected("pressed", Callable(this, "_on_confirm_no_pressed"))) {
        click_blocker->connect("pressed", Callable(this, "_on_confirm_no_pressed"));
    }

    if (confirmation_panel) {
        confirmation_panel->set_visible(false);
    }
}

void Settings::_process(double delta) {
    SceneTree *tree = get_tree();
    if (!tree) return;

    Node *current_scene = tree->get_current_scene();
    if (!current_scene) return;

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
        if (confirmation_panel && confirmation_panel->is_visible()) {
            _on_confirm_no_pressed();
        } else {
            set_paused(!is_paused);
        }
        get_viewport()->set_input_as_handled();
    }
}

void Settings::set_paused(bool p_state) {
    is_paused = p_state;
    set_visible(is_paused);

    if (!is_paused && confirmation_panel) {
        confirmation_panel->set_visible(false);
        pending_action_type = 0;
    }

    get_tree()->set_pause(is_paused);
}

void Settings::_on_resume_pressed() {
    set_paused(false);
}

// --- REDIRECT TO CONFIRMATION PROMPTS OVERLAY ---

void Settings::_on_main_menu_pressed() {
    pending_action_type = 1;
    if (confirmation_panel) {
        confirmation_panel->set_visible(true);
        confirmation_panel->queue_redraw(); // Eliminates visualization brightness pop delay
    }
    Control *bg_rect = cast_to<Control>(get_node_or_null("ConfirmationPanel/BackgroundRect"));
    if (bg_rect) bg_rect->queue_redraw();
}

void Settings::_on_restart_pressed() {
    pending_action_type = 2;
    if (confirmation_panel) {
        confirmation_panel->set_visible(true);
        confirmation_panel->queue_redraw(); // Eliminates visualization brightness pop delay
    }
    Control *bg_rect = cast_to<Control>(get_node_or_null("ConfirmationPanel/BackgroundRect"));
    if (bg_rect) bg_rect->queue_redraw();
}

// --- MODAL DIALOG USER CHOICES EXECUTORS ---

void Settings::_on_confirm_no_pressed() {
    pending_action_type = 0;
    if (confirmation_panel) {
        confirmation_panel->set_visible(false);
    }
}

void Settings::_on_confirm_yes_pressed() {
    if (confirmation_panel) {
        confirmation_panel->set_visible(false);
    }

    if (pending_action_type == 1) {
        // Option 1: Main Menu Return
        set_paused(false);
        bound_scene = nullptr;
        get_tree()->change_scene_to_file("res://scenes/main_menu.tscn");
    }
    else if (pending_action_type == 2) {
        // Option 2: Hard Persistence Reset & Level Reload (Wipe like New Game)
        UtilityFunctions::print("[SETTINGS] Running persistent data cache purge...");

        // Wipe user save file on drive storage
        if (FileAccess::file_exists("user://savegame.json")) {
            Ref<DirAccess> dir = DirAccess::open("user://");
            if (dir.is_valid()) {
                dir->remove("savegame.json");
                UtilityFunctions::print("[SETTINGS] savegame.json unlinked and wiped successfully.");
            }
        }

        // Standardize GlobalGameState active metrics back to default properties
        GameState *global = Object::cast_to<GameState>(get_node_or_null("/root/GlobalGameState"));
        if (global) {
            global->reset_state();
            UtilityFunctions::print("[SETTINGS] GlobalGameState registers cleared back to 0.");
        }

        // Resume engine timeline cycles and execute complete screen reload sequence
        set_paused(false);
        get_tree()->reload_current_scene();
    }

    pending_action_type = 0;
}

// --- BUS VOLUME UTILITY CONTROL INTERFACES ---

void Settings::_on_music_slider_value_changed(double value) {
    AudioServer *server = AudioServer::get_singleton();
    int bus_idx = server->get_bus_index("Music");
    if (bus_idx != -1) {
        if (value <= 0.005) {
            server->set_bus_mute(bus_idx, true);
        } else {
            server->set_bus_mute(bus_idx, false);
            server->set_bus_volume_db(bus_idx, UtilityFunctions::linear_to_db(value));
        }
    }
}

void Settings::_on_sfx_slider_value_changed(double value) {
    AudioServer *server = AudioServer::get_singleton();
    int bus_idx = server->get_bus_index("SFX");
    if (bus_idx != -1) {
        if (value <= 0.005) {
            server->set_bus_mute(bus_idx, true);
        } else {
            server->set_bus_mute(bus_idx, false);
            server->set_bus_volume_db(bus_idx, UtilityFunctions::linear_to_db(value));
        }
    }
}