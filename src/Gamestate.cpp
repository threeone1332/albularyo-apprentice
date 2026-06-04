#include "Gamestate.h"
#include <cstdlib>
#include <algorithm>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

GameState::GameState() :
    gold(100),
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
    for (int i = 0; i < 8; i++) {
        player_inventory[i] = 5;
    }

    for (int i = 0; i < 7; i++) {
        player_potions[i] = 0;
    }

    potion_demands[0] = {"LUNAS NG SIGLA", 90, 35, 20};
    potion_demands[1] = {"LUNAS NG LIHIM", 20, 45, 90};
    potion_demands[2] = {"LUNAS NG LINAW", 65, 90, 25};
    potion_demands[3] = {"LUNAS NG HIMBING", 10, 30, 95};
    potion_demands[4] = {"TINCTURA NG BILIS", 95, 70, 20};
    potion_demands[5] = {"LASON", 15, 35, 85};
    potion_demands[6] = {"LUNAS NG DIWA", 45, 55, 80};
}

// --- NEW STATE RESET LOGIC FOR NEW GAME LOOP ---
void GameState::reset_state() {
    gold = 100;
    current_phase = Phase::SUN;
    price_slider_value = 10;
    adventurers_unlocked = false;
    game_phase = 1;
    cat_companion = false;
    bat_companion = false;
    murder_of_crows = false;
    awaken_anito = false;
    hire_adventurers = false;
    featured_potion_id = 0;

    for (int i = 0; i < 8; i++) {
        player_inventory[i] = 5; // Resets starting ingredients back to default values
    }

    for (int i = 0; i < 7; i++) {
        player_potions[i] = 0; // Wipes out all brewed potion storage queues
    }
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
    ClassDB::bind_method(D_METHOD("calculate_sell_chance","demand","price"), &GameState::calculate_sell_chance);
    ClassDB::bind_method(D_METHOD("attempt_sale"), &GameState::attempt_sale);
    ClassDB::bind_method(D_METHOD("apply_tech_unlock","unlock_name"), &GameState::apply_tech_unlock);
    ClassDB::bind_method(D_METHOD("is_unlocked","unlock_name"), &GameState::is_unlocked);
    ClassDB::bind_method(D_METHOD("get_adventurers_unlocked"), &GameState::get_adventurers_unlocked);
    ClassDB::bind_method(D_METHOD("unlock_adventurers"), &GameState::unlock_adventurers);
    ClassDB::bind_method(D_METHOD("check_phase_progression"), &GameState::check_phase_progression);
    ClassDB::bind_method(D_METHOD("check_win_condition"), &GameState::check_win_condition);
    ClassDB::bind_method(D_METHOD("get_ingredient_count", "id"), &GameState::get_ingredient_count);
    ClassDB::bind_method(D_METHOD("set_ingredient_count", "id", "amount"), &GameState::set_ingredient_count);
    ClassDB::bind_method(D_METHOD("add_ingredient", "id", "amount"), &GameState::add_ingredient);
    ClassDB::bind_method(D_METHOD("get_potion_count", "id"), &GameState::get_potion_count);
    ClassDB::bind_method(D_METHOD("set_potion_count", "id", "amount"), &GameState::set_potion_count);
    ClassDB::bind_method(D_METHOD("add_potion", "id", "amount"), &GameState::add_potion);
    ClassDB::bind_method(D_METHOD("start_new_day"), &GameState::start_new_day);
    ClassDB::bind_method(D_METHOD("gather_phase_resources"), &GameState::gather_phase_resources);
    ClassDB::bind_method(D_METHOD("has_featured_stock"), &GameState::has_featured_stock);

    // Binding the new Save/Load methods so your MainMenu script can call them!
    ClassDB::bind_method(D_METHOD("get_save_data"), &GameState::get_save_data);
    ClassDB::bind_method(D_METHOD("load_save_data", "data"), &GameState::load_save_data);

    // FIX: Exposed reset_state macro binding so Engine scripting contexts can execute it
    ClassDB::bind_method(D_METHOD("reset_state"), &GameState::reset_state);
}

// --- SERIALIZATION LOGIC FOR JSON SAVES ---

Dictionary GameState::get_save_data() {
    Dictionary data;

    // Core metrics
    data["gold"] = gold;
    data["game_phase"] = game_phase;
    data["price_slider_value"] = price_slider_value;
    data["featured_potion_id"] = featured_potion_id;
    data["current_phase"] = static_cast<int>(current_phase);

    // Tech Upgrades
    data["adventurers_unlocked"] = adventurers_unlocked;
    data["cat_companion"] = cat_companion;
    data["bat_companion"] = bat_companion;
    data["murder_of_crows"] = murder_of_crows;
    data["awaken_anito"] = awaken_anito;
    data["hire_adventurers"] = hire_adventurers;

    // Arrays packaging (Inventory & Potions Stock)
    Array inv_arr;
    for (int i = 0; i < 8; i++) {
        inv_arr.append(player_inventory[i]);
    }
    data["player_inventory"] = inv_arr;

    Array pot_arr;
    for (int i = 0; i < 7; i++) {
        pot_arr.append(player_potions[i]);
    }
    data["player_potions"] = pot_arr;

    return data;
}

