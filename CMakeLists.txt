cmake_minimum_required(VERSION 4.0)

set(NotifyQt_MAJOR_VERSION 1)
set(NotifyQt_MINOR_VERSION 0)
set(NotifyQt_PATCH_VERSION 0)

project(notify-qt
    VERSION "${NotifyQt_MAJOR_VERSION}.${NotifyQt_MINOR_VERSION}.${NotifyQt_PATCH_VERSION}"
    DESCRIPTION "Qt binding for org.freedesktop.Notifications"
    HOMEPAGE_URL "https://github.com/ahm-forks/libnotify-qt"
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
add_subdirectory(data)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
