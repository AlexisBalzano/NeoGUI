#include "guiWindow.h"
#include "../config/Colors.h"

#if defined(_WIN32)
#include <Windows.h>
#include <shlobj.h>
#include <knownfolders.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#include <cstdlib>
#endif


using namespace neogui;

std::filesystem::path GuiWindow::getNeoRadarDirectory() const
{
#if defined(_WIN32)
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path);
    std::filesystem::path documentsPath;
    if (SUCCEEDED(hr)) {
        documentsPath = path;
        CoTaskMemFree(path);
    }
    return documentsPath / "NeoRadar";
#elif defined(__APPLE__) || defined(__linux__)
    const char* homeDir = std::getenv("HOME");
    if (homeDir) {
        return std::filesystem::path(homeDir) / "Documents" / "NeoRadar";
    }
    return std::filesystem::path(); // Return empty path if HOME is not set
#else
    return std::filesystem::path(); // Return an empty path for unsupported platforms
#endif
}

GuiWindow::GuiWindow(unsigned int width, unsigned int height, const std::string& title, const std::wstring& parentTitle, PluginSDK::Logger::LoggerAPI* loggerAPI)
	: window_(sf::VideoMode({ width, height }), title, sf::Style::None), title_(title), width_(width), height_(height), show_(true), dragging_(false), loggerAPI_(loggerAPI)
{
    // Load NeoRadar font
	std::filesystem::path fontPath = getNeoRadarDirectory() / "fonts" / "RobotoMono-VariableFont_wght.ttf";
    if (!font_.openFromFile(fontPath)) {
		loggerAPI_->log(PluginSDK::Logger::LogLevel::Error, "Failed to load font from: " + fontPath.string());
    }
	addDefaultGraphics();

#ifdef _WIN32
    hwnd_ = window_.getNativeHandle();
    LONG exStyle = GetWindowLong(hwnd_, GWL_EXSTYLE);
    exStyle &= ~WS_EX_APPWINDOW;
    exStyle |= WS_EX_TOOLWINDOW;
    SetWindowLong(hwnd_, GWL_EXSTYLE, exStyle);

    HWND mainHwnd = FindWindowW(nullptr, parentTitle.c_str());
    SetWindowLongPtr(hwnd_, GWLP_HWNDPARENT, (LONG_PTR)mainHwnd);
#endif
}

neogui::GuiWindow::~GuiWindow()
{
    // Ensure the window is closed when the object is destroyed
    if (window_.isOpen()) {
        window_.close();
	}
	drawables_.clear();
}

void GuiWindow::setVisible(bool visible)
{
    show_ = visible;
    window_.setVisible(visible);
}

void neogui::GuiWindow::addDefaultGraphics()
{
    // Add default graphics to the window, rendered in the order they are added (last rendered is above all)
    auto shape = std::make_unique<sf::RectangleShape>(sf::Vector2f{ static_cast<float>(width_), 25 });
    shape->setFillColor(Colors::DarkGrey);
    drawables_.push_back(std::move(shape));

    shape = std::make_unique<sf::RectangleShape>(sf::Vector2f{ static_cast<float>(width_), 170 });
    shape->setFillColor(Colors::LightkGrey);
    shape->setPosition({ 0, 30 });
    drawables_.push_back(std::move(shape));

    auto text = std::make_unique<sf::Text>(font_, title_, 20);
    text->setFillColor(sf::Color::White);
    sf::FloatRect textRect = text->getLocalBounds();
    float x = (static_cast<float>(width_) - textRect.size.x) / 2.f;
    text->setPosition({ x, 0 });
	text->setStyle(sf::Text::Bold);
	drawables_.push_back(std::move(text));
}

void GuiWindow::processEvents()
{
    while (const std::optional event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window_.close();

#ifdef _WIN32
        if (event->is<sf::Event::MouseButtonPressed>()) {
            auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                dragging_ = true;
                POINT mousePos;
                GetCursorPos(&mousePos);
                RECT winRect;
                GetWindowRect(hwnd_, &winRect);
                clickOffset_.x = mousePos.x - winRect.left;
                clickOffset_.y = mousePos.y - winRect.top;
            }
        }
        if (event->is<sf::Event::MouseButtonReleased>()) {
            auto mouseEvent = event->getIf<sf::Event::MouseButtonReleased>();
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                dragging_ = false;
            }
        }
#endif
    }
}

void GuiWindow::updateDrag()
{
#ifdef _WIN32
    if (dragging_) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        int xValue = mousePos.x - clickOffset_.x;
        int yValue = mousePos.y - clickOffset_.y;
        if (xValue < 0) xValue = 0;
        if (yValue < 0) yValue = 0;
        if (xValue > GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(width_)) xValue = GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(width_);
        if (yValue > GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(height_)) yValue = GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(height_);
        SetWindowPos(hwnd_, nullptr, xValue, yValue, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
#endif
}

void GuiWindow::render()
{
    if (show_) {
        window_.clear();
        for (const auto& drawable : drawables_) {
            window_.draw(*drawable);
		}
        window_.display();
    }
}
