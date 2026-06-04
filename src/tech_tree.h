#ifndef TECH_TREE_H
#define TECH_TREE_H

#include "Gamestate.h"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>

namespace godot {

class TechTree : public Control {
    GDCLASS(TechTree, Control);

private:
    // Buttons and labels from tech_tree.tscn.
    TextureButton* exit_button = nullptr;
    Label* money_label = nullptr;
    Label* message_label = nullptr;
    Control* exit_confirm_overlay = nullptr;
    TextureRect* confirm_bat = nullptr;
    Label* confirm_title = nullptr;
    Label* confirm_message = nullptr;
    Button* stay_button = nullptr;
    Button* leave_button = nullptr;
    AudioStreamPlayer* unlock_sfx = nullptr;
    AudioStreamPlayer* button_click_sfx = nullptr;
    AudioStreamPlayer* insufficient_sfx = nullptr;

    // Uses your existing autoload: /root/GlobalGameState.
    GameState* game_state = nullptr;
    String confirm_title_text = "Leaving the Tech Tree?";
    String confirm_message_text = "Buy more upgrades before\nreturning to the shop.";
    double typewriter_timer = 0.0;
    double bat_animation_time = 0.0;
    int title_visible_chars = 0;
    int message_visible_chars = 0;
    bool typing_confirm_text = false;

    int get_upgrade_cost(String upgrade_id) const;
    String get_upgrade_name(String upgrade_id) const;

    void connect_upgrade_button(String node_name, String upgrade_id);
    void show_message(String message);
    void update_money_label();
    void show_exit_confirmation();
    void hide_exit_confirmation();
    void finish_exit_confirmation_typing();
    void play_unlock_sfx();
    void play_button_click_sfx();
    void play_insufficient_sfx();
    void stop_main_game_music();

protected:
    static void _bind_methods();

public:
    TechTree();
    ~TechTree();

    void _ready() override;
    void _process(double delta) override;

    void _on_exit_pressed();
    void _on_stay_pressed();
    void _on_leave_pressed();

    void buy_upgrade(String upgrade_id);
    void show_upgrade_hover(String upgrade_id);
    void clear_upgrade_hover();
};

}

#endif
