#include "native_gallery.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void NativeGallery::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_default_type", "type"), &NativeGallery::set_default_type);
    ClassDB::bind_method(D_METHOD("get_default_type"), &NativeGallery::get_default_type);
    ClassDB::bind_method(D_METHOD("set_allow_multiple", "allow"), &NativeGallery::set_allow_multiple);
    ClassDB::bind_method(D_METHOD("get_allow_multiple"), &NativeGallery::get_allow_multiple);

    ClassDB::bind_method(D_METHOD("popup_image_picker"), &NativeGallery::popup_image_picker);
    ClassDB::bind_method(D_METHOD("popup_video_picker"), &NativeGallery::popup_video_picker);
    ClassDB::bind_method(D_METHOD("popup_image_picker_multiple"), &NativeGallery::popup_image_picker_multiple);
    ClassDB::bind_method(D_METHOD("popup_video_picker_multiple"), &NativeGallery::popup_video_picker_multiple);
    ClassDB::bind_method(D_METHOD("scan_media", "path"), &NativeGallery::scan_media);

    // Internal callbacks (exposed so platform code can invoke them)
    ClassDB::bind_method(D_METHOD("_on_image_selected", "path"), &NativeGallery::_on_image_selected);
    ClassDB::bind_method(D_METHOD("_on_images_selected", "paths"), &NativeGallery::_on_images_selected);
    ClassDB::bind_method(D_METHOD("_on_video_selected", "path"), &NativeGallery::_on_video_selected);
    ClassDB::bind_method(D_METHOD("_on_videos_selected", "paths"), &NativeGallery::_on_videos_selected);
    ClassDB::bind_method(D_METHOD("_on_cancelled"), &NativeGallery::_on_cancelled);
    ClassDB::bind_method(D_METHOD("_on_error", "code", "message"), &NativeGallery::_on_error);

    ADD_SIGNAL(MethodInfo("image_selected", PropertyInfo(Variant::STRING, "path")));
    ADD_SIGNAL(MethodInfo("images_selected", PropertyInfo(Variant::PACKED_STRING_ARRAY, "paths")));
    ADD_SIGNAL(MethodInfo("video_selected", PropertyInfo(Variant::STRING, "path")));
    ADD_SIGNAL(MethodInfo("videos_selected", PropertyInfo(Variant::PACKED_STRING_ARRAY, "paths")));
    ADD_SIGNAL(MethodInfo("cancelled"));
    ADD_SIGNAL(MethodInfo("error", PropertyInfo(Variant::INT, "code"), PropertyInfo(Variant::STRING, "message")));

    ADD_PROPERTY(PropertyInfo(Variant::INT, "default_type", PROPERTY_HINT_ENUM, "Image,Video,All"), "set_default_type", "get_default_type");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_multiple"), "set_allow_multiple", "get_allow_multiple");

    BIND_ENUM_CONSTANT(PICK_TYPE_IMAGE);
    BIND_ENUM_CONSTANT(PICK_TYPE_VIDEO);
    BIND_ENUM_CONSTANT(PICK_TYPE_ALL);
}

void NativeGallery::_notification(int p_what) {
    if (p_what == Node::NOTIFICATION_PROCESS) {
        #ifdef __ANDROID__
        _poll_android();
        #endif
    }
}

void NativeGallery::set_default_type(PickType p_type) {
    default_type = p_type;
}

NativeGallery::PickType NativeGallery::get_default_type() const {
    return default_type;
}

void NativeGallery::set_allow_multiple(bool p_allow) {
    allow_multiple = p_allow;
}

bool NativeGallery::get_allow_multiple() const {
    return allow_multiple;
}

void NativeGallery::popup_image_picker() {
    current_pick_type = PICK_TYPE_IMAGE;
    allow_multiple = false;
    _pick_native_image(false);
}

void NativeGallery::popup_video_picker() {
    current_pick_type = PICK_TYPE_VIDEO;
    allow_multiple = false;
    _pick_native_video(false);
}

void NativeGallery::popup_image_picker_multiple() {
    current_pick_type = PICK_TYPE_IMAGE;
    allow_multiple = true;
    _pick_native_image(true);
}

void NativeGallery::popup_video_picker_multiple() {
    current_pick_type = PICK_TYPE_VIDEO;
    allow_multiple = true;
    _pick_native_video(true);
}

void NativeGallery::scan_media(const String &p_path) {
    _scan_media(p_path);
}

// --- Internal callbacks ---

void NativeGallery::_on_image_selected(const String &p_path) {
    emit_signal("image_selected", p_path);
}

void NativeGallery::_on_images_selected(const PackedStringArray &p_paths) {
    emit_signal("images_selected", p_paths);
}

void NativeGallery::_on_video_selected(const String &p_path) {
    emit_signal("video_selected", p_path);
}

void NativeGallery::_on_videos_selected(const PackedStringArray &p_paths) {
    emit_signal("videos_selected", p_paths);
}

void NativeGallery::_on_cancelled() {
    emit_signal("cancelled");
}

void NativeGallery::_on_error(int p_code, const String &p_message) {
    emit_signal("error", p_code, p_message);
}

NativeGallery::NativeGallery() {
}

NativeGallery::~NativeGallery() {
    if (file_dialog) {
        memdelete(file_dialog);
        file_dialog = nullptr;
    }
}
