#include "tech_tree.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

TechTree::TechTree() {
    exit_button = nullptr;
}

TechTree::~TechTree() {}

void TechTree::_bind_methods() {
    // Bind the callback method so the Godot engine signal can communicate with C++
    ClassDB::bind_method(D_METHOD("_on_exit_pressed"), &TechTree::_on_exit_pressed);
}

void TechTree::_ready() {
    // Gracefully exit if this is running inside the editor view
    if (Engine::get_singleton()->is_editor_hint()) return;

    // Fetch the exit button node based on your scene tree layout
    exit_button = get_node<TextureButton>("exit");

    if (exit_button) {
        // Set mouse filter explicitly to ensure it intercepts clicks perfectly
        exit_button->set_mouse_filter(MOUSE_FILTER_STOP);

        // Connect the button pressed signal to our C++ method programmatically
        if (!exit_button->is_connected("pressed", Callable(this, "_on_exit_pressed"))) {
            exit_button->connect("pressed", Callable(this, "_on_exit_pressed"));
        }
    } else {
        UtilityFunctions::printerr("TechTree C++: 'exit' button node not found!");
    }
}

void TechTree::_on_exit_pressed() {
    SceneTree* tree = get_tree();
    if (tree) {
        // Route the player cleanly back to your main screen scene
        tree->change_scene_to_file("res://scenes/main_screen.tscn");
    }
}

} // namespace godot