#ifndef NATIVE_GALLERY_H
#define NATIVE_GALLERY_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class NativeGallery : public Node {
    GDCLASS(NativeGallery, Node)

public:
    enum PickType {
        PICK_TYPE_IMAGE = 0,
        PICK_TYPE_VIDEO = 1,
        PICK_TYPE_ALL = 2
    };

protected:
    static void _bind_methods();
    void _notification(int p_what);

private:
    // Desktop fallback
    FileDialog *file_dialog = nullptr;
    void _setup_file_dialog();
    void _on_file_dialog_file_selected(const String &p_path);
    void _on_file_dialog_files_selected(const PackedStringArray &p_paths);
    void _on_file_dialog_cancelled();

    // Platform-specific implementations (defined in platform files)
    void _pick_native_image(bool p_multiple);
    void _pick_native_video(bool p_multiple);
    void _scan_media(const String &p_path);
    void _poll_android();

    // State
    PickType default_type = PICK_TYPE_IMAGE;
    bool allow_multiple = false;
    PickType current_pick_type = PICK_TYPE_IMAGE;

public:
    void set_default_type(PickType p_type);
    PickType get_default_type() const;

    void set_allow_multiple(bool p_allow);
    bool get_allow_multiple() const;

    // Main API
    void popup_image_picker();
    void popup_video_picker();
    void popup_image_picker_multiple();
    void popup_video_picker_multiple();

    void scan_media(const String &p_path);

    // Internal callbacks (called from platform layers)
    void _on_image_selected(const String &p_path);
    void _on_images_selected(const PackedStringArray &p_paths);
    void _on_video_selected(const String &p_path);
    void _on_videos_selected(const PackedStringArray &p_paths);
    void _on_cancelled();
    void _on_error(int p_code, const String &p_message);

    NativeGallery();
    ~NativeGallery();
};

}

VARIANT_ENUM_CAST(NativeGallery::PickType);

#endif // NATIVE_GALLERY_H
