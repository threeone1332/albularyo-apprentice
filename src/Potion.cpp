#include "Potion.h"
using namespace godot;
 
Potion::Potion() :
    name(""), base_ingredient(""), rare_ingredient(""), base_value(10) {}
 
void Potion::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_name","n"), &Potion::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &Potion::get_name);
    ClassDB::bind_method(D_METHOD("set_base_ingredient","s"), &Potion::set_base_ingredient);
    ClassDB::bind_method(D_METHOD("get_base_ingredient"), &Potion::get_base_ingredient);
    ClassDB::bind_method(D_METHOD("set_rare_ingredient","s"), &Potion::set_rare_ingredient);
    ClassDB::bind_method(D_METHOD("get_rare_ingredient"), &Potion::get_rare_ingredient);
    ClassDB::bind_method(D_METHOD("set_base_value","v"), &Potion::set_base_value);
    ClassDB::bind_method(D_METHOD("get_base_value"), &Potion::get_base_value);
    ClassDB::bind_method(D_METHOD("set_demand_modifiers","d"), &Potion::set_demand_modifiers);
    ClassDB::bind_method(D_METHOD("get_demand_modifiers"), &Potion::get_demand_modifiers);
    ClassDB::bind_method(D_METHOD("get_demand_for_phase","phase"),
        &Potion::get_demand_for_phase);
    ClassDB::bind_method(D_METHOD("calculate_value","has_rare"),
        &Potion::calculate_value);
}
 
void Potion::set_name(const String& p) { name = p; }
String Potion::get_name() const { return name; }
void Potion::set_base_ingredient(const String& s) { base_ingredient = s; }
String Potion::get_base_ingredient() const { return base_ingredient; }
void Potion::set_rare_ingredient(const String& s) { rare_ingredient = s; }
String Potion::get_rare_ingredient() const { return rare_ingredient; }
void Potion::set_base_value(int v) { base_value = v; }
int Potion::get_base_value() const { return base_value; }
void Potion::set_demand_modifiers(const Dictionary& d) { demand_modifiers = d; }
Dictionary Potion::get_demand_modifiers() const { return demand_modifiers; }
 
float Potion::get_demand_for_phase(const String& phase) const {
    if (demand_modifiers.has(phase))
        return static_cast<float>(demand_modifiers[phase]);
    return 1.0f;
}
 
int Potion::calculate_value(bool has_rare) const {
    return has_rare ? static_cast<int>(base_value * 1.5f) : base_value;
}