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
set(PROJECT_NAME_CAMELCASE notifyQt)

find_package(ECM REQUIRED NO_MODULE)

list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTOMOC ON)
set(QT_MINIMUM_VERSION "5.15.2")
set(KDE_INSTALL_USE_QT_SYS_PATHS OFF)

include(QtPathFinder)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(FeatureSummary)
include(ECMGeneratePriFile)
include(ECMGeneratePkgConfigFile)
include(ECMUninstallTarget)

option(BUILD_STATIC "Build the static library" ON)
option(BUILD_SHARED "Build the shared library" ON)

set(QT_MAJOR_SUPPORTED 5 6)
set(QT_MAJOR_ENABLED)
foreach(SELECTED ${QT_MAJOR_SUPPORTED})
	option(ENABLE_QT${SELECTED} "Build for Qt${SELECTED}" ON)
endforeach()

block(SCOPE_FOR VARIABLES PROPAGATE QT_MAJOR_ENABLED)
	foreach(SELECTED ${QT_MAJOR_SUPPORTED})
		if(ENABLE_QT${SELECTED})
			list(APPEND QT_MAJOR_ENABLED ${SELECTED})
		endif()
	endforeach()

	list(LENGTH QT_MAJOR_ENABLED ENABLED_QTS)
	if(${ENABLED_QTS} EQUAL 0)
		message(FATAL_ERROR "At least one Qt Major version should be enabled.")
	endif()
endblock()

if(NOT (BUILD_SHARED OR BUILD_STATIC))
	message(FATAL_ERROR "Either BUILD_SHARED or BUILD_STATIC must be enabled.")
endif()

set(QT_REQUIRED_COMPONENTS Core DBus Gui)
foreach(SELECTED ${QT_MAJOR_ENABLED})
	find_package(Qt${SELECTED} COMPONENTS ${QT_REQUIRED_COMPONENTS})
endforeach()

add_subdirectory(src)
add_subdirectory(data)
