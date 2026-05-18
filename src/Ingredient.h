#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
 
using namespace godot;
 
class Ingredient : public Resource {
    GDCLASS(Ingredient, Resource)
 
private:
    String name;
    bool is_rare;
    int quantity;
    int restock_amount;
    int cost_per_unit;
 
protected:
    static void _bind_methods();
 
public:
    Ingredient();
    void set_name(const String& p_name);  String get_name() const;
    void set_is_rare(bool p_rare);         bool get_is_rare() const;
    void set_quantity(int p_qty);          int get_quantity() const;
    void set_restock_amount(int p_amt);    int get_restock_amount() const;
    void set_cost_per_unit(int p_cost);    int get_cost_per_unit() const;
    void restock();
    bool consume(int amount);
};
