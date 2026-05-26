#include "GameState.h"
#include <cstdlib>
#include <algorithm>

using namespace godot;

GameState::GameState() :
    gold(50),
    current_phase(Phase::SUN),
    price_slider_value(10),
    adventurers_unlocked(false),
    game_phase(1),
    cat_companion(false),
    bat_companion(false),
    murder_of_crows(false),
    awaken_anito(false),
    hire_adventurers(false),
    featured_potion_id(0)
{
    potion_demands[0] = {"LUNAS NG SIGLA", 70, 45, 55};
    potion_demands[1] = {"LUNAS NG LIHIM", 30, 40, 75};
    potion_demands[2] = {"LUNAS NG LINAW", 60, 70, 35};
    potion_demands[3] = {"LUNAS NG HIMBING", 25, 30, 85};
    potion_demands[4] = {"TINCTURA NG BILIS", 80, 65, 30};
    potion_demands[5] = {"LASON", 20, 35, 65};
    potion_demands[6] = {"LUNAS NG DIWA", 50, 55, 60};
}

void GameState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_gold","amount"), &GameState::set_gold);
    ClassDB::bind_method(D_METHOD("get_gold"), &GameState::get_gold);
    ClassDB::bind_method(D_METHOD("add_gold","amount"), &GameState::add_gold);
    ClassDB::bind_method(D_METHOD("advance_phase"), &GameState::advance_phase);
    ClassDB::bind_method(D_METHOD("get_current_phase"), &GameState::get_current_phase);
    ClassDB::bind_method(D_METHOD("get_game_phase"), &GameState::get_game_phase);
    ClassDB::bind_method(D_METHOD("set_price","p"), &GameState::set_price);
    ClassDB::bind_method(D_METHOD("get_price"), &GameState::get_price);
    ClassDB::bind_method(D_METHOD("get_sell_chance_percent"), &GameState::get_sell_chance_percent);
    ClassDB::bind_method(D_METHOD("set_featured_potion_id", "id"), &GameState::set_featured_potion_id);
    ClassDB::bind_method(D_METHOD("get_featured_potion_id"), &GameState::get_featured_potion_id);
    ClassDB::bind_method(D_METHOD("get_featured_potion_name"), &GameState::get_featured_potion_name);
    ClassDB::bind_method(D_METHOD("calculate_sell_chance","demand","price"),
        &GameState::calculate_sell_chance);
    ClassDB::bind_method(D_METHOD("attempt_sale"), &GameState::attempt_sale);
    ClassDB::bind_method(D_METHOD("apply_tech_unlock","unlock_name"),
        &GameState::apply_tech_unlock);
    ClassDB::bind_method(D_METHOD("is_unlocked","unlock_name"),
        &GameState::is_unlocked);
    ClassDB::bind_method(D_METHOD("get_adventurers_unlocked"),
        &GameState::get_adventurers_unlocked);
    ClassDB::bind_method(D_METHOD("unlock_adventurers"),
        &GameState::unlock_adventurers);
    ClassDB::bind_method(D_METHOD("check_phase_progression"),
        &GameState::check_phase_progression);
    ClassDB::bind_method(D_METHOD("check_win_condition"),
        &GameState::check_win_condition);
}

void GameState::set_gold(int amount) { gold = amount; }
int GameState::get_gold() const { return gold; }
void GameState::add_gold(int amount) { gold += amount; }

void GameState::advance_phase() {
    switch (current_phase) {
        case Phase::SUN:   current_phase = Phase::NOON;  break;
        case Phase::NOON:  current_phase = Phase::NIGHT; break;
        case Phase::NIGHT: current_phase = Phase::SUN;   break;
    }
    check_phase_progression();
}

String GameState::get_current_phase() const {
    switch (current_phase) {
        case Phase::SUN:   return "sun";
        case Phase::NOON:  return "noon";
        case Phase::NIGHT: return "night";
    }
    return "sun";
}

int GameState::get_game_phase() const { return game_phase; }
void GameState::set_price(float p) { price_slider_value = p; }
float GameState::get_price() const { return price_slider_value; }

float GameState::get_demand_modifier() const {
    float base = 1.0f;
    if (awaken_anito) base *= 1.3f;
    if (current_phase == Phase::NIGHT && murder_of_crows) base *= 1.4f;
    return base;
}

float GameState::calculate_sell_chance(float demand, float price) {
    float modifier = get_demand_modifier();
    float chance = (demand * modifier) / price;
    return std::clamp(chance, 0.0f, 1.0f);
}

bool GameState::attempt_sale() {
    float demand = get_current_potion_demand();
    float chance = calculate_sell_chance(demand, price_slider_value);
    float roll = static_cast<float>(rand()) / RAND_MAX;
    if (roll < chance) {
        gold += static_cast<int>(price_slider_value);
        return true;
    }
    return false;
}

void GameState::apply_tech_unlock(String unlock_name) {
    if      (unlock_name == "cat_companion")    cat_companion = true;
    else if (unlock_name == "bat_companion")    bat_companion = true;
    else if (unlock_name == "murder_of_crows")  murder_of_crows = true;
    else if (unlock_name == "awaken_anito")     awaken_anito = true;
    else if (unlock_name == "hire_adventurers") hire_adventurers = true;
}

bool GameState::is_unlocked(String unlock_name) const {
    if (unlock_name == "cat_companion")    return cat_companion;
    if (unlock_name == "bat_companion")    return bat_companion;
    if (unlock_name == "murder_of_crows")  return murder_of_crows;
    if (unlock_name == "awaken_anito")     return awaken_anito;
    if (unlock_name == "hire_adventurers") return hire_adventurers;
    return false;
}

bool GameState::get_adventurers_unlocked() const { return adventurers_unlocked; }
void GameState::unlock_adventurers() { adventurers_unlocked = true; }

void GameState::check_phase_progression() {
    if      (game_phase == 1 && gold >= 100)               game_phase = 2;
    else if (game_phase == 2 && gold >= 200)               game_phase = 3;
    else if (game_phase == 3 && adventurers_unlocked)      game_phase = 4;
    else if (game_phase == 4 && is_unlocked("awaken_anito")) game_phase = 5;
}

bool GameState::check_win_condition() const { return gold >= 5000; }

int GameState::get_current_potion_demand() const {
    const PotionDemand& potion = potion_demands[featured_potion_id];

    switch (current_phase) {
        case Phase::SUN: return potion.morning_demand;
        case Phase::NOON: return potion.noon_demand;
        case Phase::NIGHT: return potion.night_demand;
    }

    return potion.morning_demand;
}

int GameState::get_sell_chance_percent() const {
    float demand = get_current_potion_demand();
    float modifier = get_demand_modifier();
    float chance = (demand * modifier) / price_slider_value;

    if (chance < 1) return 1;
    if (chance > 95) return 95;

    return static_cast<int>(chance);
}

void GameState::set_featured_potion_id(int id) {
    if (id >= 0 && id < 7) {
        featured_potion_id = id;
    }
}

int GameState::get_featured_potion_id() const {
    return featured_potion_id;
}

String GameState::get_featured_potion_name() const {
    return potion_demands[featured_potion_id].name;
}