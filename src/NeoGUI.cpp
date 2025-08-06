#include "NeoGUI.h"
#include <numeric>
#include <chrono>

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
    int counter = 1;
    while (true) {
        counter += 1;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (true == this->m_stop) return;
        
        //this->OnTimer(counter);
    }
    return;
}

PluginSDK::PluginMetadata NeoGUI::GetMetadata() const
{
    return {"NeoGUI", PLUGIN_VERSION, "French VACC"};
}