#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace neogui {

class GuiWindow {
public:
    GuiWindow(unsigned int width, unsigned int height, const std::string& title, const std::wstring& parentTitle);

    void setVisible(bool visible);

    bool isVisible() const { return show_; }
    bool isOpen() const { return window_.isOpen(); }
    void close() { window_.close(); }

	std::string getTitle() const { return title_; }

    void processEvents();
    void updateDrag();
    void render();

private:
    sf::RenderWindow window_;
    sf::CircleShape shape_;
    std::string title_;
    unsigned int width_, height_;
    bool show_;

#ifdef _WIN32
    HWND hwnd_;
    bool dragging_;
    POINT clickOffset_{};
#endif
};
} // namespace neogui