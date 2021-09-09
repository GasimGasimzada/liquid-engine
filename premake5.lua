workspace "LiquidEngine"
    basedir "workspace/"
    configurations { "Debug" }
    language "C++"
    cppdialect "C++17"

    sysincludedirs {
        "vendor/include",
        "/usr/local/include",
    }

    includedirs {
        "engine/src/"
    }

    libdirs {
        "vendor/lib",
        "/usr/local/lib",
    }

    filter { "system:linux" }
        linkoptions { "-pthread" }

    filter { "system:macosx" }
        xcodebuildsettings {
            ["MACOSX_DEPLOYMENT_TARGET"] = "11.1",
            ["ONLY_ACTIVE_ARCH"] = "YES",
            ["ENABLE_TESTABILITY"] = "YES",
            ["ARCHS"] = "$(ARCHS_STANDARD)",
            ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
            ["CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING"] = "YES",
            ["CLANG_WARN_BOOL_CONVERSION"] = "YES",
            ["CLANG_WARN_COMMA"] = "YES",
            ["CLANG_WARN_CONSTANT_CONVERSION"] = "YES",
            ["CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS"] = "YES",
            ["CLANG_WARN_EMPTY_BODY"] = "YES",
            ["CLANG_WARN_ENUM_CONVERSION"] = "YES",
            ["CLANG_WARN_INFINITE_RECURSION"] = "YES",
            ["CLANG_WARN_INT_CONVERSION"] = "YES",
            ["CLANG_WARN_NON_LITERAL_NULL_CONVERSION"] = "YES",
            ["CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF"] = "YES",
            ["CLANG_WARN_OBJC_LITERAL_CONVERSION"] = "YES",
            ["CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER"] = "YES",
            ["CLANG_WARN_RANGE_LOOP_ANALYSIS"] = "YES",
            ["CLANG_WARN_STRICT_PROTOTYPES"] = "YES",
            ["CLANG_WARN_SUSPICIOUS_MOVE"] = "YES",
            ["CLANG_WARN_UNREACHABLE_CODE"] = "YES",
            ["CLANG_WARN__DUPLICATE_METHOD_MATCH"] = "YES",
            ["GCC_WARN_64_TO_32_BIT_CONVERSION"] = "YES",
            ["GCC_WARN_ABOUT_RETURN_TYPE"] = "YES",
            ["GCC_WARN_UNDECLARED_SELECTOR"] = "YES",
            ["GCC_WARN_UNINITIALIZED_AUTOS"] = "YES",
            ["GCC_WARN_UNUSED_FUNCTION"] = "YES",
            ["GCC_WARN_UNUSED_VARIABLE"] = "YES",
            ["GCC_NO_COMMON_BLOCKS"] = "YES",
            ["ENABLE_STRICT_OBJC_MSGSEND"] = "YES",
        }

        filter { "configurations:Debug" }
            xcodebuildsettings {
                ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
                ["CODE_SIGN_IDENTITY"] = "-"
            }

project "LiquidEngine"
    basedir "workspace/engine/"
    kind "StaticLib"

    pchheader "../../engine/src/core/Base.h"

    files {
        "engine/src/**.h",
        "engine/src/**.cpp",
        "vendor/src/imgui/*.cpp"
    }

    removefiles {
        "vendor/src/imgui/imgui_impl_vulkan.cpp"
    }

    configuration "Debug"
        defines { "LIQUID_DEBUG" }

    links { "vulkan", "glfw", "ktx" }

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../engine/assets/shaders/pbr.vert -o %{cfg.buildtarget.directory}/assets/shaders/pbr.vert.spv",
        "glslc ../../engine/assets/shaders/pbr.frag -o %{cfg.buildtarget.directory}/assets/shaders/pbr.frag.spv",
        "glslc ../../engine/assets/shaders/skybox.frag -o %{cfg.buildtarget.directory}/assets/shaders/skybox.frag.spv",
        "glslc ../../engine/assets/shaders/skybox.vert -o %{cfg.buildtarget.directory}/assets/shaders/skybox.vert.spv",
        "glslc ../../engine/assets/shaders/shadowmap.frag -o %{cfg.buildtarget.directory}/assets/shaders/shadowmap.frag.spv",
        "glslc ../../engine/assets/shaders/shadowmap.vert -o %{cfg.buildtarget.directory}/assets/shaders/shadowmap.vert.spv",
        "glslc ../../engine/assets/shaders/imgui.frag -o %{cfg.buildtarget.directory}/assets/shaders/imgui.frag.spv",
        "glslc ../../engine/assets/shaders/imgui.vert -o %{cfg.buildtarget.directory}/assets/shaders/imgui.vert.spv"
    }

