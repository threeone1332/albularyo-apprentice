#include "main_menu.h"

#include "Gamestate.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

MainMenu::MainMenu() {}
MainMenu::~MainMenu() {}

void MainMenu::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_start_button_pressed"), &MainMenu::_on_start_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_continue_pressed"), &MainMenu::_on_continue_pressed);
    ClassDB::bind_method(D_METHOD("_on_new_game_pressed"), &MainMenu::_on_new_game_pressed);
    ClassDB::bind_method(D_METHOD("_on_settings_pressed"), &MainMenu::_on_settings_pressed);
    ClassDB::bind_method(D_METHOD("_on_exit_settings_pressed"), &MainMenu::_on_exit_settings_pressed);
    ClassDB::bind_method(D_METHOD("_on_click_outside_detector_pressed"), &MainMenu::_on_click_outside_detector_pressed);
    ClassDB::bind_method(D_METHOD("_on_quit_pressed"), &MainMenu::_on_quit_pressed);

    ClassDB::bind_method(D_METHOD("_on_music_slider_value_changed", "value"), &MainMenu::_on_music_slider_value_changed);
    ClassDB::bind_method(D_METHOD("_on_sfx_slider_value_changed", "value"), &MainMenu::_on_sfx_slider_value_changed);

    ClassDB::bind_method(D_METHOD("_on_start_scene_delay_timeout"), &MainMenu::_on_start_scene_delay_timeout);
    ClassDB::bind_method(D_METHOD("_on_quit_delay_timeout"), &MainMenu::_on_quit_delay_timeout);
}

template <class T>
static T *safe_get(Node *parent, const String &path) {
    if (!parent) return nullptr;
    Node *n = parent->get_node_or_null(path);
    return Object::cast_to<T>(n);
}

static void safe_connect(Object *object, const StringName &signal, const Callable &callable) {
    if (!object) return;
    if (!object->is_connected(signal, callable)) {
        object->connect(signal, callable);
    }
}

void MainMenu::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) return;

    UtilityFunctions::print("[MAIN MENU] init");

    no_save_buttons = safe_get<VBoxContainer>(this, "MarginContainer2/BG/NoSaveButtons");
    has_save_buttons = safe_get<VBoxContainer>(this, "MarginContainer2/BG/HasSaveButtons");

    start_button = safe_get<Button>(this, "MarginContainer2/BG/NoSaveButtons/Start");
    setting_button = safe_get<Button>(this, "MarginContainer2/BG/NoSaveButtons/Setting");
    quit_button = safe_get<Button>(this, "MarginContainer2/BG/NoSaveButtons/Quit");

    continue_button = safe_get<Button>(this, "MarginContainer2/BG/HasSaveButtons/Continue");
    new_game_button = safe_get<Button>(this, "MarginContainer2/BG/HasSaveButtons/NewGame");
    setting2_button = safe_get<Button>(this, "MarginContainer2/BG/HasSaveButtons/Setting2");
    quit2_button = safe_get<Button>(this, "MarginContainer2/BG/HasSaveButtons/Quit2");

    panel = safe_get<Panel>(this, "Panel");

    music_slider = safe_get<HSlider>(this, "Panel/SliderPaddingContainer/VBoxContainer/MusicRow/MusicSlider");
    sfx_slider = safe_get<HSlider>(this, "Panel/SliderPaddingContainer/VBoxContainer/SFXRow/SFXSlider");

    click_outside_detector = safe_get<Button>(this, "Panel/ClickOutsideDetector");
    exit_button = safe_get<Button>(this, "Panel/MarginContainer/NinePatchRect/exit_settings");

    button_click_sfx = safe_get<AudioStreamPlayer>(this, "ButtonClickSFX");

    if (panel) panel->set_visible(false);

    const StringName pressed_signal("pressed");
    safe_connect(start_button, pressed_signal, callable_mp(this, &MainMenu::_on_start_button_pressed));
    safe_connect(setting_button, pressed_signal, callable_mp(this, &MainMenu::_on_settings_pressed));
    safe_connect(quit_button, pressed_signal, callable_mp(this, &MainMenu::_on_quit_pressed));

    safe_connect(continue_button, pressed_signal, callable_mp(this, &MainMenu::_on_continue_pressed));
    safe_connect(new_game_button, pressed_signal, callable_mp(this, &MainMenu::_on_new_game_pressed));
    safe_connect(setting2_button, pressed_signal, callable_mp(this, &MainMenu::_on_settings_pressed));
    safe_connect(quit2_button, pressed_signal, callable_mp(this, &MainMenu::_on_quit_pressed));

    safe_connect(exit_button, pressed_signal, callable_mp(this, &MainMenu::_on_exit_settings_pressed));
    safe_connect(click_outside_detector, pressed_signal, callable_mp(this, &MainMenu::_on_click_outside_detector_pressed));

    const StringName value_changed_signal("value_changed");
    safe_connect(music_slider, value_changed_signal, callable_mp(this, &MainMenu::_on_music_slider_value_changed));
    safe_connect(sfx_slider, value_changed_signal, callable_mp(this, &MainMenu::_on_sfx_slider_value_changed));

    bool save_exists = FileAccess::file_exists(SAVE_PATH);

    if (no_save_buttons) no_save_buttons->set_visible(!save_exists);
    if (has_save_buttons) has_save_buttons->set_visible(save_exists);
}

void MainMenu::_on_start_button_pressed() {
    play_button_click_sfx();
    pending_scene_path = "res://scenes/intro_cutscene.tscn";

    start_one_shot_timer(0.35, callable_mp(this, &MainMenu::_on_start_scene_delay_timeout));
}

