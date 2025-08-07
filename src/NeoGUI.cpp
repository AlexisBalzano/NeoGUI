#include <numeric>
#include <algorithm>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "NeoGUI.h"

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

bool neogui::NeoGUI::toggleShowWindow()
{
    showWindow_ = !showWindow_;
	return showWindow_;
}

void neogui::NeoGUI::requestNewWindow(const std::string& title)
{
    newWindowRequested_ = true;
    newWindowName_ = title;
	std::transform(newWindowName_.begin(), newWindowName_.end(), newWindowName_.begin(), ::toupper);
}

bool neogui::NeoGUI::removeWindow(const std::string& title)
{
    if (title.empty()) {
        logger_->warning("Attempted to remove a window with an empty title.");
        return false;
	}
    removeWindowRequested_ = true;
	removeWindowName_ = title;
	std::transform(removeWindowName_.begin(), removeWindowName_.end(), removeWindowName_.begin(), ::toupper);
    auto it = std::find_if(windows_.begin(), windows_.end(),
        [&title](const std::shared_ptr<GuiWindow>& win) { return win->getTitle() == title; });
    if (it != windows_.end()) {
        return true;
    }
	return false;
}

void neogui::NeoGUI::addWindow(const std::string& title)
{
    windows_.emplace_back(std::make_shared<GuiWindow>(defaultWindowWidth, defaultWindowHeight, title, L"NeoRadar", this->GetLogger()));
}

void neogui::NeoGUI::runScopeUpdate()
{
}

void NeoGUI::run()
{
	addWindow("first window");

    while (true) {
        if (m_stop) {
            for (auto& win : windows_) win->close();
			windows_.clear();
            return;
        }

        if (removeWindowRequested_) {
            auto it = std::remove_if(windows_.begin(), windows_.end(),
                [this](const std::shared_ptr<GuiWindow>& win) { return win->getTitle() == removeWindowName_; });
            if (it != windows_.end()) {
                windows_.erase(it, windows_.end());
            }
            removeWindowRequested_ = false;
		}

        if (newWindowRequested_) {
            addWindow(newWindowName_);
            newWindowRequested_ = false;
		}

        int waitTime = showWindow_ ? 10 : 500;
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

        for (auto& win : windows_) {
            win->setVisible(showWindow_);
            if (!win->isVisible() || !win->isOpen()) {
                continue;
            }
            win->processEvents();
            win->updateDrag();
            win->render();
        }
    }
}

PluginSDK::PluginMetadata NeoGUI::GetMetadata() const
{
    return {"NeoGUI", PLUGIN_VERSION, "French VACC"};
}
