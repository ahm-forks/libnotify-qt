include(CheckLanguage)

check_language(CXX)
if (CMAKE_CXX_COMPILER)
	# Enable the CXX language to let CMake look for config files in library dirs.
	# See: https://gitlab.kitware.com/cmake/cmake/-/issues/23266
	enable_language(CXX)
endif()

# QUIET to accommodate the TRY option
find_package(Qt5Core QUIET)
set(_qt5_exec_name_text "Qt5 qmake")
if(TARGET Qt5::qmake)
	get_target_property(_qmake_executable_default Qt5::qmake LOCATION)

	set(QT5_QUERY_EXECUTABLE ${_qmake_executable_default})
	set(_qt5_cli_option "-query")
endif()

# QUIET to accommodate the TRY option
find_package(Qt6 COMPONENTS CoreTools QUIET CONFIG)
set(_qt6_exec_name_text "Qt6 qtpaths")
if (TARGET Qt6::qtpaths)
	get_target_property(_qtpaths_executable Qt6::qtpaths LOCATION)

	set(QT6_QUERY_EXECUTABLE ${_qtpaths_executable})
	set(_qt6_cli_option "--query")
endif()

function(qt_path_find result_variable qt_variable)
	set(options TRY RELATIVE)
	set(oneValueArgs VERSION)
	set(multiValueArgs)
	cmake_parse_arguments(QPF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT QPF_VERSION)
		set(QPF_VERSION ${QT_MAJOR_VERSION})
	endif()
	if(QPF_RELATIVE)
		string(REPLACE "_" ";" QPF_RELATIVE "${qt_variable}")
		list(GET QPF_RELATIVE 0 QPF_RELATIVE)
	endif()
	set(qt_variable "QT_${qt_variable}")
	if(NOT QT${QPF_VERSION}_QUERY_EXECUTABLE)
		if(ARGS_TRY)
			set(${result_variable} "" PARENT_SCOPE)
			message(STATUS "No ${_qt${QPF_VERSION}_exec_name_text} executable found. Can't check ${qt_variable}")
			return()
		else()
			message(FATAL_ERROR "No ${_qt${QPF_VERSION}_exec_name_text} executable found. Can't check ${qt_variable} as required")
		endif()
	endif()

	execute_process(
		COMMAND ${QT${QPF_VERSION}_QUERY_EXECUTABLE} ${_qt${QPF_VERSION}_cli_option} "${qt_variable}"
		RESULT_VARIABLE return_code
		OUTPUT_VARIABLE output
	)
	if(return_code EQUAL 0)
		string(STRIP "${output}" output)
		file(TO_CMAKE_PATH "${output}" output_path)
		if(QPF_RELATIVE)
			execute_process(
				COMMAND ${QT${QPF_VERSION}_QUERY_EXECUTABLE} ${_qt${QPF_VERSION}_cli_option} "QT_${QPF_RELATIVE}_PREFIX"
				RESULT_VARIABLE return_code
				OUTPUT_VARIABLE prefix
			)
			string(STRIP "${prefix}" prefix)
			file(TO_CMAKE_PATH "${prefix}" prefix_path)
			file(RELATIVE_PATH output_path ${prefix_path} ${output_path})
		endif()
		set(${result_variable} "${output_path}" PARENT_SCOPE)
	else()
		message(WARNING "Failed call: ${QT${QPF_VERSION}_QUERY_EXECUTABLE} ${_qt${QPF_VERSION}_cli_option} ${qt_variable}")
		message(FATAL_ERROR "${_qt${QPF_VERSION}_exec_name_text} call failed: ${return_code}")
	endif()
endfunction()
