#include "native_gallery.h"

#if !defined(__ANDROID__) && !(defined(__APPLE__) && defined(TARGET_OS_IOS))

#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void NativeGallery::_setup_file_dialog() {
    if (file_dialog) {
        return;
    }

    file_dialog = memnew(FileDialog);
    file_dialog->set_access(FileDialog::ACCESS_FILESYSTEM);
    file_dialog->set_title("Select Media");

    // Connect signals
    file_dialog->connect("file_selected", callable_mp(this, &NativeGallery::_on_file_dialog_file_selected));
    file_dialog->connect("files_selected", callable_mp(this, &NativeGallery::_on_file_dialog_files_selected));
    file_dialog->connect("canceled", callable_mp(this, &NativeGallery::_on_file_dialog_cancelled));

    add_child(file_dialog);
}

void NativeGallery::_pick_native_image(bool p_multiple) {
    _setup_file_dialog();

    file_dialog->set_file_mode(p_multiple ? FileDialog::FILE_MODE_OPEN_FILES : FileDialog::FILE_MODE_OPEN_FILE);
    file_dialog->clear_filters();
    file_dialog->add_filter("*.png,*.jpg,*.jpeg,*.webp,*.bmp,*.gif", "Images");

    file_dialog->popup_centered_ratio();
}

void NativeGallery::_pick_native_video(bool p_multiple) {
    _setup_file_dialog();

    file_dialog->set_file_mode(p_multiple ? FileDialog::FILE_MODE_OPEN_FILES : FileDialog::FILE_MODE_OPEN_FILE);
    file_dialog->clear_filters();
    file_dialog->add_filter("*.mp4,*.webm,*.mov,*.mkv,*.avi,*.ogv", "Videos");

    file_dialog->popup_centered_ratio();
}

void NativeGallery::_scan_media(const String &p_path) {
    // No-op on desktop — files are immediately visible in the filesystem
    UtilityFunctions::print("NativeGallery: scan_media is a no-op on desktop (path: ", p_path, ")");
}

void NativeGallery::_on_file_dialog_file_selected(const String &p_path) {
    if (current_pick_type == PICK_TYPE_IMAGE) {
        emit_signal("image_selected", p_path);
    } else if (current_pick_type == PICK_TYPE_VIDEO) {
        emit_signal("video_selected", p_path);
    }
}

void NativeGallery::_on_file_dialog_files_selected(const PackedStringArray &p_paths) {
    if (current_pick_type == PICK_TYPE_IMAGE) {
        emit_signal("images_selected", p_paths);
    } else if (current_pick_type == PICK_TYPE_VIDEO) {
        emit_signal("videos_selected", p_paths);
    }
}

void NativeGallery::_on_file_dialog_cancelled() {
    emit_signal("cancelled");
}

void NativeGallery::_poll_android() {
    // No-op on desktop
}

#endif
