cmake_minimum_required(VERSION 4.0)

set(NotifyQt_MAJOR_VERSION 1)
set(NotifyQt_MINOR_VERSION 0)
set(NotifyQt_PATCH_VERSION 0)

project(notify-qt
    VERSION "${NotifyQt_MAJOR_VERSION}.${NotifyQt_MINOR_VERSION}.${NotifyQt_PATCH_VERSION}"
    DESCRIPTION "Qt5/Qt6 binding for org.freedesktop.Notifications"
    HOMEPAGE_URL "https://github.com/ahm-forks/libnotify-qt.git"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
set(CMAKE_AUTOMOC ON)
set(QT_MINIMUM_VERSION "5.15.2")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(FeatureSummary)

find_package(Qt6 COMPONENTS Core DBus Gui)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Gui)

add_subdirectory(src)

#configure_package_config_file()
