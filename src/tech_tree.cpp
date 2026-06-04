#include "tech_tree.h"

#include <cmath>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

TechTree::TechTree() {}

TechTree::~TechTree() {}

void TechTree::_bind_methods() {
    // Bind methods so Godot signals can call these C++ functions.
    ClassDB::bind_method(D_METHOD("_on_exit_pressed"), &TechTree::_on_exit_pressed);

    ClassDB::bind_method(D_METHOD("buy_upgrade", "upgrade_id"), &TechTree::buy_upgrade);
    ClassDB::bind_method(D_METHOD("show_upgrade_hover", "upgrade_id"), &TechTree::show_upgrade_hover);
    ClassDB::bind_method(D_METHOD("clear_upgrade_hover"), &TechTree::clear_upgrade_hover);
    ClassDB::bind_method(D_METHOD("_on_stay_pressed"), &TechTree::_on_stay_pressed);
    ClassDB::bind_method(D_METHOD("_on_leave_pressed"), &TechTree::_on_leave_pressed);
}

void TechTree::_ready() {
    // Prevent gameplay code from running while viewing the scene in the editor.
    if (Engine::get_singleton()->is_editor_hint()) return;

    stop_main_game_music();

    // Your project already uses this autoload name.
    game_state = Object::cast_to<GameState>(get_node_or_null("/root/GlobalGameState"));

    // These names must match the nodes in tech_tree.tscn.
    money_label = Object::cast_to<Label>(get_node_or_null("MoneyLabel"));
    message_label = Object::cast_to<Label>(get_node_or_null("MessageLabel"));
    exit_confirm_overlay = Object::cast_to<Control>(get_node_or_null("ExitConfirmOverlay"));
    confirm_bat = Object::cast_to<TextureRect>(get_node_or_null("ExitConfirmOverlay/BatDecoration"));
    confirm_title = Object::cast_to<Label>(get_node_or_null("ExitConfirmOverlay/ConfirmTitle"));
    confirm_message = Object::cast_to<Label>(get_node_or_null("ExitConfirmOverlay/ConfirmMessage"));
    stay_button = Object::cast_to<Button>(get_node_or_null("ExitConfirmOverlay/StayButton"));
    leave_button = Object::cast_to<Button>(get_node_or_null("ExitConfirmOverlay/LeaveButton"));

    unlock_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("UnlockSFX"));
    button_click_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("ButtonClickSFX"));
    insufficient_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("InsufficientSFX"));

    if (!insufficient_sfx) {
        UtilityFunctions::printerr("TechTree C++: InsufficientSFX node not found.");
    }
    if (!unlock_sfx) {
        UtilityFunctions::printerr("TechTree C++: UnlockSFX node not found.");
    }

    if (!button_click_sfx) {
        UtilityFunctions::printerr("TechTree C++: ButtonClickSFX node not found.");
    }

    if (!game_state) {
        UtilityFunctions::printerr("TechTree C++: GlobalGameState autoload not found.");
    }

    if (!money_label) {
        UtilityFunctions::printerr("TechTree C++: MoneyLabel node not found.");
    }

    if (!message_label) {
        UtilityFunctions::printerr("TechTree C++: MessageLabel node not found.");
    }

    if (!exit_confirm_overlay) {
        UtilityFunctions::printerr("TechTree C++: ExitConfirmOverlay node not found.");
    }

    // Connect the exit button.
    exit_button = Object::cast_to<TextureButton>(get_node_or_null("exit"));

    if (exit_button) {
        exit_button->set_mouse_filter(MOUSE_FILTER_STOP);

        if (!exit_button->is_connected("pressed", Callable(this, "_on_exit_pressed"))) {
            exit_button->connect("pressed", Callable(this, "_on_exit_pressed"));
        }
    } else {
        UtilityFunctions::printerr("TechTree C++: exit button node not found.");
    }

    // Connect each tech tree button to the correct upgrade.
    connect_upgrade_button("cat", "cat_companion");
    connect_upgrade_button("bat", "bat_companion");
    connect_upgrade_button("crow", "murder_of_crows");
    connect_upgrade_button("sparkle", "awaken_anito");
    connect_upgrade_button("bag", "hire_adventurers");

    if (stay_button && !stay_button->is_connected("pressed", Callable(this, "_on_stay_pressed"))) {
        stay_button->connect("pressed", Callable(this, "_on_stay_pressed"));
    }

    if (leave_button && !leave_button->is_connected("pressed", Callable(this, "_on_leave_pressed"))) {
        leave_button->connect("pressed", Callable(this, "_on_leave_pressed"));
    }

    hide_exit_confirmation();
    set_process(true);

    update_money_label();
    show_message("");
}

