#ifndef SETTINGS_H
#define SETTINGS_H

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event.hpp>

namespace godot {

class Settings : public CanvasLayer {
    GDCLASS(Settings, CanvasLayer)

private:
    bool is_paused;
    void set_paused(bool p_state);

protected:
    static void _bind_methods();

public:
    Settings();
    ~Settings();

    virtual void _ready() override;
    virtual void _unhandled_input(const Ref<InputEvent> &event) override;

    // Button Action Listeners
    void _on_resume_pressed();
    void _on_main_menu_pressed();
    void _on_restart_pressed();
    
    // Volume Slider Listeners
    void _on_music_slider_value_changed(double value);
    void _on_sfx_slider_value_changed(double value);
};

} // namespace godot

#endif // SETTINGS_H