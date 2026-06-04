#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot {

class GameState : public Node {
    GDCLASS(GameState, Node);

public:
    enum class Phase {
        SUN,
        NOON,
        NIGHT
    };

    struct PotionDemand {
        String name;
        int morning_demand;
        int noon_demand;
        int night_demand;
    };

private:
    int gold;
    Phase current_phase;
    float price_slider_value;
    bool adventurers_unlocked;
    int game_phase;

    bool cat_companion;
    bool bat_companion;
    bool murder_of_crows;
    bool awaken_anito;
    bool hire_adventurers;

    int featured_potion_id;
    double total_playtime; // Saved global persistent time tracker

    int player_inventory[8];
    int player_potions[7];
    PotionDemand potion_demands[7];

    float get_demand_modifier() const;
    int get_current_potion_demand() const;

protected:
    static void _bind_methods();

public:
    GameState();
    ~GameState() = default;

    // Core state mechanics
    void reset_state();
    void advance_phase();
    void start_new_day();
    void gather_phase_resources();
    void check_phase_progression();
    bool check_win_condition() const;

    // Gold modifiers
    void set_gold(int amount);
    int get_gold() const;
    void add_gold(int amount);

    // Persistent Game Clock tracking endpoints
    double get_total_playtime() const;
    void set_total_playtime(double time);

    // Phase management values
    String get_current_phase() const;
    int get_game_phase() const;

    // Sales metrics calculations
    void set_price(float p);
    float get_price() const;
    int get_sell_chance_percent() const;
    float calculate_sell_chance(float demand, float price) const;
    bool attempt_sale();
    bool has_featured_stock() const;

    // Upgrades bindings
    void apply_tech_unlock(String unlock_name);
    bool is_unlocked(String unlock_name) const;
    bool get_adventurers_unlocked() const;
    void unlock_adventurers();

    // Inventory & Item checks
    int get_ingredient_count(int id) const;
    void set_ingredient_count(int id, int amount);
    void add_ingredient(int id, int amount);

    int get_potion_count(int id) const;
    void set_potion_count(int id, int amount);
    void add_potion(int id, int amount);

    // Active Feature Item Configuration
    void set_featured_potion_id(int id);
    int get_featured_potion_id() const;
    String get_featured_potion_name() const;

    // Save and Load Dictionary bindings
    Dictionary get_save_data();
    void load_save_data(Dictionary data);
};

} // namespace godot

#endif // GAMESTATE_H