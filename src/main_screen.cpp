#include "main_screen.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

MainScreen::MainScreen() {
    game_state = nullptr;
    money_label = nullptr;
    gain_label = nullptr;
    sale_feedback = nullptr;
    morning_icon = nullptr;
    noon_icon = nullptr;
    night_icon = nullptr;
    decrease_button = nullptr;
    price_label = nullptr;
    increase_button = nullptr;
    sell_chance_label = nullptr;
    change_button = nullptr;
    potion_name_label = nullptr;
    tech_tree_button = nullptr;
    mixing_button = nullptr;
    potion_icon = nullptr;

    phase_timer = 0.0;
    sell_timer = 0.0;
    feedback_id = 0;
    gain_id = 0;
}

MainScreen::~MainScreen() {}

void MainScreen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_decrease_pressed"), &MainScreen::_on_decrease_pressed);
    ClassDB::bind_method(D_METHOD("_on_increase_pressed"), &MainScreen::_on_increase_pressed);
    ClassDB::bind_method(D_METHOD("_on_mixing_pressed"), &MainScreen::_on_mixing_pressed);
    ClassDB::bind_method(D_METHOD("_on_tech_tree_pressed"), &MainScreen::_on_tech_tree_pressed);
}

void MainScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) return;

    // 1. Establish custom texture coordinate indexing positions
    potion_icon_regions.push_back(Rect2(20, 20, 200, 200));   // Lunas ng Sigla
    potion_icon_regions.push_back(Rect2(260, 20, 200, 195));  // Lunas ng Lihim
    potion_icon_regions.push_back(Rect2(500, 20, 200, 195));  // Lunas ng Linaw
    potion_icon_regions.push_back(Rect2(740, 20, 150, 146));  // Lunas ng Himbing
    potion_icon_regions.push_back(Rect2(930, 20, 200, 200));  // Tinctura ng Bilis
    potion_icon_regions.push_back(Rect2(1170, 20, 200, 200)); // Lason
    potion_icon_regions.push_back(Rect2(1410, 20, 200, 200)); // Lunas ng Diwa

    // 2. Fetch Global Node reference setup
    game_state = get_node_or_null("/root/GlobalGameState");

    // 3. Bind UI Components securely
    money_label = get_node<Label>("MarginContainer/NinePatchRect/MoneyArea/Money/Label");

    // FIX: Fixed structural path mapping to include GainSlot
    gain_label = get_node<Label>("MarginContainer/NinePatchRect/MoneyArea/GainSlot/GainLabel");
    sale_feedback = get_node<Label>("SaleFeedback");

    morning_icon = get_node<TextureRect>("MarginContainer/NinePatchRect/Time/Morning");
    noon_icon = get_node<TextureRect>("MarginContainer/NinePatchRect/Time/Noon");
    night_icon = get_node<TextureRect>("MarginContainer/NinePatchRect/Time/Night");

    decrease_button = get_node<Button>("MarginContainer/NinePatchRect/Price/Slider/Decrease");
    price_label = get_node<Label>("MarginContainer/NinePatchRect/Price/Slider/Label");
    increase_button = get_node<Button>("MarginContainer/NinePatchRect/Price/Slider/Increase");
    sell_chance_label = get_node<Label>("MarginContainer/NinePatchRect/Price/Demand/Label2");

    change_button = get_node<Button>("MarginContainer/NinePatchRect/Feature/NinePatchRect/MarginContainer/Change");
    potion_name_label = get_node<Label>("MarginContainer/NinePatchRect/Feature/NinePatchRect/MarginContainer/MarginContainer/potionandname/nameofpotion");

    tech_tree_button = get_node<Button>("MarginContainer/NinePatchRect/Tech_tree_margin/Tech Tree");
    mixing_button = get_node<Button>("MarginContainer/NinePatchRect/Mixing_margin/Mixing");

    potion_icon = get_node<NinePatchRect>("MarginContainer/NinePatchRect/Feature/NinePatchRect/MarginContainer/MarginContainer/potionandname/NinePatchRect/MarginContainer/picofpotion");

    // 4. FIX: Detailed, verbose fallback validations to track down specific runtime issues
    if (!game_state) {
        UtilityFunctions::printerr("MainScreen C++ Error: GlobalGameState Autoload is missing from Project Settings!");
        return;
    }
    if (!sale_feedback) {
        UtilityFunctions::printerr("MainScreen C++ Error: Core node 'SaleFeedback' not found! Make sure it's a child of Main_screen.");
        return;
    }
    if (!gain_label) {
        UtilityFunctions::printerr("MainScreen C++ Error: Core node 'GainLabel' not found at the specified path!");
        return;
    }

    // 5. Initial layout state adjustments
    sale_feedback->set_visible(false);
    gain_label->set_text("");
    gain_label->set_visible(true);

    sale_feedback_start_pos = sale_feedback->get_position();
    gain_label_start_pos = gain_label->get_position();

    // 6. Hook signals safely
    decrease_button->connect("pressed", Callable(this, "_on_decrease_pressed"));
    increase_button->connect("pressed", Callable(this, "_on_increase_pressed"));
    mixing_button->connect("pressed", Callable(this, "_on_mixing_pressed"));
    tech_tree_button->connect("pressed", Callable(this, "_on_tech_tree_pressed"));
    change_button->connect("pressed", Callable(this, "_on_mixing_pressed"));

    _update_ui();
    _update_time_icons();
}

