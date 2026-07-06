# NativeGallery GDExtension for Godot 4

A cross-platform GDExtension that brings native image/video gallery picking and media scanning to Godot 4.

## Features

- **Native Gallery Picker** on Android (via Intent) and iOS (via PHPickerViewController)
- **Desktop Fallback** using Godot's built-in `FileDialog` (Linux, Windows, macOS)
- **Media Scan** on Android (`MediaScannerConnection`) so saved files appear in the gallery
- **Single & Multiple** selection support
- **Image & Video** picking with unified signals

## Architecture

```
NativeGallery (Node)
├── Desktop: FileDialog fallback
├── Android: Godot Android Plugin (Engine singleton polling)
└── iOS: PHPickerViewController (iOS 14+)
```

> **Note:** This extends `Node` rather than `Window`/`Popup` because the actual UI is rendered by the operating system, not by Godot. The API is designed to feel similar to `FileDialog`.

## Building

### Prerequisites

- Python 3.7+
- SCons (`pip install scons`)
- C++ compiler (GCC, Clang, or MSVC)
- Godot 4.x

### Setup

```bash
git clone https://github.com/yourusername/native_gallery_gdextension.git
cd native_gallery_gdextension
git submodule add -b 4.2 https://github.com/godotengine/godot-cpp.git
git submodule update --init --recursive
```

### Build Commands

```bash
# Linux
scons platform=linux target=template_release -j$(nproc)

# Windows
scons platform=windows target=template_release -j$(nproc)

# macOS
scons platform=macos target=template_release -j$(sysctl -n hw.ncpu)

# Android (requires NDK)
scons platform=android target=template_release android_arch=arm64 -j$(nproc)

# iOS (requires Xcode, macOS host)
scons platform=ios target=template_release ios_arch=arm64 -j$(sysctl -n hw.ncpu)
```

Binaries are output to `demo/bin/`.

## Platform Integration

### Android

The Android implementation requires a companion **Godot Android Plugin** to handle `Activity` results.

1. Build the Java plugin from `android/` into an AAR (see `android/README.md`)
2. Place the AAR in your Godot project's `android/plugins/` directory
3. Enable the plugin in **Project > Export > Android > Plugins**

The C++ side communicates with the Java plugin via Godot's `Engine` singleton system — no raw JNI required in the extension code.

**Permissions:** Depending on your target SDK, you may need:
- `android.permission.READ_EXTERNAL_STORAGE` (Android 9 and below)
- `android.permission.READ_MEDIA_IMAGES` (Android 13+)
- `android.permission.READ_MEDIA_VIDEO` (Android 13+)

> Using `ACTION_GET_CONTENT` with content URIs usually does **not** require storage permissions.

### iOS

1. Build the extension for iOS
2. Add the resulting `.a` static library to your Xcode export
3. Link these frameworks in your Xcode project:
   - `UIKit`
   - `Photos`
   - `PhotosUI`
   - `UniformTypeIdentifiers`
4. Add `NSPhotoLibraryUsageDescription` to your `Info.plist` (optional for PHPicker, but recommended)

### Desktop

No extra integration needed. The extension uses Godot's built-in `FileDialog` as a fallback.

## Usage

```gdscript
extends Node

@onready var gallery: NativeGallery = $NativeGallery

func _ready():
    gallery.image_selected.connect(_on_image_selected)
    gallery.video_selected.connect(_on_video_selected)
    gallery.cancelled.connect(_on_cancelled)
    gallery.error.connect(_on_error)

func _on_pick_image_pressed():
    gallery.popup_image_picker()

func _on_pick_video_pressed():
    gallery.popup_video_picker()

func _on_image_selected(path: String):
    var img = Image.load_from_file(path)
    if img:
        $TextureRect.texture = ImageTexture.create_from_image(img)

func _on_video_selected(path: String):
    print("Video selected: ", path)
    # Use VideoStreamPlayer or OS.shell_open(path)

func _on_cancelled():
    print("User cancelled")

func _on_error(code: int, message: String):
    push_error("NativeGallery error %d: %s" % [code, message])
```

## Signals

| Signal | Parameters | Description |
|--------|-----------|-------------|
| `image_selected` | `path: String` | Single image picked |
| `images_selected` | `paths: PackedStringArray` | Multiple images picked |
| `video_selected` | `path: String` | Single video picked |
| `videos_selected` | `paths: PackedStringArray` | Multiple videos picked |
| `cancelled` | — | User dismissed the picker |
| `error` | `code: int, message: String` | Platform error occurred |

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `default_type` | `enum` | `Image` | Default media type filter |
| `allow_multiple` | `bool` | `false` | Allow multiple selection |

## Demo

Open the `demo/` folder in Godot 4 after building the extension for your platform.

## License

MIT — see [LICENSE](LICENSE).
