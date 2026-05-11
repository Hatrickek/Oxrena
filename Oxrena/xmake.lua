target("Oxrena")
set_kind("binary")
set_languages("cxx23")
add_rpathdirs("@executable_path")

add_includedirs(".")
add_files("./src/**.cpp")

add_packages("oxylus")

add_files("./Assets/**")
add_rules("@oxylus/install_resources", {
  root_dir = os.scriptdir() .. "/Assets",
  output_dir = "Assets",
})
add_rules("@oxylus/install_shaders", {
  output_dir = "Assets/Shaders",
})
target_end()
