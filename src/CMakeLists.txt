find_package(Qt5 COMPONENTS Core Quick REQUIRED)
file(GLOB SRC "*.cpp" "*.h")
add_executable(${PROJECT_NAME} WIN32 ${SRC} "qml/qml.qrc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick)

