#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/dictionary.hpp>
 
using namespace godot;
 
class Potion : public Resource {
    GDCLASS(Potion, Resource)
 
private:
    String name;
    String base_ingredient;
    String rare_ingredient;
    int base_value;
    Dictionary demand_modifiers;
 
protected:
    static void _bind_methods();
 
public:
    Potion();
    void set_name(const String& p_name);           String get_name() const;
    void set_base_ingredient(const String& s);     String get_base_ingredient() const;
    void set_rare_ingredient(const String& s);     String get_rare_ingredient() const;
    void set_base_value(int v);                    int get_base_value() const;
    void set_demand_modifiers(const Dictionary& d);
    Dictionary get_demand_modifiers() const;
    float get_demand_for_phase(const String& phase) const;
    int calculate_value(bool has_rare) const;
};
