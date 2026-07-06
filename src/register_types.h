#ifndef REGISTER_TYPES_H
#define REGISTER_TYPES_H

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_native_gallery_module(ModuleInitializationLevel p_level);
void uninitialize_native_gallery_module(ModuleInitializationLevel p_level);

#endif // REGISTER_TYPES_H
