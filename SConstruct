#!/usr/bin/env python
import os
 
env = SConscript("godot-cpp/SConstruct")
 
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")
 
# Apple Silicon specific flags
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