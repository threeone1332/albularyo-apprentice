#include "main_screen.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <ctime> // Fixed: Added for fallback initialization seeding

namespace godot {

MainScreen::MainScreen() {
    game_state = nullptr;
    main_background = nullptr;
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
    button_click_sfx = nullptr;
    money_sfx = nullptr;
    victory_sfx = nullptr;
    autosave_panel = nullptr;
    autosave_label = nullptr;
    autosave_id = 0;
    goal_tracker_container = nullptr;
    tech_goal_label = nullptr;
    gold_goal_label = nullptr;
    gold_goal_revealed = false;
    victory_overlay = nullptr;
    score_summary = nullptr;
    time_elapsed_label = nullptr;
    return_menu_button = nullptr;
    is_victory_triggered = false;
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
    ClassDB::bind_method(D_METHOD("_on_return_menu_pressed"), &MainScreen::_on_return_menu_pressed);
    ClassDB::bind_method(D_METHOD("_save_game_to_disk"), &MainScreen::_save_game_to_disk);
}

void MainScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) return;
    ensure_main_game_music();

    // FIX: Seed the pseudo-random generator with system time so numbers change every session
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    potion_icon_regions.push_back(Rect2(20, 20, 200, 200));
    potion_icon_regions.push_back(Rect2(260, 20, 200, 195));
    potion_icon_regions.push_back(Rect2(500, 20, 200, 195));
    potion_icon_regions.push_back(Rect2(740, 20, 150, 146));
    potion_icon_regions.push_back(Rect2(930, 20, 200, 200));
    potion_icon_regions.push_back(Rect2(1170, 20, 200, 200));
    potion_icon_regions.push_back(Rect2(1410, 20, 200, 200));

    game_state = get_node_or_null("/root/GlobalGameState");
    main_background = get_node<NinePatchRect>("MarginContainer/NinePatchRect");

    ResourceLoader* loader = ResourceLoader::get_singleton();
    bg_morning   = loader->load("res://assets/ui/Main Screen/Morning Main Screen.png");
    bg_afternoon = loader->load("res://assets/ui/Main Screen/Afternoon Main Screen.png");
    bg_night     = loader->load("res://assets/ui/Main Screen/Night Main Screen.png");

    money_label = get_node<Label>("MarginContainer/NinePatchRect/MoneyArea/Money/Label");
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

    autosave_panel = Object::cast_to<PanelContainer>(get_node_or_null("AutosaveNotification/PanelContainer"));
    autosave_label = Object::cast_to<Label>(get_node_or_null("AutosaveNotification/PanelContainer/HBoxContainer/AutosaveLabel"));

    goal_tracker_container = get_node<MarginContainer>("MarginContainer/NinePatchRect/GoalTrackerContainer");
    tech_goal_label = get_node<Label>("MarginContainer/NinePatchRect/GoalTrackerContainer/VBoxContainer/TechGoalLabel");
    gold_goal_label = get_node<Label>("MarginContainer/NinePatchRect/GoalTrackerContainer/VBoxContainer/GoldGoalLabel");

    button_click_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("ButtonClickSFX"));
    money_sfx = Object::cast_to<AudioStreamPlayer>(get_node_or_null("MoneySFX"));
    victory_sfx = Object::cast_to<AudioStreamPlayer>(
        get_node_or_null("VictorySFX")
    );

    if (!victory_sfx) {
        UtilityFunctions::printerr("MainScreen C++: VictorySFX node not found.");
    }
    victory_overlay = Object::cast_to<CanvasLayer>(get_node_or_null("VictoryOverlay"));
    if (victory_overlay) {
        score_summary = get_node<Label>("VictoryOverlay/CenterContainer/PanelLayout/VBoxContainer/ScoreSummary");
        time_elapsed_label = get_node<Label>("VictoryOverlay/CenterContainer/PanelLayout/VBoxContainer/TimeElapsedLabel");
        return_menu_button = get_node<TextureButton>("VictoryOverlay/CenterContainer/PanelLayout/VBoxContainer/ButtonSpacer/ReturnMenuButton");
    }

    if (!sale_feedback || !gain_label || !game_state) return;

    sale_feedback->set_visible(false);
    gain_label->set_text("");
    if (autosave_panel) autosave_panel->set_visible(false);

    sale_feedback_start_pos = sale_feedback->get_position();
    gain_label_start_pos = gain_label->get_position();

    decrease_button->connect("pressed", Callable(this, "_on_decrease_pressed"));
    increase_button->connect("pressed", Callable(this, "_on_increase_pressed"));
    mixing_button->connect("pressed", Callable(this, "_on_mixing_pressed"));
    tech_tree_button->connect("pressed", Callable(this, "_on_tech_tree_pressed"));
    change_button->connect("pressed", Callable(this, "_on_mixing_pressed"));

    if (return_menu_button) {
        return_menu_button->connect("pressed", Callable(this, "_on_return_menu_pressed"));
    }

    if (gold_goal_label) {
        gold_goal_label->set_modulate(Color(1, 1, 1, 0));
        gold_goal_label->set_visible(false);
    }

    if (victory_overlay) victory_overlay->set_visible(false);

    _update_ui();
    _update_time_icons();
}

