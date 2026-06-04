#ifndef TUTORIAL_SCENE_H
#define TUTORIAL_SCENE_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/classes/input_event.hpp>

namespace godot {

class TutorialScene : public Control {
    GDCLASS(TutorialScene, Control);

private:
    TextureRect *slide_image;
    TextureButton *back_button;
    TextureButton *next_button;
    TextureButton *skip_button;
    TextureButton *start_button;
    AudioStreamPlayer *button_click_sfx;
    String pending_scene_path;

    Vector<Ref<Texture2D>> slides;
    int current_slide;
    void play_button_click_sfx();

protected:
    static void _bind_methods();

public:
    TutorialScene();
    ~TutorialScene();

    void _ready() override;
    void update_slide();

    void _on_next_pressed();
    void _on_back_pressed();
    void _on_skip_pressed();
    void _on_start_pressed();


    void start_game();
    void _on_start_delay_timeout();
    void _unhandled_input(const Ref<InputEvent> &event) override;
};

}

#endif