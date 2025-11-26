add_executable(lookup_cli main.cc)

target_link_libraries(lookup_cli PRIVATE
    config_compiler_flags_library
    desk_up_backend_interface_library
)
