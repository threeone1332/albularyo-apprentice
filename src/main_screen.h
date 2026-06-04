#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/nine_patch_rect.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/interval_tweener.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

namespace godot {

class MainScreen : public Control {
    GDCLASS(MainScreen, Control);

private:
    const double PHASE_DURATION = 15.0;
    const double SELL_INTERVAL = 1.0;

    // Node pointers
    Node* game_state;
    Label* money_label;
    Label* gain_label;
    Label* sale_feedback;
    TextureRect* morning_icon;
    TextureRect* noon_icon;
    TextureRect* night_icon;
    Button* decrease_button;
    Label* price_label;
    Button* increase_button;
    Label* sell_chance_label;
    Button* change_button;
    Label* potion_name_label;
    Button* tech_tree_button;
    Button* mixing_button;
    NinePatchRect* potion_icon;

    // --- AUTOSAVE INDICATOR UI DECLARATIONS ---
    PanelContainer* autosave_panel; // Changed from TextureRect* to PanelContainer*
    Label* autosave_label;
    int autosave_id;
    // ------------------------------------------

    // Local loop state tracker mechanics
    double phase_timer;
    double sell_timer;
    int feedback_id;
    int gain_id;

    Vector<Rect2> potion_icon_regions;
    Vector2 sale_feedback_start_pos;
    Vector2 gain_label_start_pos;

    // Private function signature to match main_screen.cpp
    void _save_game_to_disk();

    //for SFX 
    AudioStreamPlayer* button_click_sfx;
    AudioStreamPlayer* money_sfx;
    String pending_scene_path;

    void play_button_click_sfx();
    void play_money_sfx();
    void ensure_main_game_music();

protected:
    static void _bind_methods();

public:
    MainScreen();
    ~MainScreen();

    void _ready() override;
    void _process(double delta) override;

    // Core state ticks
    void _attempt_sale_tick();
    void _advance_phase_tick();

    // UI Signal hooks
    void _on_decrease_pressed();
    void _on_increase_pressed();
    void _on_mixing_pressed();
    void _on_tech_tree_pressed();

    // Internal Helpers
    void _update_ui();
    void _update_time_icons();
    void _show_sale_feedback(String text, bool sold);
    void _show_gold_gain(int amount);
    void _show_gold_loss(int amount);
    void _on_scene_change_delay_timeout();
};

} // namespace godot

#endif // MAIN_SCREEN_H