set(PUBH libnotify-qt.h)

set(SRCS
	Interface.h Interface.cpp

	Manager.cpp Event.cpp
)

set(Qt5Libs
	Qt5::Core
	Qt5::DBus
	Qt5::Gui
)

add_library(${PROJECT_NAME}5_static STATIC ${PUBH} ${SRCS})
target_link_libraries(${PROJECT_NAME}5_static PUBLIC ${Qt5Libs})

add_library(${PROJECT_NAME}5 SHARED ${PUBH} ${SRCS})
target_link_libraries(${PROJECT_NAME}5 PUBLIC ${Qt5Libs})
install(TARGETS ${PROJECT_NAME}5 ${PROJECT_NAME}5_static)

set_target_properties(${PROJECT_NAME}5 PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${NotifyQt_MAJOR_VERSION}.${NotifyQt_MINOR_VERSION}"
)


if (Qt6_FOUND)
set(Qt6Libs
	Qt6::Core
	Qt6::DBus
	Qt6::Gui
)
add_library(${PROJECT_NAME}_static STATIC ${PUBH} ${SRCS})
target_link_libraries(${PROJECT_NAME}_static PUBLIC ${Qt6Libs})

add_library(${PROJECT_NAME} SHARED ${PUBH} ${SRCS})
set_target_properties(${PROJECT_NAME}
PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${NotifyQt_MAJOR_VERSION}.${NotifyQt_MINOR_VERSION}"
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Qt6Libs})

install(
	TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
)
endif()

install(
	FILES ${PUBH}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)
