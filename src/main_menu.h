#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/h_slider.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

namespace godot {

class MainMenu : public Control {
    GDCLASS(MainMenu, Control);

private:
    // UI Connections
    Panel* panel;
    HSlider* music_slider;
    HSlider* sfx_slider;
    Button* click_outside_detector;
    TextureButton* exit_button;

    const String SAVE_PATH = "user://savegame.json";

protected:
    static void _bind_methods();

public:
    MainMenu();
    ~MainMenu();

    void _ready() override;

    // UI Button Actions
    void _on_start_button_pressed();
    void _on_settings_pressed();
    void _on_exit_settings_pressed();
    void _on_click_outside_detector_pressed();
    void _on_quit_pressed();

    // Audio Slider Callbacks
    void _on_music_slider_value_changed(double value);
    void _on_sfx_slider_value_changed(double value);

    void load_game();
};

} // namespace godot

#endif // MAIN_MENU_H