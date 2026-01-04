find_package(Qt6 COMPONENTS Core Quick Concurrent REQUIRED)
file(GLOB SRC "*.cpp" "*.h")
add_executable(${PROJECT_NAME} WIN32 ${SRC} "qml/qml.qrc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick Qt6::Concurrent)

