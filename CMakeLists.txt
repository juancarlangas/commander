cmake_minimum_required(VERSION 3.22.1)

project(Commander
    VERSION 0.3
    DESCRIPTION "Programa para manipular combinaciones y performances en sintetizadores musicales"
    LANGUAGES C CXX)

# Variables auxiliares
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Buscar dependencias
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Pdf)

add_executable(Commander
	"${SRC_DIR}/main.cpp"
	"${SRC_DIR}/midi/Keyboard.cpp"
	"${SRC_DIR}/ui/form.cpp"
	"${SRC_DIR}/common/matroska.cpp"
	"${SRC_DIR}/ui/digits.cpp"
	"${SRC_DIR}/ui/fields.cpp"
	"${SRC_DIR}/ui/ncurses.cpp"
	"${SRC_DIR}/ui/printing.cpp"
	"${SRC_DIR}/ui/screen.cpp"
	"${SRC_DIR}/ui/Orchestra.cpp"
	"${SRC_DIR}/ui/elements/window.cpp"
	"${SRC_DIR}/ui/elements/windows/popup.cpp"
	"${SRC_DIR}/ui/elements/windows/boxed_window.cpp"
	"${SRC_DIR}/ui/elements/windows/boxed_windows/boxed_popup.cpp"
	"${SRC_DIR}/ui/elements/windows/popups/orchestra_element.cpp"
	"${SRC_DIR}/ui/elements/windows/popups/orchestra_elements/check_popup.cpp"
	"${SRC_DIR}/ui/elements/windows/popups/orchestra_elements/double_X_slider.cpp"
	"${SRC_DIR}/ui/elements/windows/popups/orchestra_elements/text_popup.cpp"
	"${SRC_DIR}/ui/elements/windows/popups/orchestra_elements/text_popups/field_popup.cpp"
	"${SRC_DIR}/data/Catalog.cpp"
	"${SRC_DIR}/data/tables.cpp"
	"${SRC_DIR}/data/Playlist.cpp"
	"${SRC_DIR}/common/math.cpp"
	"${SRC_DIR}/common/string.cpp"
	"${SRC_DIR}/utils/timer.cpp"
	"${SRC_DIR}/utils/environment.cpp"
)

# Incluir directorios
target_include_directories(Commander PRIVATE "${SRC_DIR}")

# Enlazar librerías
target_link_libraries(Commander PRIVATE
    ncurses panel asound jack
    Qt6::Core Qt6::Widgets Qt6::Pdf
)

# Configurar compilación
target_compile_features(Commander PUBLIC cxx_std_20)
target_compile_options(Commander PRIVATE -Wall -Wextra -Wpedantic -Werror)

# Propiedades del objetivo
set_target_properties(Commander PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    OUTPUT_NAME "commander"
)

# Exportar comandos de compilación
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
