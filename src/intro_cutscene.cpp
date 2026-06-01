#include "intro_cutscene.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

IntroCutscene::IntroCutscene() {
    label = nullptr;
    animation_player = nullptr;
    current_line = 0;
}

IntroCutscene::~IntroCutscene() {}

void IntroCutscene::_bind_methods() {
    // Bind the callback method so the AnimationPlayer signal can find it
    ClassDB::bind_method(D_METHOD("_on_animation_player_animation_finished", "anim_name"), &IntroCutscene::_on_animation_player_animation_finished);
    ClassDB::bind_method(D_METHOD("_on_skip_pressed"), &IntroCutscene::_on_skip_pressed);
}

void IntroCutscene::_ready() {
    // Escape early if running inside the viewport editor frame
    if (Engine::get_singleton()->is_editor_hint()) return;

    // Define story line sequences
    lines.push_back("Your Master...");
    lines.push_back("...");
    lines.push_back("She has perished.");
    lines.push_back("Her remedies are running low.");
    lines.push_back("The people are still coming.");
    lines.push_back("You are not ready.");
    lines.push_back("But you are all they have.");

    // Extract runtime node locations safely
    label = get_node<Label>("CanvasLayer/Label");
    animation_player = get_node<AnimationPlayer>("CanvasLayer/AnimationPlayer");

    if (!label || !animation_player) {
        UtilityFunctions::printerr("IntroCutscene C++: Missing critical UI child nodes!");
        return;
    }

    // Set initial text transparency to 0 (completely transparent alpha)
    label->set_modulate(Color(1.0, 1.0, 1.0, 0.0));

    // Connect the finish trigger signal
    if (!animation_player->is_connected("animation_finished", Callable(this, "_on_animation_player_animation_finished"))) {
        animation_player->connect("animation_finished", Callable(this, "_on_animation_player_animation_finished"));
    }

    Button* skip_button = get_node<Button>("CanvasLayer/MarginContainer/Skip");
    
    if (skip_button) {
        skip_button->connect("pressed", Callable(this, "_on_skip_pressed"));
    }

    // Kickstart the narrative crawl
    show_current_line();
}

void IntroCutscene::show_current_line() {
    if (current_line >= lines.size()) {
        SceneTree* tree = get_tree();
        if (tree) {
            // Cut introduction completed -> Route into your loading layer
            tree->change_scene_to_file("res://scenes/game_loading_screen.tscn");
        }
        return;
    }

    // Assign current string step and fire the viewport viewport animation layer
    label->set_text(lines[current_line]);
    animation_player->play("fade_in_out");
}

void IntroCutscene::_on_animation_player_animation_finished(String anim_name) {
    // Advance line tracker when the current text sequence finishes its fade out sequence
    current_line++;
    show_current_line();
}

void IntroCutscene::_on_skip_pressed() {
    SceneTree* tree = get_tree();

    if (tree) {
        tree->change_scene_to_file("res://scenes/game_loading_screen.tscn");
    }
}

} // namespace godot