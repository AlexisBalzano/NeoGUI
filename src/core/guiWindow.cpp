#include "guiWindow.h"

using namespace neogui;

GuiWindow::GuiWindow(unsigned int width, unsigned int height, const std::string& title, const std::wstring& parentTitle)
    : window_(sf::VideoMode({ width, height }), title, sf::Style::None), width_(width), height_(height), show_(true), dragging_(false)
{
    shape_.setRadius(static_cast<float>(std::min(width, height)) / 2.f);
    shape_.setFillColor(sf::Color::Green);

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

void GuiWindow::setVisible(bool visible)
{
    show_ = visible;
    window_.setVisible(visible);
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
        window_.draw(shape_);
        window_.display();
    }
}
