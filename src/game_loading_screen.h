#ifndef GAME_LOADING_SCREEN_H
#define GAME_LOADING_SCREEN_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot {

class GameLoadingScreen : public Control {
    GDCLASS(GameLoadingScreen, Control);

private:
    ProgressBar* progress_bar;
    Label* status_label;
    Label* dynamic_flavor_label;

    TypedArray<String> flavor_texts;
    int current_flavor_index;
    float flavor_timer;
    float flavor_interval;

    double progress_speed;
    bool is_switching;

    void change_flavor_text();

protected:
    static void _bind_methods();

public:
    GameLoadingScreen();
    ~GameLoadingScreen();

    void _ready() override;
    void _process(double delta) override;
};

}

#endif // GAME_LOADING_SCREEN_H