void TechTree::_process(double delta) {
    if (!exit_confirm_overlay || !exit_confirm_overlay->is_visible()) return;

    if (confirm_bat) {
        bat_animation_time += delta;

        // A gentle fake wing-flap using one bat image.
        float flap = static_cast<float>(std::sin(bat_animation_time * 8.0));
        confirm_bat->set_scale(Vector2(1.0f + (flap * 0.025f), 1.0f - (flap * 0.055f)));
        confirm_bat->set_rotation(static_cast<float>(flap * 0.035f));
    }

    if (!typing_confirm_text) return;

    typewriter_timer += delta;
    if (typewriter_timer < 0.035) return;

    typewriter_timer = 0.0;

    if (title_visible_chars < confirm_title_text.length()) {
        title_visible_chars++;
        if (confirm_title) {
            confirm_title->set_text(confirm_title_text.substr(0, title_visible_chars));
        }
        return;
    }

    if (message_visible_chars < confirm_message_text.length()) {
        message_visible_chars++;
        if (confirm_message) {
            confirm_message->set_text(confirm_message_text.substr(0, message_visible_chars));
        }
        return;
    }

    finish_exit_confirmation_typing();
}

void TechTree::_on_exit_pressed() {
    play_button_click_sfx();
    show_exit_confirmation();
}

void TechTree::_on_stay_pressed() {
    play_button_click_sfx();
    hide_exit_confirmation();
}

void TechTree::_on_leave_pressed() {
    play_button_click_sfx();
    // Return to the main game screen.
    SceneTree* tree = get_tree();

    if (tree) {
        tree->change_scene_to_file("res://scenes/main_screen.tscn");
    }
}

void TechTree::connect_upgrade_button(String node_name, String upgrade_id) {
    // Find the TextureButton node in the scene.
    TextureButton* button = Object::cast_to<TextureButton>(get_node_or_null(NodePath(node_name)));

    if (!button) {
        UtilityFunctions::printerr("TechTree C++: upgrade button not found: ", node_name);
        return;
    }

    button->set_mouse_filter(MOUSE_FILTER_STOP);

    // One set of functions handles all buttons by passing the upgrade_id.
    Callable pressed_callable = Callable(this, "buy_upgrade").bind(upgrade_id);
    Callable hover_callable = Callable(this, "show_upgrade_hover").bind(upgrade_id);
    Callable exit_hover_callable = Callable(this, "clear_upgrade_hover");

    if (!button->is_connected("pressed", pressed_callable)) {
        button->connect("pressed", pressed_callable);
    }

    if (!button->is_connected("mouse_entered", hover_callable)) {
        button->connect("mouse_entered", hover_callable);
    }

    if (!button->is_connected("mouse_exited", exit_hover_callable)) {
        button->connect("mouse_exited", exit_hover_callable);
    }
}

int TechTree::get_upgrade_cost(String upgrade_id) const {
    // Upgrade prices based on your tech tree layout.
    if (upgrade_id == "cat_companion") return 200;
    if (upgrade_id == "bat_companion") return 300;
    if (upgrade_id == "murder_of_crows") return 600;
    if (upgrade_id == "awaken_anito") return 600;
    if (upgrade_id == "hire_adventurers") return 500;

    return 0;
}

String TechTree::get_upgrade_name(String upgrade_id) const {
    // Display names used in hover and purchase messages.
    if (upgrade_id == "cat_companion") return "Cat Companion";
    if (upgrade_id == "bat_companion") return "Bat Companion";
    if (upgrade_id == "murder_of_crows") return "Murder of Crows";
    if (upgrade_id == "awaken_anito") return "Awaken the Anito";
    if (upgrade_id == "hire_adventurers") return "Hire More Adventurers";

    return "Unknown Upgrade";
}

