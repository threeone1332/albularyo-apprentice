#include "tutorial_scene.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

TutorialScene::TutorialScene() {
    slide_image = nullptr;
    back_button = nullptr;
    next_button = nullptr;
    skip_button = nullptr;
    start_button = nullptr;
    current_slide = 0;
}

TutorialScene::~TutorialScene() {}

void TutorialScene::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_next_pressed"), &TutorialScene::_on_next_pressed);
    ClassDB::bind_method(D_METHOD("_on_back_pressed"), &TutorialScene::_on_back_pressed);
    ClassDB::bind_method(D_METHOD("_on_skip_pressed"), &TutorialScene::_on_skip_pressed);
    ClassDB::bind_method(D_METHOD("_on_start_pressed"), &TutorialScene::_on_start_pressed);
}

void TutorialScene::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) return;

    slide_image = get_node<TextureRect>("SlideImage");
    back_button = get_node<TextureButton>("BackButton");
    next_button = get_node<TextureButton>("NextButton");
    skip_button = get_node<TextureButton>("SkipButton");
    start_button = get_node<TextureButton>("StartButton");

    if (!slide_image || !back_button || !next_button || !skip_button || !start_button) {
        UtilityFunctions::printerr("TutorialScene C++: Missing nodes.");
        return;
    }

    ResourceLoader *loader = ResourceLoader::get_singleton();

    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_01.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_02.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_03.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_04.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_05.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_06.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_07.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_08.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_09.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_10.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_11.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_12.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_13.png"));
    slides.push_back(loader->load("res://assets/ui/Tutorial Screen/tutorial_14.png"));

    next_button->connect("pressed", Callable(this, "_on_next_pressed"));
    back_button->connect("pressed", Callable(this, "_on_back_pressed"));
    skip_button->connect("pressed", Callable(this, "_on_skip_pressed"));
    start_button->connect("pressed", Callable(this, "_on_start_pressed"));

    update_slide();
}

void TutorialScene::update_slide() {
    slide_image->set_texture(slides[current_slide]);

    back_button->set_visible(current_slide > 0);

    if (current_slide == slides.size() - 1) {
        skip_button->set_visible(false);
        start_button->set_visible(true);
    } else {
        skip_button->set_visible(true);
        start_button->set_visible(false);
    }
}

void TutorialScene::_on_next_pressed() {
    if (current_slide < slides.size() - 1) {
        current_slide++;
        update_slide();
    } else {
        start_game();
    }
}

void TutorialScene::_on_back_pressed() {
    if (current_slide > 0) {
        current_slide--;
        update_slide();
    }
}

void TutorialScene::_on_skip_pressed() {
    current_slide = slides.size() - 1;
    update_slide();
}

void TutorialScene::_on_start_pressed() {
    start_game();
}

void TutorialScene::start_game() {
    get_tree()->change_scene_to_file("res://scenes/game_loading_screen.tscn");
}

}