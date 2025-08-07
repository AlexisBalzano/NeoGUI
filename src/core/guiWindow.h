#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

#include "SDK.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace neogui {

class GuiWindow {
public:
    GuiWindow(unsigned int width, unsigned int height, const std::string& title, const std::wstring& parentTitle, PluginSDK::Logger::LoggerAPI* loggerAPI);
	~GuiWindow();

    void setVisible(bool visible);

    bool isVisible() const { return show_; }
    bool isOpen() const { return window_.isOpen(); }
    void close() { window_.close(); }

	std::string getTitle() const { return title_; }
	std::filesystem::path getNeoRadarDirectory() const;

    void addDefaultGraphics();
    void processEvents();
    void updateDrag();
    void render();

private:
    // API
    PluginSDK::Logger::LoggerAPI* loggerAPI_;

	// SFML
    sf::RenderWindow window_;
    std::vector <std::unique_ptr<sf::Drawable>> drawables_;
    std::string title_;
	sf::Font font_;
    unsigned int width_, height_;
    bool show_;

#ifdef _WIN32
    HWND hwnd_;
    bool dragging_;
    POINT clickOffset_{};
#endif
};
} // namespace neogui