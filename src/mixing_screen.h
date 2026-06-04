#ifndef MIXING_SCREEN_H
#define MIXING_SCREEN_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/templates/vector.hpp>
#include "Gamestate.h"

namespace godot {

struct RecipeItem {
    int ingredient_id;
    int amount;
};

struct PotionRecipe {
    String name;
    Vector<RecipeItem> requirements;
};

class MixingScreen : public Control {
    GDCLASS(MixingScreen, Control);

private:
    // Core Data Models
    int player_inventory[8];      // Ingredients 0-7
    int player_potions[7];        // Potions 0-6
    int planned_craft_counts[7];  // Counter tracking values for [-] [1x] [+]
    GameState* game_state;

    Vector<PotionRecipe> recipes;

    // UI Nodes Arrays Mapping Caches
    Vector<Label*> ingredient_stock_labels;
    Vector<Label*> potion_stock_labels;
    Vector<Label*> craft_counter_labels;

    void initialize_recipes();
    void cache_ui_references();
    void update_ui_displays();
    bool can_increase_craft(int potion_idx);

protected:
    static void _bind_methods();

public:
    MixingScreen();
    ~MixingScreen();

    void _ready() override;

    // Engine Interface Callback Signals
    void _on_close_pressed();
    void _on_quantity_changed(int potion_idx, int delta);
    void _on_brew_pressed();
    void _on_feature_potion_pressed(int potion_idx);
};

}

#endif
