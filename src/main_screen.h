#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/nine_patch_rect.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class MainScreen : public Control {
    GDCLASS(MainScreen, Control);

private:
    const double PHASE_DURATION = 15.0;
    const double SELL_INTERVAL = 1.0;

    Node* game_state;
    NinePatchRect* main_background;
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

    AudioStreamPlayer* button_click_sfx;
    AudioStreamPlayer* money_sfx;

    PanelContainer* autosave_panel;
    Label* autosave_label;
    int autosave_id;

    MarginContainer* goal_tracker_container;
    Label* tech_goal_label;
    Label* gold_goal_label;
    bool gold_goal_revealed;

    CanvasLayer* victory_overlay;
    Label* score_summary;
    Label* time_elapsed_label;
    TextureButton* return_menu_button;

    bool is_victory_triggered;

    Ref<Texture2D> bg_morning;
    Ref<Texture2D> bg_afternoon;
    Ref<Texture2D> bg_night;

    double phase_timer;
    double sell_timer;
    int feedback_id;
    int gain_id;

    // Fixed: Track active runtime references to prevent overlapping tween memory leaks
    Ref<Tween> active_feedback_tween;
    Ref<Tween> active_gain_tween;

    Vector<Rect2> potion_icon_regions;
    Vector2 sale_feedback_start_pos;
    Vector2 gain_label_start_pos;

    void _save_game_to_disk();
    void _check_victory_condition();
    void ensure_main_game_music();
    void play_button_click_sfx();
    void play_money_sfx();
    String _format_time(double total_seconds);

protected:
    static void _bind_methods();

public:
    MainScreen();
    ~MainScreen();

    void _ready() override;
    void _process(double delta) override;

    void _attempt_sale_tick();
    void _advance_phase_tick();

    void _on_decrease_pressed();
    void _on_increase_pressed();
    void _on_mixing_pressed();
    void _on_tech_tree_pressed();
    void _on_return_menu_pressed();

    void _update_ui();
    void _update_time_icons();
    void _show_sale_feedback(String text, bool sold);
    void _show_gold_gain(int amount);
    void _show_gold_loss(int amount);
};

} // namespace godot

#endif // MAIN_SCREEN_H