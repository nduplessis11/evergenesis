add_executable(${PROJECT_NAME})

# Copy the assets directory to the output binary directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        COMMENT "Copying assets directory to output binary folder..."
)

target_sources(${PROJECT_NAME}
    PRIVATE main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE vendor
    PRIVATE renderer
)

add_subdirectory(renderer)