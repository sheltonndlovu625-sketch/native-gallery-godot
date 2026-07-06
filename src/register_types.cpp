#include "register_types.h"
#include "native_gallery.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_native_gallery_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ClassDB::register_class<NativeGallery>();
    }
}

void uninitialize_native_gallery_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        // Cleanup if needed
    }
}

extern "C" {
    GDExtensionBool GDE_EXPORT native_gallery_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        const GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization
    ) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        init_obj.register_initializer(initialize_native_gallery_module);
        init_obj.register_terminator(uninitialize_native_gallery_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
        return init_obj.init();
    }
}
