CMAKE_MINIMUM_REQUIRED( VERSION 3.18.4 )

PROJECT( Commander LANGUAGES C CXX VERSION 0.3 )

ADD_EXECUTABLE( Commander
			"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/midi/audio.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/channelForm.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/midi/communicator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/data/databases.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/digits.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/fields.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/data/files.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/form.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/midi/keyboards.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/common/math.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/ncurses.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/printing.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/screen.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/common/string.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/src/data/tables.cpp"
			"/home/juancarlangas/Programming/lib/utilities/src/files.cpp" )
	
target_include_directories( Commander
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
	PRIVATE	"/home/juancarlangas/Programming/lib/" )

target_link_libraries( Commander ncurses panel asound )

target_compile_features( Commander PUBLIC cxx_std_20 )

set_target_properties( Commander
			PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" 
			OUTPUT_NAME "commander" )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