void MainMenu::_on_continue_pressed() {
    UtilityFunctions::print("[MAIN MENU] Continue pressed.");
    play_button_click_sfx();

    UtilityFunctions::print("[MAIN MENU] Loading save before scene switch...");
    if (!load_game()) {
        UtilityFunctions::printerr("[MAIN MENU] Continue aborted because save loading failed.");
        return;
    }

    UtilityFunctions::print("[MAIN MENU] Save load finished. Scheduling game loading screen.");

    pending_scene_path = "res://scenes/game_loading_screen.tscn";

    start_one_shot_timer(0.35, callable_mp(this, &MainMenu::_on_start_scene_delay_timeout));
}

void MainMenu::_on_new_game_pressed() {
    play_button_click_sfx();
    pending_scene_path = "res://scenes/intro_cutscene.tscn";

    start_one_shot_timer(0.35, callable_mp(this, &MainMenu::_on_start_scene_delay_timeout));
}

void MainMenu::_on_settings_pressed() {
    play_button_click_sfx();
    if (panel) panel->set_visible(true);
}

void MainMenu::_on_exit_settings_pressed() {
    play_button_click_sfx();
    if (panel) panel->set_visible(false);
}

void MainMenu::_on_click_outside_detector_pressed() {
    play_button_click_sfx();
    if (panel) panel->set_visible(false);
}

void MainMenu::_on_quit_pressed() {
    play_button_click_sfx();
    start_one_shot_timer(0.15, callable_mp(this, &MainMenu::_on_quit_delay_timeout));
}

void MainMenu::_on_music_slider_value_changed(double value) {
    int bus_idx = AudioServer::get_singleton()->get_bus_index("Music");
    if (bus_idx != -1) {
        float db = UtilityFunctions::linear_to_db(value);
        AudioServer::get_singleton()->set_bus_volume_db(bus_idx, db);
    } else {
        UtilityFunctions::print("Music Bus not found yet. Current Slider Value: ", value);
    }
}

void MainMenu::_on_sfx_slider_value_changed(double value) {
    int bus_idx = AudioServer::get_singleton()->get_bus_index("SFX");
    if (bus_idx != -1) {
        float db = UtilityFunctions::linear_to_db(value);
        AudioServer::get_singleton()->set_bus_volume_db(bus_idx, db);
    } else {
        UtilityFunctions::print("SFX Bus not found yet. Current Slider Value: ", value);
    }
}

void MainMenu::start_one_shot_timer(double delay_seconds, const Callable &timeout_callback) {
    Timer *timer = memnew(Timer);
    timer->set_one_shot(true);
    timer->set_wait_time(delay_seconds < 0.0 ? 0.0 : delay_seconds);
    add_child(timer);

    const StringName timeout_signal("timeout");
    timer->connect(timeout_signal, timeout_callback, Object::CONNECT_ONE_SHOT);
    timer->start();
}

bool MainMenu::load_game() {
    String resolved_path = SAVE_PATH;
    if (ProjectSettings::get_singleton()) {
        resolved_path = ProjectSettings::get_singleton()->globalize_path(SAVE_PATH);
    }

    UtilityFunctions::print("[MAIN MENU] Save path: ", SAVE_PATH);
    UtilityFunctions::print("[MAIN MENU] Resolved save path: ", resolved_path);

    if (!FileAccess::file_exists(SAVE_PATH)) {
        UtilityFunctions::printerr("[MAIN MENU] Save file does not exist.");
        return false;
    }

    UtilityFunctions::print("[MAIN MENU] Save file exists. Opening for read...");

    Ref<FileAccess> file = FileAccess::open(SAVE_PATH, FileAccess::READ);
    if (file.is_null()) {
        UtilityFunctions::printerr("[MAIN MENU] Failed to open save file. Open error: ", FileAccess::get_open_error());
        return false;
    }

    UtilityFunctions::print("[MAIN MENU] Save file opened.");

    String content = file->get_as_text();
    file->close();

    UtilityFunctions::print("[MAIN MENU] Save file bytes/chars read: ", content.length());

    if (content.strip_edges().is_empty()) {
        UtilityFunctions::printerr("[MAIN MENU] Save file is empty.");
        return false;
    }

    Ref<JSON> json;
    json.instantiate();

    UtilityFunctions::print("[MAIN MENU] Parsing save JSON...");
    Error parse_error = json->parse(content);
    if (parse_error != OK) {
        UtilityFunctions::printerr("[MAIN MENU] Save JSON parse failed. Error: ", json->get_error_message(), " line: ", json->get_error_line());
        return false;
    }

    UtilityFunctions::print("[MAIN MENU] Save JSON parsed.");

    Variant data = json->get_data();
    if (data.get_type() != Variant::DICTIONARY) {
        UtilityFunctions::printerr("[MAIN MENU] Save JSON root is not a Dictionary. Variant type: ", data.get_type());
        return false;
    }

    UtilityFunctions::print("[MAIN MENU] Save JSON root is Dictionary.");

    GameState *global = Object::cast_to<GameState>(get_node_or_null(NodePath("/root/GlobalGameState")));
    if (!global) {
        UtilityFunctions::printerr("MainMenu C++: GlobalGameState autoload not found.");
        return false;
    }

    UtilityFunctions::print("[MAIN MENU] Applying save data to GlobalGameState...");
    global->load_save_data(data);
    UtilityFunctions::print("[MAIN MENU] GlobalGameState loaded successfully.");
    return true;
}

void MainMenu::play_button_click_sfx() {
    if (button_click_sfx) button_click_sfx->play();
}

void MainMenu::_on_start_scene_delay_timeout() {
    if (pending_scene_path.is_empty()) return;
    if (get_tree()) get_tree()->change_scene_to_file(pending_scene_path);
}

void MainMenu::_on_quit_delay_timeout() {
    if (get_tree()) get_tree()->quit();
}

}