void TechTree::buy_upgrade(String upgrade_id) {
    
    if (!game_state) {
        show_message("GlobalGameState not found.");
        return;
    }

    int cost = get_upgrade_cost(upgrade_id);
    String upgrade_name = get_upgrade_name(upgrade_id);

    // Prevent buying the same upgrade twice.
    if (game_state->is_unlocked(upgrade_id)) {
        play_button_click_sfx();
        show_message(upgrade_name + " is already unlocked.");
        return;
    }

    // Check if the player has enough gold.
    if (game_state->get_gold() < cost) {
        play_insufficient_sfx();
        show_message(String("Cannot purchase ") + upgrade_name + ".");
        return;
    }

    // Subtract gold and apply the upgrade effect in GameState.
    game_state->set_gold(game_state->get_gold() - cost);
    game_state->apply_tech_unlock(upgrade_id);
    play_unlock_sfx();

    update_money_label();
    show_message(upgrade_name + " unlocked.");
}

void TechTree::show_upgrade_hover(String upgrade_id) {
    if (!game_state) {
        show_message("GlobalGameState not found.");
        return;
    }

    int cost = get_upgrade_cost(upgrade_id);
    String upgrade_name = get_upgrade_name(upgrade_id);

    if (game_state->is_unlocked(upgrade_id)) {
        show_message(upgrade_name + " is already unlocked.");
    } else if (game_state->get_gold() >= cost) {
        show_message(String("Click to unlock ") + upgrade_name + ". Cost: " + String::num_int64(cost));
    } else {
        show_message(String("Not enough gold for ") + upgrade_name + ". Cost: " + String::num_int64(cost));
    }

    update_money_label();
}

void TechTree::clear_upgrade_hover() {
    show_message("");
    update_money_label();
}

void TechTree::show_message(String message) {
    if (message_label) {
        message_label->set_text(message);
    } else if (!message.is_empty()) {
        UtilityFunctions::print(message);
    }
}

void TechTree::update_money_label() {
    if (!money_label) return;

    if (game_state) {
        // Show only the gold amount because the coin art already gives the context.
        money_label->set_text(String::num_int64(game_state->get_gold()));
    } else {
        money_label->set_text("--");
    }
}

void TechTree::show_exit_confirmation() {
    if (!exit_confirm_overlay) return;

    exit_confirm_overlay->set_visible(true);
    exit_confirm_overlay->move_to_front();

    title_visible_chars = confirm_title_text.length();
    message_visible_chars = confirm_message_text.length();
    typewriter_timer = 0.0;
    bat_animation_time = 0.0;
    typing_confirm_text = false;

    if (confirm_title) {
        confirm_title->set_text(confirm_title_text);
    }

    if (confirm_message) {
        confirm_message->set_text(confirm_message_text);
    }

    if (stay_button) {
        stay_button->set_visible(true);
    }

    if (leave_button) {
        leave_button->set_visible(true);
    }
}

void TechTree::hide_exit_confirmation() {
    typing_confirm_text = false;

    if (exit_confirm_overlay) {
        exit_confirm_overlay->set_visible(false);
    }

    if (confirm_bat) {
        confirm_bat->set_scale(Vector2(1.0f, 1.0f));
        confirm_bat->set_rotation(0.0f);
    }
}

void TechTree::finish_exit_confirmation_typing() {
    typing_confirm_text = false;

    if (stay_button) {
        stay_button->set_visible(true);
    }

    if (leave_button) {
        leave_button->set_visible(true);
    }
}
void TechTree::play_unlock_sfx() {
    if (unlock_sfx) {
        unlock_sfx->play();
    }
}

void TechTree::play_button_click_sfx() {
    if (button_click_sfx) {
        button_click_sfx->play();
    }
}

void TechTree::play_insufficient_sfx() {
    if (insufficient_sfx) {
        insufficient_sfx->play();
    }
}

void TechTree::stop_main_game_music() {
    AudioStreamPlayer *main_music = Object::cast_to<AudioStreamPlayer>(
        get_node_or_null("/root/MainGameMusic")
    );

    if (main_music) {
        main_music->stop();
        main_music->queue_free();
    }
}

}
