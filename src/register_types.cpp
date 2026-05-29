#include "register_types.h"
#include "Gamestate.h"
#include "Potion.h"
#include "Ingredient.h"
#include "loading_screen.h"
#include "mixing_screen.h"
#include "game_loading_screen.h"
#include "main_menu.h"
#include "tech_tree.h"
#include "intro_cutscene.h"
#include "main_screen.h"
#include "settings.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_albularyo_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;

    ClassDB::register_class<GameState>();
    ClassDB::register_class<Potion>();
    ClassDB::register_class<Ingredient>();
    ClassDB::register_class<LoadingScreen>();
    ClassDB::register_class<MixingScreen>();
    ClassDB::register_class<GameLoadingScreen>();
    ClassDB::register_class<MainMenu>();
    ClassDB::register_class<TechTree>();
    ClassDB::register_class<IntroCutscene>();
    ClassDB::register_class<MainScreen>();
    ClassDB::register_class<Settings>();
}

void uninitialize_albularyo_module(ModuleInitializationLevel p_level) {}

extern "C" {
GDExtensionBool GDE_EXPORT albularyo_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(
        p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_albularyo_module);
    init_obj.register_terminator(uninitialize_albularyo_module);
    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
}