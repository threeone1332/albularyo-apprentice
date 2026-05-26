#pragma once
#include <godot_cpp/classes/node.hpp>
#include "Potion.h"
 
using namespace godot;
 
enum class Phase { SUN, NOON, NIGHT };

struct PotionDemand {
    String name;
    int morning_demand;
    int noon_demand;
    int night_demand;
};
 
class GameState : public Node {
    GDCLASS(GameState, Node)
 
private:
    int gold;
    Phase current_phase;
    float price_slider_value;
    bool adventurers_unlocked;
    int game_phase;

    bool cat_companion, bat_companion;
    bool murder_of_crows, awaken_anito, hire_adventurers;

    int featured_potion_id;
    PotionDemand potion_demands[7];

    float get_demand_modifier() const;
    int get_current_potion_demand() const;

protected:
    static void _bind_methods();
 
public:
    GameState();

        //money

    void set_gold(int amount);    
    int get_gold() const;
    void add_gold(int amount);

        // phase
    void advance_phase();
    String get_current_phase() const;
    int get_game_phase() const;

        // price
    void set_price(float p);      
    float get_price() const;

        //demand
    float calculate_sell_chance(float demand, float price);
    int get_sell_chance_percent() const;
    bool attempt_sale();

        //featured
    void set_featured_potion_id(int id);
    int get_featured_potion_id() const;
    String get_featured_potion_name() const;

        //techtree
    void apply_tech_unlock(String unlock_name);
    bool is_unlocked(String unlock_name) const;
    bool get_adventurers_unlocked() const;
    void unlock_adventurers();

        //ending
    void check_phase_progression();
    bool check_win_condition() const;
};
