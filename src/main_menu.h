#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/h_slider.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class MainMenu : public Control {
    GDCLASS(MainMenu, Control)

private:
    static constexpr const char *SAVE_PATH = "user://savegame.json";

    VBoxContainer *no_save_buttons = nullptr;
    VBoxContainer *has_save_buttons = nullptr;

    Button *start_button = nullptr;
    Button *setting_button = nullptr;
    Button *quit_button = nullptr;

    Button *continue_button = nullptr;
    Button *new_game_button = nullptr;
    Button *setting2_button = nullptr;
    Button *quit2_button = nullptr;

    Button *click_outside_detector = nullptr;
    Button *exit_button = nullptr;

    Panel *panel = nullptr;

    HSlider *music_slider = nullptr;
    HSlider *sfx_slider = nullptr;

    AudioStreamPlayer *button_click_sfx = nullptr;

    String pending_scene_path;

protected:
    static void _bind_methods();

public:
    MainMenu();
    ~MainMenu();

    void _ready() override;

    void _on_start_button_pressed();
    void _on_continue_pressed();
    void _on_new_game_pressed();
    void _on_settings_pressed();
    void _on_exit_settings_pressed();
    void _on_click_outside_detector_pressed();
    void _on_quit_pressed();

    void _on_music_slider_value_changed(double value);
    void _on_sfx_slider_value_changed(double value);

    void _on_start_scene_delay_timeout(Variant timer_node);
    void _on_quit_delay_timeout(Variant timer_node);

    void start_one_shot_timer(double delay_seconds, const Callable &timeout_callback);
    void play_button_click_sfx();
    bool load_game();
};

}

#endif
