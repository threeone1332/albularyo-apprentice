#ifndef SETTINGS_H
#define SETTINGS_H

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/base_button.hpp>

namespace godot {

class Settings : public CanvasLayer {
    GDCLASS(Settings, CanvasLayer)

private:
    bool is_paused;
    Node *bound_scene = nullptr;

    // UI Confirmation References
    Control *confirmation_panel = nullptr;
    BaseButton *check_button = nullptr;
    BaseButton *x_button = nullptr;
    BaseButton *click_blocker = nullptr;

    // State Tracking Flags: 0 = None, 1 = Main Menu, 2 = Hard Wipe & Restart
    int pending_action_type = 0;

    void set_paused(bool p_state);
    void try_bind_settings_button(Node *current_scene);

protected:
    static void _bind_methods();

public:
    Settings();
    ~Settings();

    virtual void _ready() override;
    virtual void _process(double delta) override;
    virtual void _unhandled_input(const Ref<InputEvent> &event) override;

    // Button Pressed Observers
    void open_settings_menu();
    void _on_resume_pressed();
    void _on_main_menu_pressed();
    void _on_restart_pressed();

    // Dialog Confirmation Callbacks
    void _on_confirm_yes_pressed();
    void _on_confirm_no_pressed();

    // Linear Bus Sound Mixers
    void _on_music_slider_value_changed(double value);
    void _on_sfx_slider_value_changed(double value);
};

} // namespace godot

#endif // SETTINGS_H