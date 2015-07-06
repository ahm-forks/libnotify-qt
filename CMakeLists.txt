cmake_minimum_required(VERSION 2.8.11)

project(libnotify-qt)

set(PROJECT_VERSION 1.0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5DBus)

execute_process(COMMAND qmake -query QT_HOST_DATA OUTPUT_VARIABLE QMAKEMKSPECS
								OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(${QT_DEFINITIONS})
add_definitions(-D_GLIBCXX_VISIBILITY=0 -D_NDEBUG -DQT_NO_DEBUG_OUTPUT)

set(LIB_SRCS src/Notification.cpp src/OrgFreedesktopNotificationsInterface.cpp)

add_library(notify-qt_static STATIC ${LIB_SRCS})

if(${BUILD_SHARED_LIBS})
add_library(notify-qt SHARED ${LIB_SRCS})

set_property(TARGET notify-qt PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(notify-qt PROPERTIES VERSION 1.0.1 SOVERSION 1.0.1)

target_link_libraries(notify-qt Qt5::Core)
target_link_libraries(notify-qt Qt5::DBus)
endif(${BUILD_SHARED_LIBS})

target_link_libraries(notify-qt_static Qt5::Core)
target_link_libraries(notify-qt_static Qt5::DBus)

configure_file(${CMAKE_SOURCE_DIR}/libnotify-qt.pc.in
			   ${PROJECT_BINARY_DIR}/libnotify-qt.pc @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/libnotify-qt.prf.in
			   ${PROJECT_BINARY_DIR}/libnotify-qt.prf @ONLY)

if(${BUILD_SHARED_LIBS})
install(TARGETS notify-qt DESTINATION lib)
endif(${BUILD_SHARED_LIBS})

install(TARGETS notify-qt_static DESTINATION lib)
install(FILES src/Notification.h DESTINATION include/libnotify-qt)
install(FILES ${PROJECT_BINARY_DIR}/libnotify-qt.pc DESTINATION lib/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/libnotify-qt.prf DESTINATION ${QMAKEMKSPECS}/mkspecs/features)
