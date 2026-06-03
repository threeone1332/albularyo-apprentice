#include "main_menu.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

MainMenu::MainMenu() {
    panel = nullptr;
    music_slider = nullptr;
    sfx_slider = nullptr;
    click_outside_detector = nullptr;
    exit_button = nullptr;
    button_click_sfx = nullptr;
}

MainMenu::~MainMenu() {}

void MainMenu::_bind_methods() {
    // Bind our UI action methods so Godot signals can see them
    ClassDB::bind_method(D_METHOD("_on_start_button_pressed"), &MainMenu::_on_start_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_settings_pressed"), &MainMenu::_on_settings_pressed);
    ClassDB::bind_method(D_METHOD("_on_exit_settings_pressed"), &MainMenu::_on_exit_settings_pressed);
    ClassDB::bind_method(D_METHOD("_on_click_outside_detector_pressed"), &MainMenu::_on_click_outside_detector_pressed);
    ClassDB::bind_method(D_METHOD("_on_quit_pressed"), &MainMenu::_on_quit_pressed);

    // Bind slider value changes
    ClassDB::bind_method(D_METHOD("_on_music_slider_value_changed", "value"), &MainMenu::_on_music_slider_value_changed);
    ClassDB::bind_method(D_METHOD("_on_sfx_slider_value_changed", "value"), &MainMenu::_on_sfx_slider_value_changed);

    //delay audio for start and quit in main menu
    ClassDB::bind_method(D_METHOD("_on_start_scene_delay_timeout"), &MainMenu::_on_start_scene_delay_timeout);
    ClassDB::bind_method(D_METHOD("_on_quit_delay_timeout"), &MainMenu::_on_quit_delay_timeout);
}

void MainMenu::_ready() {
    // Correct C++ singleton way to check for the editor mode
    if (Engine::get_singleton()->is_editor_hint()) return;

    // Fetching the UI pointers safely...
    panel = get_node<Panel>("Panel");
    // (Rest of your code remains exactly the same!)
    music_slider = get_node<HSlider>("Panel/SliderPaddingContainer/VBoxContainer/MusicRow/MusicSlider");
    sfx_slider = get_node<HSlider>("Panel/SliderPaddingContainer/VBoxContainer/SFXRow/SFXSlider");
    click_outside_detector = get_node<Button>("Panel/ClickOutsideDetector");
    exit_button = get_node<TextureButton>("Panel/MarginContainer/NinePatchRect/exit_settings");

    if (!panel || !music_slider || !sfx_slider || !click_outside_detector) {
        UtilityFunctions::printerr("MainMenu C++: UI Nodes not found properly!");
        return;
    }

    panel->set_visible(false);

    // 1. LAYOUT MOUSE FILTER CONTROLS
    get_node<Control>("Panel/MarginContainer")->set_mouse_filter(MOUSE_FILTER_IGNORE);
    get_node<Control>("Panel/MarginContainer/NinePatchRect")->set_mouse_filter(MOUSE_FILTER_PASS);
    get_node<Control>("Panel/SliderPaddingContainer")->set_mouse_filter(MOUSE_FILTER_IGNORE);
    get_node<Control>("Panel/SliderPaddingContainer/VBoxContainer")->set_mouse_filter(MOUSE_FILTER_IGNORE);
    get_node<Control>("Panel/SliderPaddingContainer/VBoxContainer/MusicRow")->set_mouse_filter(MOUSE_FILTER_IGNORE);
    get_node<Control>("Panel/SliderPaddingContainer/VBoxContainer/SFXRow")->set_mouse_filter(MOUSE_FILTER_IGNORE);

    music_slider->set_mouse_filter(MOUSE_FILTER_STOP);
    sfx_slider->set_mouse_filter(MOUSE_FILTER_STOP);
    click_outside_detector->set_mouse_filter(MOUSE_FILTER_STOP);
    if (exit_button) exit_button->set_mouse_filter(MOUSE_FILTER_STOP);

    // 2. Configure slider scale settings for 0.1% precision
    music_slider->set_min(0.0);
    music_slider->set_max(1.0);
    music_slider->set_step(0.001);

    sfx_slider->set_min(0.0);
    sfx_slider->set_max(1.0);
    sfx_slider->set_step(0.001);

    // 3. Connect signals programmatically
    if (!music_slider->is_connected("value_changed", Callable(this, "_on_music_slider_value_changed"))) {
        music_slider->connect("value_changed", Callable(this, "_on_music_slider_value_changed"));
    }
    if (!sfx_slider->is_connected("value_changed", Callable(this, "_on_sfx_slider_value_changed"))) {
        sfx_slider->connect("value_changed", Callable(this, "_on_sfx_slider_value_changed"));
    }
    if (!click_outside_detector->is_connected("pressed", Callable(this, "_on_click_outside_detector_pressed"))) {
        click_outside_detector->connect("pressed", Callable(this, "_on_click_outside_detector_pressed"));
    }

    // 4. Initialize at exactly 50% sound volume
    music_slider->set_value(0.5);
    sfx_slider->set_value(0.5);

    button_click_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("ButtonClickSFX"));

    if (!button_click_sfx) {
        UtilityFunctions::printerr("MainMenu C++: ButtonClickSFX node not found.");
}
}

