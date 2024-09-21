cmake_minimum_required(VERSION 3.16)
project(ezanvakti-qt-gui VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(ezanvakti-qt-gui WIN32 MACOSX_BUNDLE
    main.cpp
    qtezanvakti.cpp qtezanvakti.h qtezanvakti.ui
)
target_compile_definitions(ezanvakti-qt-gui PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(ezanvakti-qt-gui PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Multimedia
)


# Resources:
set(images_resource_files
    "images/ezanvakti96.png"
)

qt_add_resources(ezanvakti-qt-gui "images"
    PREFIX
        "/"
    FILES
        ${images_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(ezanvakti-qt-gui PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS ezanvakti-qt-gui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ezanvakti-qt-gui
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
