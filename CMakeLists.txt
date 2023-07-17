CMAKE_MINIMUM_REQUIRED( VERSION 3.22.1 )

PROJECT( Commander
	VERSION 0.3
	DESCRIPTION "Programa para manipular combinaciones y performances en sintetizadores musicales"
	LANGUAGES C CXX )

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

ADD_EXECUTABLE( Commander
	"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/midi/keyboards.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/form.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/common/matroska.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/digits.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/fields.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ncurses.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/printing.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/screen.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/orchestra.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/window.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/boxed_window.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/boxed_windows/boxed_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/popups/orchestra_element.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/popups/orchestra_elements/check_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/popups/orchestra_elements/double_X_slider.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/popups/orchestra_elements/text_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/elements/windows/popups/orchestra_elements/text_popups/field_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/data/catalog.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/data/tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/data/playlist.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/common/math.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/common/string.cpp" )
	#"/home/juancarlangas/Programming/lib/utilities/src/files.cpp" )
	
target_include_directories( Commander
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/data" )
	#PRIVATE	"/home/juancarlangas/Programming/lib/" )

target_link_libraries( Commander ncurses panel asound jack)

target_compile_features( Commander PUBLIC cxx_std_20 )

target_compile_options( Commander PRIVATE -Wall -Wextra -Wpedantic -Werror )

set_target_properties( Commander
			PROPERTIES
			CXX_STANDARD 20
			RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" 
			OUTPUT_NAME "commander" )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
