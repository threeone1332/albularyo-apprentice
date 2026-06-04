#include "mixing_screen.h"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include "Gamestate.h"

namespace godot {

void MixingScreen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_close_pressed"), &MixingScreen::_on_close_pressed);
    ClassDB::bind_method(D_METHOD("_on_quantity_changed", "potion_idx", "delta"), &MixingScreen::_on_quantity_changed);
    ClassDB::bind_method(D_METHOD("_on_brew_pressed"), &MixingScreen::_on_brew_pressed);
    ClassDB::bind_method(D_METHOD("_on_feature_potion_pressed", "potion_idx"), &MixingScreen::_on_feature_potion_pressed);
}

MixingScreen::MixingScreen() {
    game_state = nullptr;

    for (int i = 0; i < 7; i++) {
        planned_craft_counts[i] = 0;
    }
    initialize_recipes();
}

MixingScreen::~MixingScreen() {}

void MixingScreen::initialize_recipes() {
    recipes.resize(7);

        // Lunas ng Sigla
        recipes.write[0].name = "Lunas ng Sigla";
        recipes.write[0].requirements.push_back({0, 2}); // Bulaklak ng Sigla
        recipes.write[0].requirements.push_back({3, 1}); // Butil ng Araw

        // Lunas ng Lihim
        recipes.write[1].name = "Lunas ng Lihim";
        recipes.write[1].requirements.push_back({4, 2}); // Kabute
        recipes.write[1].requirements.push_back({5, 1}); // Puso ng Dilim

        // Lunas ng Linaw
        recipes.write[2].name = "Lunas ng Linaw";
        recipes.write[2].requirements.push_back({6, 2}); // Paru-paro
        recipes.write[2].requirements.push_back({7, 1}); // Biyak ng Tala

        // Lunas ng Himbing
        recipes.write[3].name = "Lunas ng Himbing";
        recipes.write[3].requirements.push_back({2, 2}); // Bulaklak ng Himbing
        recipes.write[3].requirements.push_back({4, 1}); // Kabute

        // Tinctura ng Bilis
        recipes.write[4].name = "Tinctura ng Bilis";
        recipes.write[4].requirements.push_back({3, 2}); // Butil ng Araw
        recipes.write[4].requirements.push_back({6, 1}); // Paru-paro

        // Lason
        recipes.write[5].name = "Lason";
        recipes.write[5].requirements.push_back({4, 3}); // Kabute
        recipes.write[5].requirements.push_back({7, 1}); // Biyak ng Tala

        // Lunas ng Diwa
        recipes.write[6].name = "Lunas ng Diwa";
        recipes.write[6].requirements.push_back({1, 2}); // Hibla ng Diwa
        recipes.write[6].requirements.push_back({0, 1}); // Bulaklak ng Sigla
}

Node* find_node_by_name_recursive(Node* current, const String& target_name) {
    if (current == nullptr) return nullptr;
    if (current->get_name() == target_name) return current;

    for (int i = 0; i < current->get_child_count(); i++) {
        Node* found = find_node_by_name_recursive(current->get_child(i), target_name);
        if (found != nullptr) return found;
    }
    return nullptr;
}

void MixingScreen::_ready() {

    game_state = get_node<GameState>(
        NodePath("/root/GlobalGameState")
    );

    if (game_state == nullptr) {
        UtilityFunctions::print("ERROR: GlobalGameState not found.");
    }

    cache_ui_references();
    update_ui_displays();

    TextureButton* close_btn = Object::cast_to<TextureButton>(find_node_by_name_recursive(this, "CloseButton"));
    if (close_btn) {
        close_btn->connect("pressed", Callable(this, "_on_close_pressed"));
    }
    TextureButton* brew_btn = Object::cast_to<TextureButton>(find_node_by_name_recursive(this, "BrewButton"));
    if (brew_btn) {
        brew_btn->connect("pressed", Callable(this, "_on_brew_pressed"));
    }
    for (int i = 0; i < 7; i++) {
        String r_num = (i == 0) ? String("") : String::num_int64(i + 1);
        Node* row_node = find_node_by_name_recursive(this, "PotionRow" + r_num);
        if (row_node) {
            TextureButton* minus_btn = Object::cast_to<TextureButton>(find_node_by_name_recursive(row_node, "MinusButton"));
            TextureButton* plus_btn = Object::cast_to<TextureButton>(find_node_by_name_recursive(row_node, "PlusButton"));

            if (minus_btn) {
                minus_btn->connect("pressed", Callable(this, "_on_quantity_changed").bind(i, -1));
            }
            if (plus_btn) {
                plus_btn->connect("pressed", Callable(this, "_on_quantity_changed").bind(i, 1));
            }

            TextureButton* feature_btn = Object::cast_to<TextureButton>(
                find_node_by_name_recursive(row_node, "FeatureButton")            );

            if (feature_btn) {
                feature_btn->connect(
                    "pressed",
                    Callable(this, "_on_feature_potion_pressed").bind(i)
                );
            }
        }
    }
}