void MainScreen::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) return;

    if (game_state) {
        double current_playtime = game_state->call("get_total_playtime");
        game_state->call("set_total_playtime", current_playtime + delta);
    }

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
    if (is_victory_triggered) return;

    bool has_featured = game_state->call("has_featured_stock");
    if (!has_featured) {
        _show_sale_feedback("NO POTION", false);
    } else {
        bool sold = game_state->call("attempt_sale");
        if (sold) {
            play_money_sfx();
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
    if (is_victory_triggered) return;

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
    _save_game_to_disk();
}

String MainScreen::_format_time(double total_seconds) {
    int total_secs_int = static_cast<int>(total_seconds);
    int minutes = total_secs_int / 60;
    int seconds = total_secs_int % 60;

    String min_str = (minutes < 10) ? "0" + UtilityFunctions::str(minutes) : UtilityFunctions::str(minutes);
    String sec_str = (seconds < 10) ? "0" + UtilityFunctions::str(seconds) : UtilityFunctions::str(seconds);

    return "TIME ELAPSED: " + min_str + ":" + sec_str;
}

void MainScreen::_check_victory_condition() {
    if (!game_state || is_victory_triggered) return;

    int current_gold = game_state->call("get_gold");

    int unlocked_count = 0;
    if (game_state->call("is_unlocked", "cat_companion")) unlocked_count++;
    if (game_state->call("is_unlocked", "bat_companion")) unlocked_count++;
    if (game_state->call("is_unlocked", "murder_of_crows")) unlocked_count++;
    if (game_state->call("is_unlocked", "awaken_anito")) unlocked_count++;
    if (game_state->call("is_unlocked", "hire_adventurers")) unlocked_count++;

    if (unlocked_count >= 5 && current_gold >= 1000) {
        is_victory_triggered = true;
        play_victory_sfx();

        if (score_summary) {
            score_summary->set_text("YOUR MASTER IS PROUD. YOU HAVE MASTERED THE ELEMENTS, GATHERED THE SECRETS, AND FILLED THE TREASURY.");
        }

        if (time_elapsed_label) {
            double global_time = game_state->call("get_total_playtime");
            time_elapsed_label->set_text(_format_time(global_time));
        }

        if (victory_overlay) victory_overlay->set_visible(true);
    }
}

void MainScreen::_on_return_menu_pressed() {
    play_button_click_sfx();
    get_tree()->change_scene_to_file("res://scenes/main_menu.tscn");
}

void MainScreen::_save_game_to_disk() {
    if (!game_state || is_victory_triggered) return;

    Dictionary save_dict = game_state->call("get_save_data");
    String json_string = JSON::stringify(save_dict);

    Ref<FileAccess> file = FileAccess::open("user://savegame.json", FileAccess::WRITE);
    if (file.is_valid()) {
        file->store_line(json_string);
        file->close();

        if (autosave_panel && autosave_label) {
            autosave_id++;
            autosave_panel->set_visible(true);
            autosave_panel->set_modulate(Color(1, 1, 1, 0));

            Ref<Tween> tween = create_tween();
            tween->tween_property(autosave_panel, "modulate:a", 1.0, 0.3);

            Ref<Tween> delay_tween = create_tween();
            delay_tween->tween_interval(1.5);
            delay_tween->tween_property(autosave_panel, "modulate:a", 0.0, 0.5);
            delay_tween->tween_callback(Callable(autosave_panel, "set_visible").bind(false));
        }
    }
}

void MainScreen::_on_decrease_pressed() {
    if (is_victory_triggered) return;
    play_button_click_sfx();
    int current_price = game_state->call("get_price");
    game_state->call("set_price", current_price - 1);
    _update_ui();
}

void MainScreen::_on_increase_pressed() {
    if (is_victory_triggered) return;
    play_button_click_sfx();

    int current_price = game_state->call("get_price");

    // FIX: Only increase the value if it has not reached the upper limit constraint of 100
    if (current_price < 100) {
        game_state->call("set_price", current_price + 1);
        _update_ui();
    }
}

void MainScreen::_update_ui() {
    if (!game_state) return;

    int current_gold = game_state->call("get_gold");
    money_label->set_text(UtilityFunctions::str(current_gold));

    int current_price = game_state->call("get_price");
    price_label->set_text(UtilityFunctions::str(current_price));

    String sell_percent = UtilityFunctions::str(game_state->call("get_sell_chance_percent"));
    sell_chance_label->set_text(sell_percent + "% PER SECOND");

    potion_name_label->set_text(game_state->call("get_featured_potion_name"));

    int potion_id = game_state->call("get_featured_potion_id");
    if (potion_id >= 0 && potion_id < potion_icon_regions.size()) {
        potion_icon->set_region_rect(potion_icon_regions[potion_id]);
    }

    int unlocked_count = 0;
    if (game_state->call("is_unlocked", "cat_companion")) unlocked_count++;
    if (game_state->call("is_unlocked", "bat_companion")) unlocked_count++;
    if (game_state->call("is_unlocked", "murder_of_crows")) unlocked_count++;
    if (game_state->call("is_unlocked", "awaken_anito")) unlocked_count++;
    if (game_state->call("is_unlocked", "hire_adventurers")) unlocked_count++;

    if (tech_goal_label) {
        tech_goal_label->set_text("Tech Tree Upgrades: " + UtilityFunctions::str(unlocked_count) + "/5");
        tech_goal_label->add_theme_color_override("font_color", (unlocked_count >= 5) ? Color::html("#6abe30") : Color::html("#ffffff"));
    }

    if (gold_goal_label) {
        gold_goal_label->set_text("Gold: " + UtilityFunctions::str(current_gold) + "/1000");

        if (unlocked_count >= 5) {
            if (!gold_goal_revealed) {
                gold_goal_revealed = true;
                gold_goal_label->set_visible(true);
                Ref<Tween> fade_tween = create_tween();
                fade_tween->tween_property(gold_goal_label, "modulate:a", 1.0, 1.2)->set_trans(Tween::TRANS_SINE)->set_ease(Tween::EASE_OUT);
            }
            gold_goal_label->add_theme_color_override("font_color", (current_gold >= 1000) ? Color::html("#6abe30") : Color::html("#ffffff"));
        } else {
            gold_goal_revealed = false;
            gold_goal_label->set_visible(false);
            gold_goal_label->set_modulate(Color(1, 1, 1, 0));
        }
    }

    _check_victory_condition();
}

void MainScreen::_update_time_icons() {
    String phase = game_state->call("get_current_phase");

    morning_icon->set_visible(phase == "sun");
    noon_icon->set_visible(phase == "noon");
    night_icon->set_visible(phase == "night");

    if (!main_background) return;

    if (phase == "sun" && bg_morning.is_valid()) main_background->set_texture(bg_morning);
    else if (phase == "noon" && bg_afternoon.is_valid()) main_background->set_texture(bg_afternoon);
    else if (phase == "night" && bg_night.is_valid()) main_background->set_texture(bg_night);
}

void MainScreen::_show_sale_feedback(String text, bool sold) {
    // FIX: Kill old active feedback animation immediately to prevent overlapping tween state blockages
    if (active_feedback_tween.is_valid() && active_feedback_tween->is_running()) {
        active_feedback_tween->kill();
    }

    feedback_id++;
    sale_feedback->set_text(text);
    sale_feedback->set_visible(true);
    sale_feedback->set_position(sale_feedback_start_pos);
    sale_feedback->set_modulate(Color(1.0, 1.0, 1.0, 1.0));
    sale_feedback->add_theme_color_override("font_color", sold ? Color::html("#da8b24") : Color::html("#f0dcb4"));

    Vector2 end_pos = sale_feedback_start_pos + Vector2(0, -18);
    active_feedback_tween = create_tween();
    active_feedback_tween->tween_property(sale_feedback, "position", end_pos, 0.4);

    Ref<Tween> alpha_tween = create_tween();
    alpha_tween->tween_property(sale_feedback, "modulate:a", 0.0, 0.4)->set_delay(0.4);
    alpha_tween->tween_callback(Callable(sale_feedback, "set_visible").bind(false));
}

void MainScreen::_show_gold_gain(int amount) {
    // FIX: Safely terminate running gain modifications
    if (active_gain_tween.is_valid() && active_gain_tween->is_running()) {
        active_gain_tween->kill();
    }

    gain_id++;
    gain_label->set_text("+" + UtilityFunctions::str(amount));
    gain_label->add_theme_color_override("font_color", Color::html("#D39B38"));
    gain_label->set_position(gain_label_start_pos);
    gain_label->set_modulate(Color(1, 1, 1, 1));
    gain_label->set_scale(Vector2(1.5, 1.5));

    Vector2 end_pos = gain_label_start_pos + Vector2(0, -25);
    active_gain_tween = create_tween();
    active_gain_tween->tween_property(gain_label, "position", end_pos, 0.5);

    Ref<Tween> alpha_tween = create_tween();
    alpha_tween->tween_property(gain_label, "modulate:a", 0.0, 0.4)->set_delay(0.4);
}

void MainScreen::_show_gold_loss(int amount) {
    if (active_gain_tween.is_valid() && active_gain_tween->is_running()) {
        active_gain_tween->kill();
    }

    gain_id++;
    gain_label->set_text("-" + UtilityFunctions::str(amount));
    gain_label->add_theme_color_override("font_color", Color::html("#8A5A52"));
    gain_label->set_position(gain_label_start_pos);
    gain_label->set_modulate(Color(1, 1, 1, 1));
    gain_label->set_scale(Vector2(1.5, 1.5));

    Vector2 end_pos = gain_label_start_pos + Vector2(0, -25);
    active_gain_tween = create_tween();
    active_gain_tween->tween_property(gain_label, "position", end_pos, 0.5);

    Ref<Tween> alpha_tween = create_tween();
    alpha_tween->tween_property(gain_label, "modulate:a", 0.0, 0.4)->set_delay(0.4);
}

void MainScreen::_on_mixing_pressed() {
    if (is_victory_triggered) return;
    play_button_click_sfx();
    get_tree()->change_scene_to_file("res://scenes/mixing_screen.tscn");
}

void MainScreen::_on_tech_tree_pressed() {
    if (is_victory_triggered) return;
    play_button_click_sfx();
    get_tree()->change_scene_to_file("res://scenes/tech_tree.tscn");
}

void MainScreen::ensure_main_game_music() {
    AudioStreamPlayer *main_music = Object::cast_to<AudioStreamPlayer>(
        get_node_or_null("/root/MainGameMusic")
    );

    // If the shared music player does not exist yet, create it.
    if (!main_music) {
        main_music = memnew(AudioStreamPlayer);
        main_music->set_name("MainGameMusic");

        Ref<AudioStream> music_stream = ResourceLoader::get_singleton()->load(
            "res://assets/Sounds/(Main Screen) Lobby-Time(chosic.com).mp3"
        );

        main_music->set_stream(music_stream);
        main_music->set_bus("Music");
        main_music->set_volume_db(-12.0);
        main_music->set_process_mode(Node::PROCESS_MODE_ALWAYS);

        get_tree()->get_root()->add_child(main_music);
    }

    // If it exists but is stopped, play it again.
    if (!main_music->is_playing()) {
        main_music->play();
    }
}

void MainScreen::play_button_click_sfx() {
    if (button_click_sfx) button_click_sfx->play();
}

void MainScreen::play_money_sfx() {
    if (money_sfx) money_sfx->play();
}

void MainScreen::play_victory_sfx() {
    if (victory_sfx) {
        victory_sfx->play();
    }
}
} // namespace godot