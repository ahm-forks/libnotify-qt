set(PUBH include/libnotify-qt.h)

set(SRCS
	${PUBH}

	Interface.h Interface.cpp

	logging.h

	libnotify-qt_p.h Manager.cpp Event.cpp
)

foreach(SELECTED ${QT_MAJOR_ENABLED})
	set(${PROJECT_NAME}${SELECTED}_TARGETS)
	set(Qt${SELECTED}Libs
		Qt${SELECTED}::Core
		Qt${SELECTED}::DBus
		Qt${SELECTED}::Gui
	)
	set(BASENAME ${PROJECT_NAME}${SELECTED})

	if(BUILD_STATIC)
		add_library(${BASENAME}_static STATIC ${SRCS})

		target_link_libraries(${BASENAME}_static PRIVATE ${Qt${SELECTED}Libs})
		target_include_directories(${BASENAME}_static INTERFACE
		  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
		)

		list(APPEND ${BASENAME}_TARGETS ${BASENAME}_static)
	endif()

	if(BUILD_SHARED)
		add_library(${BASENAME} SHARED ${SRCS})

		target_link_libraries(${BASENAME} PRIVATE ${Qt${SELECTED}Libs})
		target_include_directories(${BASENAME} INTERFACE
		  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
		)
		set_target_properties(${BASENAME} PROPERTIES
			VERSION "${PROJECT_VERSION}"
			SOVERSION "${NotifyQt_MAJOR_VERSION}.${NotifyQt_MINOR_VERSION}"
		)
		list(APPEND ${BASENAME}_TARGETS ${BASENAME})
	endif()

	install(
		TARGETS ${${BASENAME}_TARGETS}
		EXPORT ${BASENAME}Targets
	)

	install(EXPORT ${BASENAME}Targets
		    FILE ${PROJECT_NAME}Targets.cmake
		    NAMESPACE ${BASENAME}::
		    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${BASENAME}
	)
	export(EXPORT ${BASENAME}Targets
		   FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${BASENAME}Targets.cmake"
		   NAMESPACE ${BASENAME}::
	)
endforeach()

install(
	FILES ${PUBH}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)