void MixingScreen::cache_ui_references() {
    ingredient_stock_labels.clear();
    potion_stock_labels.clear();
    craft_counter_labels.clear();

    for (int i = 1; i <= 8; i++) {
        String i_num = (i == 1) ? String("") : String::num_int64(i);
        Node* row_node = find_node_by_name_recursive(this, "IngredientRow" + i_num);
        if (row_node) {
            Label* lbl = Object::cast_to<Label>(find_node_by_name_recursive(row_node, "InventoryCount"));
            ingredient_stock_labels.push_back(lbl);
        } else {
            ingredient_stock_labels.push_back(nullptr);
        }
    }

    for (int i = 0; i < 7; i++) {
        String r_num = (i == 0) ? String("") : String::num_int64(i + 1);
        Node* row_node = find_node_by_name_recursive(this, "PotionRow" + r_num);

        if (row_node) {
            Label* p_stock = Object::cast_to<Label>(find_node_by_name_recursive(row_node, "StockCount"));
            Label* c_counter = Object::cast_to<Label>(find_node_by_name_recursive(row_node, "CraftCounter"));

            potion_stock_labels.push_back(p_stock);
            craft_counter_labels.push_back(c_counter);
        } else {
            potion_stock_labels.push_back(nullptr);
            craft_counter_labels.push_back(nullptr);
        }
    }
}

void MixingScreen::update_ui_displays() {
    if (game_state == nullptr) return;

    for (int i = 0; i < 8; i++) {
        if (ingredient_stock_labels[i] != nullptr) {
            ingredient_stock_labels[i]->set_text(
                String::num_int64(game_state->get_ingredient_count(i)) + "x"
            );
        }
    }

    for (int i = 0; i < 7; i++) {
        if (potion_stock_labels[i] != nullptr) {
            potion_stock_labels[i]->set_text(
                String::num_int64(game_state->get_potion_count(i)) + "x"
            );
        }

        if (craft_counter_labels[i] != nullptr) {
            craft_counter_labels[i]->set_text(
                String::num_int64(planned_craft_counts[i]) + "x"
            );
        }
    }
}

bool MixingScreen::can_increase_craft(int potion_idx) {
    if (game_state == nullptr) return false;
    int simulated_usage[8] = {0};
    for (int p = 0; p < 7; p++) {
        int count = planned_craft_counts[p] + (p == potion_idx ? 1 : 0);
        for (const auto& req : recipes[p].requirements) {
            simulated_usage[req.ingredient_id] += req.amount * count;
        }
    }
    for (int i = 0; i < 8; i++) {
        if (simulated_usage[i] > game_state->get_ingredient_count(i)) {
            return false;
        }
    }
    return true;
}

void MixingScreen::_on_quantity_changed(int potion_idx, int delta) {
    UtilityFunctions::print("C++ Click Input Detected! Row Index: ", potion_idx, " | Action: ", delta);

    if (delta > 0) {
        if (can_increase_craft(potion_idx)) {
            planned_craft_counts[potion_idx]++;
        } else {
            UtilityFunctions::print("Craft blocked: Insufficient inventory ingredients.");
        }
    } else if (delta < 0 && planned_craft_counts[potion_idx] > 0) {
        planned_craft_counts[potion_idx]--;
    }

    update_ui_displays();
}

void MixingScreen::_on_brew_pressed() {
    if (game_state == nullptr) return;
    UtilityFunctions::print("Processing brewing queue selection...");
    bool brewed_anything = false;
    for (int p = 0; p < 7; p++) {
        int craft_qty = planned_craft_counts[p];
        if (craft_qty <= 0) continue;
        brewed_anything = true;
        for (const auto& req : recipes[p].requirements) {
            game_state->add_ingredient(
                req.ingredient_id,
                -(req.amount * craft_qty)
            );
        }
        game_state->add_potion(p, craft_qty);
        planned_craft_counts[p] = 0;
    }
    if (brewed_anything) {
        update_ui_displays();
        UtilityFunctions::print("Alchemy success! GameState inventory updated.");
    } else {
        UtilityFunctions::print("Brewing canceled: No items in planning queue.");
    }
}

void MixingScreen::_on_close_pressed() {
    SceneTree* tree = get_tree();

    if (tree != nullptr) {
        tree->call_deferred(
            "change_scene_to_file",
            "res://scenes/main_screen.tscn"
        );
    }
}

void MixingScreen::_on_feature_potion_pressed(int potion_idx) {

    UtilityFunctions::print("FEATURE BUTTON CLICKED");

    if (game_state == nullptr) return;

    game_state->set_featured_potion_id(potion_idx);

    UtilityFunctions::print(
        "Featured potion changed to: ",
        game_state->get_featured_potion_name()
    );

    SceneTree* tree = get_tree();

    if (tree != nullptr) {
        tree->call_deferred(
            "change_scene_to_file",
            "res://scenes/main_screen.tscn"
        );
    }
    
}

}