project "LiquidEngineTest"
    basedir "workspace/engine-test/"
    kind "ConsoleApp"

    pchheader "../../engine/src/core/Base.h"

    files {
        "engine/src/**.cpp",
        "engine/src/**.h",
        "vendor/src/imgui/*.cpp",
        "engine/tests/**.cpp",
        "engine/tests/**.h"
    }

    removefiles {
        "engine/src/renderer/vulkan/VmaImpl.cpp",
        "vendor/src/imgui/imgui_impl_vulkan.cpp"
    }

    postbuildcommands {
        "{COPY} ../../engine/tests/fixtures/white-image-100x100.png %{cfg.buildtarget.directory}/white-image-100x100.png",
        "{COPY} ../../engine/tests/fixtures/1x1-cubemap.ktx %{cfg.buildtarget.directory}/1x1-cubemap.ktx",
        "{COPY} ../../engine/tests/fixtures/1x1-2d.ktx %{cfg.buildtarget.directory}/1x1-2d.ktx",
        "{COPY} ../../engine/tests/fixtures/1x1-1d.ktx %{cfg.buildtarget.directory}/1x1-1d.ktx"
    }

    includedirs {
        "engine/src"
    }

    configuration "Debug"
        defines { "LIQUID_DEBUG" }

    links { "glfw", "gtest", "gtest_main", "gmock", "ktx" }

    filter { "system:linux" }
        links { "dl" }

    filter { "toolset:clang" }
        buildoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

        linkoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

    filter { "toolset:gcc" }
        buildoptions {
            "-fprofile-arcs",
            "-ftest-coverage"
        }

        linkoptions {
            "-fprofile-arcs",
            "-ftest-coverage"
        }

project "DemoBasicTriangle"
    basedir "workspace/demos/basic-triangle"
    kind "ConsoleApp"
    configurations { "Debug" }

    files {
        "demos/basic-triangle/src/**.h",
        "demos/basic-triangle/src/**.cpp"
    }

    links { "vulkan", "glfw", "ktx", "LiquidEngine" }

    postbuildcommands {
        "glslc ../../../demos/basic-triangle/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/basic-triangle/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/red-shader.frag -o %{cfg.buildtarget.directory}/red-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/texture-shader.frag -o %{cfg.buildtarget.directory}/texture-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/texture-shader.vert -o %{cfg.buildtarget.directory}/texture-shader.vert.spv",
        "{COPY} ../../../demos/basic-triangle/assets/textures/brick.png %{cfg.buildtarget.directory}/brick.png"
    }

project "DemoRotatingCube"
    basedir "workspace/demos/rotating-cube"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "demos/rotating-cube/src/**.h",
        "demos/rotating-cube/src/**.cpp"
    }

    links { "vulkan", "glfw", "ktx", "LiquidEngine" }

    postbuildcommands {
        "glslc ../../../demos/rotating-cube/assets/texture-shader.vert -o %{cfg.buildtarget.directory}/texture-shader.vert.spv",
        "glslc ../../../demos/rotating-cube/assets/texture-shader.frag -o %{cfg.buildtarget.directory}/texture-shader.frag.spv",
        "glslc ../../../demos/rotating-cube/assets/material-shader.vert -o %{cfg.buildtarget.directory}/material-shader.vert.spv",
        "glslc ../../../demos/rotating-cube/assets/material-shader.frag -o %{cfg.buildtarget.directory}/material-shader.frag.spv",
        "{COPY} ../../../demos/rotating-cube/assets/textures/brick.png %{cfg.buildtarget.directory}/brick.png"
    }


project "DemoTransformingSpheres"
    basedir "workspace/demos/earth-and-moon"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "demos/transforming-spheres/src/**.h",
        "demos/transforming-spheres/src/**.cpp"
    }

    links { "vulkan", "glfw", "ktx", "LiquidEngine" }

    postbuildcommands {
        "glslc ../../../demos/transforming-spheres/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/transforming-spheres/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv",
        "glslc ../../../demos/transforming-spheres/assets/texture-shader.vert -o %{cfg.buildtarget.directory}/texture-shader.vert.spv",
        "glslc ../../../demos/transforming-spheres/assets/texture-shader.frag -o %{cfg.buildtarget.directory}/texture-shader.frag.spv",
        "{COPY} ../../../demos/transforming-spheres/assets/textures/lava.jpg %{cfg.buildtarget.directory}/lava.jpg",
        "{COPY} ../../../demos/transforming-spheres/assets/textures/water.png %{cfg.buildtarget.directory}/water.png",
        "{COPY} ../../../demos/transforming-spheres/assets/textures/grass.png %{cfg.buildtarget.directory}/grass.png"
    }

project "DemoPong"
    basedir "workspace/demos/pong-3d"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "demos/pong-3d/src/**.h",
        "demos/pong-3d/src/**.cpp"
    }

    links { "vulkan", "glfw", "ktx", "LiquidEngine" }

    postbuildcommands {
        "glslc ../../../demos/pong-3d/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/pong-3d/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv"
    }

project "DemoSceneViewer"
    basedir "workspace/demos/scene-viewer"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "demos/scene-viewer/src/**.h",
        "demos/scene-viewer/src/**.cpp"
    }

    links { "vulkan", "glfw", "ktx", "LiquidEngine" }

    filter { "system:linux" }
        removefiles { "demos/scene-viewer/src/**.win32.cpp" }
    
    filter { "system:windows" }
        removefiles { "demos/scene-viewer/src/**.linux.cpp" }

    filter { "system:macosx" }
        files {
            "demos/scene-viewer/src/**.mm"
        }

        removefiles { "demos/scene-viewer/src/**.linux.cpp", "demos/scene-viewer/src/**.win32.cpp" }

        links { "AppKit.framework" }

    