void MainMenu::_on_start_button_pressed() {
    play_button_click_sfx();

    if (FileAccess::file_exists(SAVE_PATH)) {
        load_game();
        pending_scene_path = "res://scenes/game_loading_screen.tscn";
    } else {
        pending_scene_path = "res://scenes/intro_cutscene.tscn";
    }

    Ref<SceneTreeTimer> timer = get_tree()->create_timer(0.35);
    timer->connect("timeout", Callable(this, "_on_start_scene_delay_timeout"));
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
    Ref<SceneTreeTimer> timer = get_tree()->create_timer(0.15);
    timer->connect("timeout", Callable(this, "_on_quit_delay_timeout"));
}

void MainMenu::load_game() {
    Ref<FileAccess> file = FileAccess::open(SAVE_PATH, FileAccess::READ);
    if (file.is_null()) return;

    String content = file->get_as_text();
    file->close();

    JSON json;
    if (json.parse(content) != OK) return;

    Variant data = json.get_data();
}

// --- AUDIO SLIDER LOGIC ---

void MainMenu::_on_music_slider_value_changed(double value) {
    AudioServer* audio_server = AudioServer::get_singleton();
    if (!audio_server) return;

    int bus_index = audio_server->get_bus_index("Music");
    if (bus_index != -1) {
        if (value <= 0.005) {
            audio_server->set_bus_mute(bus_index, true);
        } else {
            audio_server->set_bus_mute(bus_index, false);
            // Using Godot's native conversion function inside UtilityFunctions namespace
            double volume_db = UtilityFunctions::linear_to_db(value);
            audio_server->set_bus_volume_db(bus_index, volume_db);
        }
    }
}

void MainMenu::_on_sfx_slider_value_changed(double value) {
    AudioServer* audio_server = AudioServer::get_singleton();
    if (!audio_server) return;

    int bus_index = audio_server->get_bus_index("SFX");
    if (bus_index != -1) {
        if (value <= 0.005) {
            audio_server->set_bus_mute(bus_index, true);
        } else {
            audio_server->set_bus_mute(bus_index, false);
            double volume_db = UtilityFunctions::linear_to_db(value);
            audio_server->set_bus_volume_db(bus_index, volume_db);
        }
    }
}

void MainMenu::play_button_click_sfx() {
    if (button_click_sfx) {
        button_click_sfx->play();
    }
}

void MainMenu::_on_start_scene_delay_timeout() {
    if (!pending_scene_path.is_empty()) {
        get_tree()->change_scene_to_file(pending_scene_path);
    }
}

void MainMenu::_on_quit_delay_timeout() {
    get_tree()->quit();
}
} // namespace godot