static bool save_value_is_number(const Variant &value) {
    return value.get_type() == Variant::INT || value.get_type() == Variant::FLOAT;
}

static int save_read_int(const Dictionary &data, const String &key, int fallback) {
    if (!data.has(key)) {
        return fallback;
    }

    Variant value = data[key];
    if (!save_value_is_number(value)) {
        UtilityFunctions::printerr("GameState Save Load: Key has non-numeric value: ", key);
        return fallback;
    }

    return static_cast<int>(value);
}

static float save_read_float(const Dictionary &data, const String &key, float fallback) {
    if (!data.has(key)) {
        return fallback;
    }

    Variant value = data[key];
    if (!save_value_is_number(value)) {
        UtilityFunctions::printerr("GameState Save Load: Key has non-numeric value: ", key);
        return fallback;
    }

    return static_cast<float>(value);
}

static bool save_read_bool(const Dictionary &data, const String &key, bool fallback) {
    if (!data.has(key)) {
        return fallback;
    }

    Variant value = data[key];
    if (value.get_type() != Variant::BOOL) {
        UtilityFunctions::printerr("GameState Save Load: Key has non-bool value: ", key);
        return fallback;
    }

    return static_cast<bool>(value);
}

void GameState::load_save_data(Dictionary data) {
    UtilityFunctions::print("GameState Save Load: Applying dictionary...");

    gold = std::max(0, save_read_int(data, "gold", gold));
    game_phase = std::max(1, save_read_int(data, "game_phase", game_phase));
    price_slider_value = std::max(1.0f, save_read_float(data, "price_slider_value", price_slider_value));
    featured_potion_id = std::clamp(save_read_int(data, "featured_potion_id", featured_potion_id), 0, 6);

    if (data.has("current_phase")) {
        int phase_id = std::clamp(save_read_int(data, "current_phase", static_cast<int>(current_phase)), 0, 2);
        current_phase = static_cast<Phase>(phase_id);
    }

    // Load Tech Upgrades State
    adventurers_unlocked = save_read_bool(data, "adventurers_unlocked", adventurers_unlocked);
    cat_companion = save_read_bool(data, "cat_companion", cat_companion);
    bat_companion = save_read_bool(data, "bat_companion", bat_companion);
    murder_of_crows = save_read_bool(data, "murder_of_crows", murder_of_crows);
    awaken_anito = save_read_bool(data, "awaken_anito", awaken_anito);
    hire_adventurers = save_read_bool(data, "hire_adventurers", hire_adventurers);

    // Unpack Player Raw Ingredients Arrays safely
    if (data.has("player_inventory")) {
        Variant inv_value = data["player_inventory"];
        if (inv_value.get_type() == Variant::ARRAY) {
            Array inv_arr = inv_value;
            for (int i = 0; i < 8 && i < inv_arr.size(); i++) {
                Variant item = inv_arr[i];
                if (save_value_is_number(item)) {
                    player_inventory[i] = std::max(0, static_cast<int>(item));
                }
            }
        } else {
            UtilityFunctions::printerr("GameState Save Load: player_inventory is not an Array.");
        }
    }

    // Unpack Processed Potion Stocks Arrays safely
    if (data.has("player_potions")) {
        Variant pot_value = data["player_potions"];
        if (pot_value.get_type() == Variant::ARRAY) {
            Array pot_arr = pot_value;
            for (int i = 0; i < 7 && i < pot_arr.size(); i++) {
                Variant item = pot_arr[i];
                if (save_value_is_number(item)) {
                    player_potions[i] = std::max(0, static_cast<int>(item));
                }
            }
        } else {
            UtilityFunctions::printerr("GameState Save Load: player_potions is not an Array.");
        }
    }

    UtilityFunctions::print("GameState Save Load: Complete.");
}

// --- CORE GAME SYSTEM MECHANICS TRACKERS ---

void GameState::set_gold(int amount) { gold = amount; }
int GameState::get_gold() const { return gold; }
void GameState::add_gold(int amount) { gold += amount; }

