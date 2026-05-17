#pragma once
#include <godot_cpp/classes/node.hpp>
#include "Potion.h"
 
using namespace godot;
 
enum class Phase { SUN, NOON, NIGHT };
 
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
    float get_demand_modifier() const;
 
protected:
    static void _bind_methods();
 
public:
    GameState();
    void set_gold(int amount);    int get_gold() const;
    void add_gold(int amount);
    void advance_phase();
    String get_current_phase() const;
    int get_game_phase() const;
    void set_price(float p);      float get_price() const;
    float calculate_sell_chance(float demand, float price);
    bool attempt_sale();
    void apply_tech_unlock(String unlock_name);
    bool is_unlocked(String unlock_name) const;
    bool get_adventurers_unlocked() const;
    void unlock_adventurers();
    void check_phase_progression();
    bool check_win_condition() const;
};
