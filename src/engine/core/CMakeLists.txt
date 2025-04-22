add_library(core STATIC)

target_sources(core
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
        graphics_context.ixx
        engine_core.ixx
        types/color.ixx
    PRIVATE graphics_context.cpp
)

target_link_libraries(core
    PRIVATE vendor
)
