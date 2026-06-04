#include "intro_cutscene.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

IntroCutscene::IntroCutscene() {
    label = nullptr;
    animation_player = nullptr;
    current_line = 0;

    intro_ambience = nullptr;
    button_click_sfx = nullptr;

    fading_out_music = false;
    music_fade_timer = 0.0;
    music_fade_duration = 1.5;
    music_start_volume_db = -12.0;
    pending_scene_path = "";
    skip_requested = false;
    skip_button = nullptr;
}

IntroCutscene::~IntroCutscene() {}

void IntroCutscene::_bind_methods() {
    // Bind the callback method so the AnimationPlayer signal can find it
    ClassDB::bind_method(D_METHOD("_on_animation_player_animation_finished", "anim_name"), &IntroCutscene::_on_animation_player_animation_finished);
    ClassDB::bind_method(D_METHOD("_on_skip_pressed"), &IntroCutscene::_on_skip_pressed);
    ClassDB::bind_method(D_METHOD("_on_skip_delay_timeout"), &IntroCutscene::_on_skip_delay_timeout);
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

    intro_ambience = Object::cast_to<AudioStreamPlayer>(get_node_or_null("IntroAmbience"));
    button_click_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("ButtonClickSFX"));

    if (!intro_ambience) {
        UtilityFunctions::printerr("IntroCutscene C++: IntroAmbience node not found.");
    }

    if (!button_click_sfx) {
        UtilityFunctions::printerr("IntroCutscene C++: ButtonClickSFX node not found.");
    }

    set_process(true);

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

    skip_button = get_node<Button>("CanvasLayer/MarginContainer/Skip");
    
    if (skip_button) {
        skip_button->connect("pressed", Callable(this, "_on_skip_pressed"));
    }

    // Kickstart the narrative crawl
    show_current_line();
}

void IntroCutscene::show_current_line() {
    if (current_line >= lines.size()) {
        pending_scene_path = "res://scenes/tutorial_scene.tscn";
        start_music_fade_out();
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
    if (skip_requested) return;

    skip_requested = true;
    play_button_click_sfx();

    if (skip_button) {
        skip_button->set_disabled(true);
    }

    pending_scene_path = "res://scenes/tutorial_scene.tscn";

    Ref<SceneTreeTimer> timer = get_tree()->create_timer(0.08);
    timer->connect("timeout", Callable(this, "_on_skip_delay_timeout"));
}

void IntroCutscene::_process(double delta) {
    if (!fading_out_music || !intro_ambience) return;

    music_fade_timer += delta;

    double t = music_fade_timer / music_fade_duration;
    if (t > 1.0) {
        t = 1.0;
    }

    double end_volume_db = -80.0;
    double current_volume_db = music_start_volume_db + ((end_volume_db - music_start_volume_db) * t);

    intro_ambience->set_volume_db(current_volume_db);

    if (t >= 1.0) {
        fading_out_music = false;
        go_to_pending_scene();
    }
}

void IntroCutscene::start_music_fade_out() {
    if (fading_out_music) return;

    if (intro_ambience) {
        music_start_volume_db = intro_ambience->get_volume_db();
    }

    music_fade_timer = 0.0;
    fading_out_music = true;
}

void IntroCutscene::play_button_click_sfx() {
    if (button_click_sfx) {
        button_click_sfx->play();
    }
}

void IntroCutscene::go_to_pending_scene() {
    if (pending_scene_path.is_empty()) return;

    SceneTree* tree = get_tree();
    if (tree) {
        tree->change_scene_to_file(pending_scene_path);
    }
}
void IntroCutscene::_on_skip_delay_timeout() {
    SceneTree* tree = get_tree();

    if (tree) {
        tree->change_scene_to_file("res://scenes/tutorial_scene.tscn");
    }
}

} // namespace godot