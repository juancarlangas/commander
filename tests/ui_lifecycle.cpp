#include "ui/elements/windows/boxed_windows/boxed_popup.hpp"
#include "ui/elements/windows/popup.hpp"
#include "ui/form.hpp"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace {
std::unordered_set<WINDOW*> windows;
std::unordered_map<PANEL*, WINDOW*> panels;

void require(bool condition, const char* message)
{
    if (!condition) {
        std::fprintf(stderr, "%s\n", message);
        std::abort();
    }
}

void check_counts(std::size_t window_count, std::size_t panel_count)
{
    require(windows.size() == window_count, "Unexpected number of live windows");
    require(panels.size() == panel_count, "Unexpected number of live panels");
}
}

extern "C" {
WINDOW* __real_newwin(int, int, int, int);
int __real_delwin(WINDOW*);
PANEL* __real_new_panel(WINDOW*);
int __real_del_panel(PANEL*);

WINDOW* __wrap_newwin(int height, int width, int y, int x)
{
    WINDOW* window = __real_newwin(height, width, y, x);
    require(window != nullptr, "newwin failed");
    require(windows.insert(window).second, "Window already owned");
    return window;
}

int __wrap_delwin(WINDOW* window)
{
    for (const auto& entry : panels)
        require(entry.second != window, "Window deleted before its panel");
    require(windows.erase(window) == 1, "Window deleted twice or not owned");
    int result = __real_delwin(window);
    require(result == OK, "delwin failed");
    return result;
}

PANEL* __wrap_new_panel(WINDOW* window)
{
    PANEL* panel = __real_new_panel(window);
    require(panel != nullptr, "new_panel failed");
    require(panels.emplace(panel, window).second, "Panel already owned");
    return panel;
}

int __wrap_del_panel(PANEL* panel)
{
    require(panels.erase(panel) == 1, "Panel deleted twice or not owned");
    int result = __real_del_panel(panel);
    require(result == OK, "del_panel failed");
    return result;
}
}

static_assert(!std::is_copy_constructible_v<Window>);
static_assert(!std::is_move_constructible_v<Window>);
static_assert(!std::is_copy_constructible_v<Popup>);
static_assert(!std::is_copy_constructible_v<BoxedWindow>);
static_assert(!std::is_copy_constructible_v<BoxedPopup>);
static_assert(!std::is_copy_constructible_v<Field>);
static_assert(!std::is_copy_constructible_v<Form>);

int main()
{
    // Real ncurses resources, with output redirected instead of requiring a TTY.
    FILE* output = std::tmpfile();
    FILE* input = std::tmpfile();
    require(output && input, "Cannot create terminal streams");
    SCREEN* terminal = newterm("xterm", output, input);
    require(terminal != nullptr, "Cannot initialize ncurses");
    require(resizeterm(60, 200) == OK, "Cannot set terminal dimensions");

    // Destruction before init must also be safe.
    { Window window; Popup popup; BoxedWindow box; BoxedPopup boxed_popup; Field field; }
    check_counts(0, 0);

    for (int cycle = 0; cycle < 20; ++cycle) {
        {
            Window window;
            Popup popup;
            BoxedWindow box;
            BoxedPopup boxed_popup;
            Field field;
            for (int resize = 0; resize < 10; ++resize) {
                window.init(3, 12, 2, 2);
                popup.init(3, 12, 6, 2);
                box.init(5, 20, 10, 2, 1);
                boxed_popup.init(5, 20, 16, 2, 1);
                field.create("Field", 3, 12, 22, 2, STRING, 8);
                popup.show(); popup.hide();
                boxed_popup.show(); boxed_popup.hide();
                field.show(); field.hide();
                check_counts(8, 5);
            }
        }
        check_counts(0, 0);
        {
            // Deleting through the base must release derived panels and borders.
            auto popup = std::make_unique<BoxedPopup>();
            popup->init(5, 20, 2, 2, 1);
            std::unique_ptr<Window> base = std::move(popup);
            check_counts(2, 2);
        }
        check_counts(0, 0);
        { Form form; }
        check_counts(0, 0);
    }

    endwin();
    delscreen(terminal);
    std::fclose(input);
    std::fclose(output);
    std::puts("PASS: repeated initialization, derived destruction, forms, and panel-before-window cleanup");
}
