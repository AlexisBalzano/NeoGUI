#include "NeoGUI.h"
#include <numeric>
#include <chrono>
#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Version.h"
#include "core/CompileCommands.h"


using namespace neogui;

NeoGUI::NeoGUI() : m_stop(false), controllerDataAPI_(nullptr) {};
NeoGUI::~NeoGUI() = default;

void NeoGUI::Initialize(const PluginMetadata &metadata, CoreAPI *coreAPI, ClientInformation info)
{
    metadata_ = metadata;
    clientInfo_ = info;
    CoreAPI *lcoreAPI = coreAPI;
    aircraftAPI_ = &lcoreAPI->aircraft();
    airportAPI_ = &lcoreAPI->airport();
    chatAPI_ = &lcoreAPI->chat();
    flightplanAPI_ = &lcoreAPI->flightplan();
    fsdAPI_ = &lcoreAPI->fsd();
    controllerDataAPI_ = &lcoreAPI->controllerData();
    logger_ = &lcoreAPI->logger();
    tagInterface_ = lcoreAPI->tag().getInterface();

    DisplayMessage("Version " + std::string(PLUGIN_VERSION) + " loaded.", "Initialisation");

    try
    {
        this->RegisterCommand();

        initialized_ = true;
    }
    catch (const std::exception &e)
    {
        logger_->error("Failed to initialize NeoGUI: " + std::string(e.what()));
    }

    this->m_stop = false;
    this->m_worker = std::thread(&NeoGUI::run, this);
}

void NeoGUI::Shutdown()
{
    if (initialized_)
    {
        initialized_ = false;
        logger_->info("NeoGUI shutdown complete");
    }

    this->m_stop = true;
    this->m_worker.join();
    this->unegisterCommand();
}

void NeoGUI::DisplayMessage(const std::string &message, const std::string &sender) {
    Chat::ClientTextMessageEvent textMessage;
    textMessage.sentFrom = "NeoGUI";
    (sender.empty()) ? textMessage.message = ": " + message : textMessage.message = sender + ": " + message;
    textMessage.useDedicatedChannel = true;

    chatAPI_->sendClientMessage(textMessage);
}

void neogui::NeoGUI::OnTimer(int Counter)
{
}

void neogui::NeoGUI::TagProcessing(const std::string& callsign, const std::string& actionId, const std::string& userInput)
{
}

void neogui::NeoGUI::runScopeUpdate()
{
}

void NeoGUI::run() {
	unsigned int windowWidth = 200;
	unsigned int windowHeight = 200;

    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "NeoGUI Default Window", sf::Style::None);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle &= ~WS_EX_APPWINDOW;
    exStyle |= WS_EX_TOOLWINDOW;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
    
    bool dragging = false;
    POINT clickOffset = {0, 0};

    HWND mainHwnd = FindWindowW(nullptr, L"NeoRadar");
    SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, (LONG_PTR)mainHwnd);

    while (window.isOpen()) {
        if (m_stop) {
            window.close();
            return;
        }

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    dragging = true;
                    // Get the mouse position relative to the screen
                    POINT mousePos;
                    GetCursorPos(&mousePos);
                    // Get the window position
                    RECT winRect;
                    GetWindowRect(hwnd, &winRect);
                    // Store the offset between mouse and window origin
                    clickOffset.x = mousePos.x - winRect.left;
                    clickOffset.y = mousePos.y - winRect.top;
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                auto mouseEvent = event->getIf<sf::Event::MouseButtonReleased>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    dragging = false;
                }
            }
        }

        // If dragging, move the window to follow the mouse
        if (dragging) {
            POINT mousePos;
            auto xValue = mousePos.x - clickOffset.x;
			auto yValue = mousePos.y - clickOffset.y;
            
			if (xValue < 0) xValue = 0;
			if (yValue < 0) yValue = 0;
            if (xValue > GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(windowWidth)) xValue = GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(windowWidth);
            if (yValue > GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(windowHeight)) yValue = GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(windowHeight);

            GetCursorPos(&mousePos);
            SetWindowPos(hwnd, nullptr,
                xValue,
                yValue,
                0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
#endif
}

PluginSDK::PluginMetadata NeoGUI::GetMetadata() const
{
    return {"NeoGUI", PLUGIN_VERSION, "French VACC"};
}
