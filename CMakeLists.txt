cmake_minimum_required(VERSION 3.19)
project(MusicPlayer LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia Sql)

qt_standard_project_setup()

qt_add_executable(MusicPlayer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    user.h user.cpp
    authmanager.h authmanager.cpp
    logindialog.h logindialog.cpp
    track.h track.cpp
    playlist.h playlist.cpp
    libraryscanner.h libraryscanner.cpp
    marqueelabel.h marqueelabel.cpp
)

target_link_libraries(MusicPlayer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Multimedia
        Qt6::Sql
)

install(TARGETS MusicPlayer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MusicPlayer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
