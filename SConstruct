#!/usr/bin/env python
import os
import sys

# Ensure godot-cpp submodule is present
if not os.path.exists("godot-cpp/SConstruct"):
    print("ERROR: godot-cpp submodule not found.")
    print("Run: git submodule update --init --recursive")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct")

# Add our source path
env.Append(CPPPATH=["src/"])

# Core sources (always compiled)
sources = [
    "src/register_types.cpp",
    "src/native_gallery.cpp",
]

# Platform-specific implementations
if env["platform"] == "android":
    sources.append("src/native_gallery_android.cpp")
elif env["platform"] == "ios":
    sources.append("src/native_gallery_ios.mm")
    # iOS frameworks for PHPickerViewController
    env.Append(LINKFLAGS=["-framework", "UIKit"])
    env.Append(LINKFLAGS=["-framework", "Photos"])
    env.Append(LINKFLAGS=["-framework", "PhotosUI"])
    env.Append(LINKFLAGS=["-framework", "UniformTypeIdentifiers"])
else:
    # Desktop fallback (Linux, Windows, macOS)
    sources.append("src/native_gallery_desktop.cpp")

# Library output paths
lib_name = "libnative_gallery"

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/{0}.macos.{1}.framework/{0}.macos.{1}".format(lib_name, env["target"]),
        source=sources,
    )
elif env["platform"] == "ios":
    library = env.StaticLibrary(
        "demo/bin/{0}.ios.{1}.a".format(lib_name, env["target"]),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/{0}{1}{2}".format(lib_name, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
