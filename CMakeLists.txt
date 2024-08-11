CMAKE_MINIMUM_REQUIRED( VERSION 3.22.1 )

PROJECT( Commander
	VERSION 0.3
	DESCRIPTION "Programa para manipular combinaciones y performances en sintetizadores musicales"
	LANGUAGES C CXX )

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

ADD_EXECUTABLE( Commander
	"${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/midi/Keyboard.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/form.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/common/matroska.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/digits.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/fields.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/ncurses.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/printing.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/screen.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/orchestra.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/window.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/boxed_window.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/boxed_windows/boxed_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/popups/orchestra_element.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/popups/orchestra_elements/check_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/popups/orchestra_elements/double_X_slider.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/popups/orchestra_elements/text_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/elements/windows/popups/orchestra_elements/text_popups/field_popup.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/data/Catalog.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/data/tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/data/Playlist.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/common/math.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/common/string.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/utils/timer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/utils/environment.cpp"
	)
	
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
