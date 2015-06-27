cmake_minimum_required(VERSION 2.6)
project(libnotify-qt)

find_package(Qt4 REQUIRED)

set(CMAKE_BUILD_TYPE Release)

set(QT_USE_QTGUI FALSE)
set(QT_USE_QTCORE TRUE)
set(QT_USE_QTDBUS TRUE)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
include_directories(${QT_INCLUDE_DIR})

set(LIB_SRCS
	src/Notification.cpp
	src/OrgFreedesktopNotificationsInterface.cpp)

set(LIB_MOC_HDRS
	src/Notification.h
	src/OrgFreedesktopNotificationsInterface.h)

qt4_wrap_cpp(LIB_MOC_SRCS ${LIB_MOC_HDRS})

add_library(notify-qt SHARED ${LIB_MOC_SRCS} ${LIB_SRCS})
add_library(notify-qt_static STATIC ${LIB_MOC_SRCS} ${LIB_SRCS})

set_property(TARGET notify-qt PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(notify-qt PROPERTIES VERSION 0 SOVERSION 0.0.0)

target_link_libraries(notify-qt QtDBus)
target_link_libraries(notify-qt QtCore)

install(TARGETS notify-qt DESTINATION lib)
install(TARGETS notify-qt_static DESTINATION lib)
install(FILES ./src/Notification.h DESTINATION include/notify-qt)
