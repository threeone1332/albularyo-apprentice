#ifndef TECH_TREE_H
#define TECH_TREE_H

#include "GameState.h"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class TechTree : public Control {
    GDCLASS(TechTree, Control);

private:
    // Buttons and labels from tech_tree.tscn.
    TextureButton* exit_button = nullptr;
    Label* money_label = nullptr;
    Label* message_label = nullptr;

    // Uses your existing autoload: /root/GlobalGameState.
    GameState* game_state = nullptr;

    int get_upgrade_cost(String upgrade_id) const;
    String get_upgrade_name(String upgrade_id) const;

    void connect_upgrade_button(String node_name, String upgrade_id);
    void show_message(String message);
    void update_money_label();

protected:
    static void _bind_methods();

public:
    TechTree();
    ~TechTree();

    void _ready() override;

    void _on_exit_pressed();

    void buy_upgrade(String upgrade_id);
    void show_upgrade_hover(String upgrade_id);
    void clear_upgrade_hover();
};

}

#endif