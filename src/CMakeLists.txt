find_package(Qt5 REQUIRED Widgets Multimedia)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
add_executable(renju ${SRC} ${RESOURCES})
target_link_libraries(renju Qt5::Widgets Qt5::Multimedia)
