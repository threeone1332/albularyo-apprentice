#include "Ingredient.h"
using namespace godot;
 
Ingredient::Ingredient() :
    name(""), is_rare(false), quantity(0),
    restock_amount(3), cost_per_unit(0) {}
 
void Ingredient::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_name","name"), &Ingredient::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &Ingredient::get_name);
    ClassDB::bind_method(D_METHOD("set_is_rare","rare"), &Ingredient::set_is_rare);
    ClassDB::bind_method(D_METHOD("get_is_rare"), &Ingredient::get_is_rare);
    ClassDB::bind_method(D_METHOD("set_quantity","qty"), &Ingredient::set_quantity);
    ClassDB::bind_method(D_METHOD("get_quantity"), &Ingredient::get_quantity);
    ClassDB::bind_method(D_METHOD("set_restock_amount","amt"), &Ingredient::set_restock_amount);
    ClassDB::bind_method(D_METHOD("get_restock_amount"), &Ingredient::get_restock_amount);
    ClassDB::bind_method(D_METHOD("set_cost_per_unit","cost"), &Ingredient::set_cost_per_unit);
    ClassDB::bind_method(D_METHOD("get_cost_per_unit"), &Ingredient::get_cost_per_unit);
    ClassDB::bind_method(D_METHOD("restock"), &Ingredient::restock);
    ClassDB::bind_method(D_METHOD("consume","amount"), &Ingredient::consume);
}
 
void Ingredient::set_name(const String& p_name) { name = p_name; }
String Ingredient::get_name() const { return name; }
void Ingredient::set_is_rare(bool p_rare) { is_rare = p_rare; }
bool Ingredient::get_is_rare() const { return is_rare; }
void Ingredient::set_quantity(int p_qty) { quantity = p_qty; }
int Ingredient::get_quantity() const { return quantity; }
void Ingredient::set_restock_amount(int p) { restock_amount = p; }
int Ingredient::get_restock_amount() const { return restock_amount; }
void Ingredient::set_cost_per_unit(int p) { cost_per_unit = p; }
int Ingredient::get_cost_per_unit() const { return cost_per_unit; }
 
void Ingredient::restock() {
    if (!is_rare) quantity += restock_amount;
}
 
bool Ingredient::consume(int amount) {
    if (quantity < amount) return false;
    quantity -= amount;
    return true;
}
