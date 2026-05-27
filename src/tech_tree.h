#ifndef TECH_TREE_H
#define TECH_TREE_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {

class TechTree : public Control {
    GDCLASS(TechTree, Control);

private:
    TextureButton* exit_button;

protected:
    static void _bind_methods();

public:
    TechTree();
    ~TechTree();

    void _ready() override;
    void _on_exit_pressed();
};

} // namespace godot

#endif // TECH_TREE_H