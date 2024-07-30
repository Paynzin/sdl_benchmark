add_rules("mode.debug", "mode.release")
add_requires("libsdl", "libsdl_image", "libsdl_ttf")

if is_plat("windows") then
    add_rules("win.sdk.application")
end

target("sdl_benchmark")
    set_kind("binary")
    add_files("src/*.c")
    add_packages("libsdl", "libsdl_image", "libsdl_ttf")
target_end()
