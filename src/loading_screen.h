#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/classes/label.hpp>

namespace godot {

class LoadingScreen : public Control {
    GDCLASS(LoadingScreen, Control);

protected:
    static void _bind_methods();

public:
    LoadingScreen();
    ~LoadingScreen();

    void _ready() override;
    void _process(double delta) override;

private:
    ProgressBar* loading_bar;
    Label* percent_label;
    double progress_speed = 25.0; // The fill rate speed
    bool is_switching = false;
};

}

#endif