void GameState::advance_phase() {
    switch (current_phase) {
        case Phase::SUN:
            current_phase = Phase::NOON;
            break;
        case Phase::NOON:
            current_phase = Phase::NIGHT;
            break;
        case Phase::NIGHT:
            current_phase = Phase::SUN;
            break;
            start_new_day();
            break;
    }
    gather_phase_resources();
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

void GameState::set_price(float p) {
    price_slider_value = std::max(1.0f, p);
}

float GameState::get_price() const { return price_slider_value; }

float GameState::get_demand_modifier() const {
    float base = 1.0f;
    if (awaken_anito) base *= 1.3f;
    if (current_phase == Phase::NIGHT && murder_of_crows) base *= 1.4f;
    return base;
}

float GameState::calculate_sell_chance(float demand, float price) const {
    float modifier = get_demand_modifier();
    float base_chance = (demand * modifier) / 100.0f;
    float price_penalty = 1.0f / (1.0f + (price * 0.08f));
    float chance = base_chance * price_penalty;
    return std::clamp(chance, 0.01f, 0.95f);
}

bool GameState::attempt_sale() {
    if (player_potions[featured_potion_id] <= 0) {
        return false;
    }
    float demand = get_current_potion_demand();
    float chance = calculate_sell_chance(demand, price_slider_value);
    float roll = static_cast<float>(rand()) / RAND_MAX;
    if (roll < chance) {
        player_potions[featured_potion_id]--;
        gold += static_cast<int>(price_slider_value);
        return true;
    }
    return false;
}

void GameState::apply_tech_unlock(String unlock_name) {
    if (unlock_name == "cat_companion") {
        cat_companion = true;
    } else if (unlock_name == "bat_companion") {
        bat_companion = true;
    } else if (unlock_name == "murder_of_crows") {
        murder_of_crows = true;
    } else if (unlock_name == "awaken_anito") {
        awaken_anito = true;
    } else if (unlock_name == "hire_adventurers") {
        hire_adventurers = true;
        adventurers_unlocked = true;
    }
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
    if      (game_phase == 1 && gold >= 100)        game_phase = 2;
    else if (game_phase == 2 && gold >= 200)        game_phase = 3;
    else if (game_phase == 3 && adventurers_unlocked)       game_phase = 4;
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
    float chance = calculate_sell_chance(demand, price_slider_value);
    return static_cast<int>(chance * 100.0f);
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

int GameState::get_ingredient_count(int id) const {
    if (id < 0 || id >= 8) {
        UtilityFunctions::printerr("GameState Error: Out of bounds ingredient ID requested: ", id);
        return 0;
    }
    return player_inventory[id];
}

void GameState::set_ingredient_count(int id, int amount) {
    if (id >= 0 && id < 8) {
        player_inventory[id] = std::max(0, amount);
    }
}

void GameState::add_ingredient(int id, int amount) {
    if (id < 0 || id >= 8) return;
    player_inventory[id] = std::max(0, player_inventory[id] + amount);
}

int GameState::get_potion_count(int id) const {
    if (id < 0 || id >= 7) {
        UtilityFunctions::printerr("GameState Error: Out of bounds potion ID requested: ", id);
        return 0;
    }
    return player_potions[id];
}

void GameState::set_potion_count(int id, int amount) {
    if (id >= 0 && id < 7) {
        player_potions[id] = std::max(0, amount);
    }
}

void GameState::add_potion(int id, int amount) {
    if (id < 0 || id >= 7) return;
    player_potions[id] = std::max(0, player_potions[id] + amount);
}

void GameState::start_new_day() {
    add_ingredient(0, bat_companion ? 3 : 2); // BULAKLAK NG SIGLA
    add_ingredient(2, bat_companion ? 3 : 2); // BULAKLAK NG HIMBING
    add_ingredient(4, 2);                     // KABUTE
    add_ingredient(6, cat_companion ? 3 : 1); // PARUPARO

    if (adventurers_unlocked) {
        int delivery = hire_adventurers ? 2 : 1;

        add_ingredient(1, delivery); // HIBLA NG DIWA
        add_ingredient(3, delivery); // BUTIL NG ARAW
        add_ingredient(5, delivery); // PUSO NG DILIM
        add_ingredient(7, delivery); // BIYAK NG TALA

        gold = std::max(0, gold - (hire_adventurers ? 40 : 25));
    }
}

void GameState::gather_phase_resources() {
    switch (current_phase) {
        case Phase::SUN:
            add_ingredient(0, 1 + rand() % 2); // BULAKLAK NG SIGLA
            add_ingredient(2, rand() % 2);     // BULAKLAK NG HIMBING
            break;

        case Phase::NOON:
            add_ingredient(4, 1 + rand() % 2); // KABUTE
            add_ingredient(6, 1 + rand() % 2); // PARUPARO

            if (cat_companion) {
                add_ingredient(6, 1); // extra PARUPARO
            }
            break;

        case Phase::NIGHT:
            add_ingredient(2, 1); // BULAKLAK NG HIMBING

            if (bat_companion) {
                add_ingredient(0, 1); // extra BULAKLAK NG SIGLA
                add_ingredient(2, 1); // extra BULAKLAK NG HIMBING
            }

            if (adventurers_unlocked) {
                if ((rand() % 100) < 20) add_ingredient(1, 1); // HIBLA NG DIWA
                if ((rand() % 100) < 15) add_ingredient(3, 1); // BUTIL NG ARAW
                if ((rand() % 100) < 15) add_ingredient(5, 1); // PUSO NG DILIM
                if ((rand() % 100) < 10) add_ingredient(7, 1); // BIYAK NG TALA
            }
            break;
    }
}

bool GameState::has_featured_stock() const {
    return player_potions[featured_potion_id] > 0;
}