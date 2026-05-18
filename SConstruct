#!/usr/bin/env python
import os

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=[
    "src/",
    "godot-cpp/include/",
    "godot-cpp/gen/include/",
])
sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    env.Append(CCFLAGS=["-arch", "arm64"])
    env.Append(LINKFLAGS=["-arch", "arm64"])

library = env.SharedLibrary(
    "game/bin/libalbularyo{}{}".format(
        env["suffix"], env["SHLIBSUFFIX"]
    ),
    source=sources,
)

Default(library)