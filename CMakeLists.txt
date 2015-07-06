cmake_minimum_required(VERSION 2.8.7)

project(libnotify-qt)

set(PROJECT_VERSION 0.0.1)

include(GenerateExportHeader)
include(CheckCCompilerFlag)
include(FindPkgConfig)

find_package(Qt4)

set(QT_USE_QTDBUS TRUE)
set(QT_USE_QTCORE TRUE)

include(${QT_USE_FILE})

execute_process(COMMAND qmake -query QMAKE_MKSPECS OUTPUT_VARIABLE QMAKEMKSPECS
				OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_CAST_TO_ASCII -DQT_USE_FAST_OPERATOR_PLUS
				-DQT_USE_FAST_CONCATENATION -DQT_STRICT_ITERATORS -D_GLIBCXX_VISIBILITY=0
				-D_NDEBUG -DQT_NO_DEBUG_OUTPUT)

include_directories(${QT_INCLUDE_DIR})
include_directories(${PROJECT_BINARY_DIR})

set(LIB_SRCS ./src/Notification.cpp ./src/OrgFreedesktopNotificationsInterface.cpp)
set(LIB_MOC_HDRS ./src/Notification.h ./src/OrgFreedesktopNotificationsInterface.h)

check_cxx_compiler_flag(-Wa,--noexecstack COMPILE_NOEXECSTACK)

if(${COMPILE_NOEXECSTACK})
set_property(SOURCE ${LIB_SRCS} ${LIB_MOC_SRCS} APPEND PROPERTY COMPILE_FLAGS -Wa,--noexecstack)
endif(${COMPILE_NOEXECSTACK})

qt4_wrap_cpp(LIB_MOC_SRCS ${LIB_MOC_HDRS})

add_library(notify-qt_static STATIC ${LIB_MOC_SRCS} ${LIB_SRCS})

if(${BUILD_SHARED_LIBS})
add_library(notify-qt SHARED ${LIB_MOC_SRCS} ${LIB_SRCS})
endif(${BUILD_SHARED_LIBS})

if(${BUILD_SHARED_LIBS})
set_property(TARGET notify-qt PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(notify-qt PROPERTIES VERSION 0.0.1 SOVERSION 0.0.1)
target_link_libraries(notify-qt ${QT_LIBRARIES})
endif(${BUILD_SHARED_LIBS})

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
install(FILES ${PROJECT_BINARY_DIR}/libnotify-qt.prf DESTINATION ${QMAKEMKSPECS}/features)