void MainScreen::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) return;

    phase_timer += delta;
    sell_timer += delta;

    if (sell_timer >= SELL_INTERVAL) {
        sell_timer = 0.0;
        _attempt_sale_tick();
    }

    if (phase_timer >= PHASE_DURATION) {
        phase_timer = 0.0;
        _advance_phase_tick();
    }
}

void MainScreen::_attempt_sale_tick() {
    bool has_featured = game_state->call("has_featured_stock");
    if (!has_featured) {
        _show_sale_feedback("NO POTION", false);
    } else {
        bool sold = game_state->call("attempt_sale");
        if (sold) {
            int gained = game_state->call("get_price");
            _show_sale_feedback("SOLD!", true);
            _show_gold_gain(gained);
        } else {
            _show_sale_feedback("NO SALE", false);
        }
    }
    _update_ui();
}

void MainScreen::_advance_phase_tick() {
    int old_gold = game_state->call("get_gold");

    game_state->call("advance_phase");

    int new_gold = game_state->call("get_gold");
    int difference = new_gold - old_gold;

    if (difference < 0) {
        _show_sale_feedback("ADVENTURERS FEE", false);
        _show_gold_loss(UtilityFunctions::abs(difference));
    }

    _update_ui();
    _update_time_icons();
}

void MainScreen::_on_decrease_pressed() {
    int current_price = game_state->call("get_price");
    game_state->call("set_price", current_price - 1);
    _update_ui();
}

void MainScreen::_on_increase_pressed() {
    int current_price = game_state->call("get_price");
    game_state->call("set_price", current_price + 1);
    _update_ui();
}

void MainScreen::_update_ui() {
    if (!game_state) return;

    money_label->set_text(UtilityFunctions::str(game_state->call("get_gold")));

    int current_price = game_state->call("get_price");
    price_label->set_text(UtilityFunctions::str(current_price));

    String sell_percent = UtilityFunctions::str(game_state->call("get_sell_chance_percent"));
    sell_chance_label->set_text(sell_percent + "% PER SECOND");

    potion_name_label->set_text(game_state->call("get_featured_potion_name"));

    int potion_id = game_state->call("get_featured_potion_id");
    if (potion_id >= 0 && potion_id < potion_icon_regions.size()) {
        potion_icon->set_region_rect(potion_icon_regions[potion_id]);
    }
}

void MainScreen::_update_time_icons() {
    String phase = game_state->call("get_current_phase");

    morning_icon->set_visible(phase == "sun");
    noon_icon->set_visible(phase == "noon");
    night_icon->set_visible(phase == "night");
}

// --- TWEEN VISUAL EFFECTS CODE ---

void MainScreen::_show_sale_feedback(String text, bool sold) {
    feedback_id++;

    sale_feedback->set_text(text);
    sale_feedback->set_visible(true);
    sale_feedback->set_position(sale_feedback_start_pos);
    sale_feedback->set_modulate(Color(1.0, 1.0, 1.0, 1.0));

    if (sold) {
        sale_feedback->add_theme_color_override("font_color", Color::html("#A97839"));
    } else {
        sale_feedback->add_theme_color_override("font_color", Color::html("#8A5A52"));
    }

    Vector2 end_pos = sale_feedback_start_pos + Vector2(0, -18);

    Ref<Tween> tween = create_tween();
    tween->tween_interval(0.25);
    tween->set_parallel(true);
    tween->tween_property(sale_feedback, "position", end_pos, 0.9);
    tween->tween_property(sale_feedback, "modulate:a", 0.0, 0.9);
}

void MainScreen::_show_gold_gain(int amount) {
    gain_id++;

    gain_label->set_text("+" + UtilityFunctions::str(amount));
    gain_label->add_theme_color_override("font_color", Color::html("#D39B38"));

    gain_label->set_position(gain_label_start_pos);
    gain_label->set_modulate(Color(1, 1, 1, 1));
    gain_label->set_scale(Vector2(1.5, 1.5));

    Vector2 end_pos = gain_label_start_pos + Vector2(0, -25);

    Ref<Tween> tween = create_tween();
    tween->set_parallel(true);
    tween->tween_property(gain_label, "position", end_pos, 0.8);
    tween->tween_property(gain_label, "modulate:a", 0.0, 0.8);
}

void MainScreen::_show_gold_loss(int amount) {
    gain_id++;

    gain_label->set_text("-" + UtilityFunctions::str(amount));
    gain_label->add_theme_color_override("font_color", Color::html("#8A5A52"));

    gain_label->set_position(gain_label_start_pos);
    gain_label->set_modulate(Color(1, 1, 1, 1));
    gain_label->set_scale(Vector2(1.5, 1.5));

    Vector2 end_pos = gain_label_start_pos + Vector2(0, -25);

    Ref<Tween> tween = create_tween();
    tween->set_parallel(true);
    tween->tween_property(gain_label, "position", end_pos, 0.8);
    tween->tween_property(gain_label, "modulate:a", 0.0, 0.8);
}

void MainScreen::_on_mixing_pressed() {
    get_tree()->change_scene_to_file("res://scenes/mixing_screen.tscn");
}

void MainScreen::_on_tech_tree_pressed() {
    get_tree()->change_scene_to_file("res://scenes/tech_tree.tscn");
}

} //