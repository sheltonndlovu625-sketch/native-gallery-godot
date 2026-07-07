#include "native_gallery.h"

#ifdef __ANDROID__

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void NativeGallery::_pick_native_image(bool p_multiple) {
    Object *plugin = Engine::get_singleton()->get_singleton("NativeGalleryAndroid");
    if (!plugin) {
        emit_signal("error", 1, "NativeGalleryAndroid plugin not found. Ensure the Android plugin is built and enabled in your export preset.");
        return;
    }
    plugin->call("pickImage", p_multiple);
    set_process(true);
}

void NativeGallery::_pick_native_video(bool p_multiple) {
    Object *plugin = Engine::get_singleton()->get_singleton("NativeGalleryAndroid");
    if (!plugin) {
        emit_signal("error", 1, "NativeGalleryAndroid plugin not found.");
        return;
    }
    plugin->call("pickVideo", p_multiple);
    set_process(true);
}

void NativeGallery::_scan_media(const String &p_path) {
    Object *plugin = Engine::get_singleton()->get_singleton("NativeGalleryAndroid");
    if (plugin) {
        plugin->call("scanMedia", p_path);
    } else {
        UtilityFunctions::print("NativeGallery: scan_media skipped — plugin not found");
    }
}

void NativeGallery::_poll_android() {
    Object *plugin = Engine::get_singleton()->get_singleton("NativeGalleryAndroid");
    if (!plugin) return;
    
    bool has_result = plugin->call("hasResult");
    if (!has_result) return;
    
    set_process(false);
    
    bool cancelled = plugin->call("wasCancelled");
    if (cancelled) {
        emit_signal("cancelled");
        return;
    }
    
    if (!allow_multiple) {
        String path = plugin->call("getLastResult");
        if (current_pick_type == PICK_TYPE_IMAGE) {
            emit_signal("image_selected", path);
        } else {
            emit_signal("video_selected", path);
        }
    } else {
        PackedStringArray paths = plugin->call("getLastResults");
        if (current_pick_type == PICK_TYPE_IMAGE) {
            emit_signal("images_selected", paths);
        } else {
            emit_signal("videos_selected", paths);
        }
    }
}

#endif
