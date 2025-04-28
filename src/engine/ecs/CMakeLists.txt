target_sources(engine
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_LIST_DIR} FILES
        entity.ixx
        component_storage.ixx
        registry.ixx
        system.ixx
    PRIVATE
        entity.cpp
        registry.cpp
)