cmake_minimum_required(VERSION 3.16)

project(fzVideoPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg4.4.2)
set(SDL2_IMAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2/SDL2_image-devel-2.8.4-VC)
set(SDL2_TTF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2/SDL2_ttf-devel-2.24.0-VC)
set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2/SDL2-devel-2.30.11-VC)

include_directories(${FFMPEG_DIR}/include)
include_directories(${SDL2_IMAGE_DIR}/include)
include_directories(${SDL2_TTF_DIR}/include)
include_directories(${SDL2_DIR}/include)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 设置库文件目录
link_directories(${FFMPEG_DIR}/lib)
link_directories(${SDL2_IMAGE_DIR}/x64)
link_directories(${SDL2_TTF_DIR}/x64)
link_directories(${SDL2_DIR}/x64)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
	globalhelper.cpp
	globalhelper.h
	ctrlbar.cpp
	ctrlbar.h
	playlist.cpp
	playlist.h
	medialist.cpp
	medialist.h
	show.cpp
	show.h
	CustomSlider.cpp
	CustomSlider.h
	data.h
	title.cpp
	title.h
	sonic.h
	sonic.cpp
	VideoCtl.cpp
	VideoCtl.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(fzVideoPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET fzVideoPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(fzVideoPlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(fzVideoPlayer
            ${PROJECT_SOURCES}
	    fzVideoPlayer.qrc
        )
    endif()
endif()

target_link_libraries(fzVideoPlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets avcodec avdevice avformat avutil swresample swscale SDL2 SDL2_image SDL2_ttf.lib)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting anSDL2_image.lib
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.fzVideoPlayer)
endif()
set_target_properties(fzVideoPlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


include(GNUInstallDirs)
install(TARGETS fzVideoPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(fzVideoPlayer)
endif()
