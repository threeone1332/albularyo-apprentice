#ifndef INTRO_CUTSCENE_H
#define INTRO_CUTSCENE_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/button.hpp>

namespace godot {

class IntroCutscene : public Node2D {
    GDCLASS(IntroCutscene, Node2D);

private:
    Label* label;
    AnimationPlayer* animation_player;

    Vector<String> lines;
    int current_line;

protected:
    static void _bind_methods();

public:
    IntroCutscene();
    ~IntroCutscene();

    void _ready() override;
    void show_current_line();

    // Animation signal callback
    void _on_animation_player_animation_finished(String anim_name);
    
    void _on_skip_pressed();
};

} // namespace godot

#endif // INTRO_CUTSCENE_H