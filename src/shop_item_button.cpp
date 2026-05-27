#include "shop_item_button.h"

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void ShopItemButton::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_item_name", "name"), &ShopItemButton::set_item_name);
    ClassDB::bind_method(D_METHOD("get_item_name"), &ShopItemButton::get_item_name);

    ClassDB::bind_method(D_METHOD("set_price", "value"), &ShopItemButton::set_price);
    ClassDB::bind_method(D_METHOD("get_price"), &ShopItemButton::get_price);

    ClassDB::bind_method(D_METHOD("on_mouse_entered"), &ShopItemButton::on_mouse_entered);
    ClassDB::bind_method(D_METHOD("on_mouse_exited"), &ShopItemButton::on_mouse_exited);
    ClassDB::bind_method(D_METHOD("on_pressed"), &ShopItemButton::on_pressed);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "item_name"), "set_item_name", "get_item_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "price"), "set_price", "get_price");
}

void ShopItemButton::_ready() {
    connect("mouse_entered", Callable(this, "on_mouse_entered"));
    connect("mouse_exited", Callable(this, "on_mouse_exited"));
    connect("pressed", Callable(this, "on_pressed"));
}

void ShopItemButton::set_item_name(String name) {
    item_name = name;
}

String ShopItemButton::get_item_name() const {
    return item_name;
}

void ShopItemButton::set_price(int value) {
    price = value;
}

int ShopItemButton::get_price() const {
    return price;
}

void ShopItemButton::on_mouse_entered() {
    Node *shop = get_parent();

    int money = shop->get("money");

    Label *message_label = Object::cast_to<Label>(shop->get_node("MessageLabel"));

    if (money >= price) {
        message_label->set_text("Click to buy " + item_name);
    } else {
        message_label->set_text("Not enough coins for " + item_name);
    }
}

void ShopItemButton::on_mouse_exited() {
    Node *shop = get_parent();

    Label *message_label = Object::cast_to<Label>(shop->get_node("MessageLabel"));

    message_label->set_text("");
}

void ShopItemButton::on_pressed() {
    Node *shop = get_parent();

    int money = shop->get("money");

    Label *message_label = Object::cast_to<Label>(shop->get_node("MessageLabel"));

    if (money >= price) {
        money -= price;

        shop->set("money", money);

        shop->call("update_money_label");

        message_label->set_text("Bought " + item_name);
    } else {
        message_label->set_text("You cannot buy this");
    }
}