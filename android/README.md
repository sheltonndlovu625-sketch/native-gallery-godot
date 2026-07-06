# Android Plugin Integration

The NativeGallery GDExtension requires this companion Godot Android Plugin to handle `Activity` results on Android.

## Why a Plugin?

Godot GDExtensions run as native libraries loaded by the engine. They do not have direct access to `Activity` lifecycle callbacks like `onActivityResult()`. This Java plugin bridges that gap by:
1. Receiving `onMainActivityResult` from the Godot Activity
2. Storing the picked URI(s)
3. Exposing query methods (`hasResult()`, `getLastResult()`, etc.) that the C++ extension polls via `Engine.get_singleton()`

## Building the Plugin

### Option A: AAR via Android Studio (Recommended)

1. Create a new Android Library module in Android Studio
2. Copy `NativeGalleryPlugin.java` into `src/main/java/org/godotengine/plugin/nativegallery/`
3. Add the Godot library dependency in `build.gradle`:
   ```gradle
   dependencies {
       implementation "org.godotengine:godot:4.2.0.stable"
   }
   ```
4. Build → Generate AAR
5. Copy `native-gallery-plugin-release.aar` to your Godot project's `android/plugins/`
6. In Godot: **Project > Export > Android > Plugins** → enable "NativeGalleryAndroid"

### Option B: Custom Android Build Template

1. Enable **Use Custom Build** in your Android export preset
2. Copy `NativeGalleryPlugin.java` into your custom build template source tree
3. Register the plugin in your main Activity or `GodotApp`:
   ```java
   @Override
   public GodotPlugin onPluginGetInstance(String name) {
       if ("NativeGalleryAndroid".equals(name)) {
           return new NativeGalleryPlugin(this);
       }
       return super.onPluginGetInstance(name);
   }
   ```
4. Export as usual

## Permissions

Add these to your Android export preset depending on your `targetSdk`:

- **Android 12 and below:** `android.permission.READ_EXTERNAL_STORAGE`
- **Android 13+:** `android.permission.READ_MEDIA_IMAGES` and `android.permission.READ_MEDIA_VIDEO`

> **Note:** If you use content URIs (which `ACTION_GET_CONTENT` returns), you typically do **not** need storage permissions. The URI grants temporary read access.

## Content URIs vs File Paths

`ACTION_GET_CONTENT` returns content URIs (e.g., `content://media/...`), not absolute file paths. If you need real file paths for FFmpeg or other native code, you must use a `ContentResolver` to open an `InputStream` and copy the file to your app's cache directory. This can be added to the Java plugin if needed.
