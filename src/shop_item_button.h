#ifndef SHOP_ITEM_BUTTON_H
#define SHOP_ITEM_BUTTON_H

#include <godot_cpp/classes/texture_button.hpp>

using namespace godot;

class ShopItemButton : public TextureButton {
    GDCLASS(ShopItemButton, TextureButton)

private:
    String item_name = "Item";
    int price = 100;

protected:
    static void _bind_methods();

public:
    void _ready();

    void set_item_name(String name);
    String get_item_name() const;

    void set_price(int value);
    int get_price() const;

    void on_mouse_entered();
    void on_mouse_exited();
    void on_pressed();
};

#endif