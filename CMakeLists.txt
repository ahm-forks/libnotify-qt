cmake_minimum_required(VERSION 2.6)
project(libnotify-qt)

find_package(Qt4 REQUIRED)

set(CMAKE_BUILD_TYPE Release)

set(QT_USE_QTCORE TRUE)
set(QT_USE_QTDBUS TRUE)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
include_directories(${QT_INCLUDE_DIR})

set(LIB_SRCS
	./src/Notification.cpp
	./src/OrgFreedesktopNotificationsInterface.cpp)

set(LIB_MOC_HDRS
	./src/Notification.h
	./src/OrgFreedesktopNotificationsInterface.h)

qt4_wrap_cpp(LIB_MOC_SRCS ${LIB_MOC_HDRS})

add_library(libnotify-qt SHARED ${LIB_MOC_SRCS} ${LIB_SRCS})
add_library(libnotify-qt_static STATIC ${LIB_MOC_SRCS} ${LIB_SRCS})

set_property(TARGET libnotify-qt PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(libnotify-qt PROPERTIES VERSION 0 SOVERSION 0.0.0)

target_link_libraries(libnotify-qt QtDBus)
target_link_libraries(libnotify-qt QtCore)

install(TARGETS libnotify-qt DESTINATION lib)
install(TARGETS libnotify-qt_static DESTINATION lib)
install(FILES ./src/Notification.h DESTINATION include)
