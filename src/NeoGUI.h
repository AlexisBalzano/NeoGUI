#pragma once
#include <memory>
#include <thread>
#include <vector>

#include "SDK.h"
#include "core/NeoGUICommandProvider.h"

using namespace PluginSDK;

namespace neogui {

    class NeoGUI : public BasePlugin
    {
    public:
        NeoGUI();
        ~NeoGUI();

		// Plugin lifecycle methods
        void Initialize(const PluginMetadata& metadata, CoreAPI* coreAPI, ClientInformation info) override;
        void Shutdown() override;
        PluginMetadata GetMetadata() const override;

        // Radar commands
        void DisplayMessage(const std::string& message, const std::string& sender = "");
		
        // Scope Events
        void OnTimer(int Counter);

        // Command handling
        void TagProcessing(const std::string& callsign, const std::string& actionId, const std::string& userInput = "");
		bool toggleShowWindow();

		// API Accessors
        PluginSDK::Logger::LoggerAPI* GetLogger() const { return logger_; }
        Aircraft::AircraftAPI* GetAircraftAPI() const { return aircraftAPI_; }
        Airport::AirportAPI* GetAirportAPI() const { return airportAPI_; }
        Chat::ChatAPI* GetChatAPI() const { return chatAPI_; }
        Flightplan::FlightplanAPI* GetFlightplanAPI() const { return flightplanAPI_; }
        Fsd::FsdAPI* GetFsdAPI() const { return fsdAPI_; }
        PluginSDK::ControllerData::ControllerDataAPI* GetControllerDataAPI() const { return controllerDataAPI_; }
		Tag::TagInterface* GetTagInterface() const { return tagInterface_; }

    private:
        void runScopeUpdate();
        void run();

    public:
        // Command IDs
        std::string versionCommandId_;
        std::string showCommandId_;
        
    private:
        // Plugin state
        bool initialized_ = false;
        std::thread m_worker;
        bool m_stop;
		bool showWindow_ = true;

		unsigned int windowWidth = 200;
		unsigned int windowHeight = 200;

        // APIs
        PluginMetadata metadata_;
        ClientInformation clientInfo_;
        Aircraft::AircraftAPI* aircraftAPI_ = nullptr;
        Airport::AirportAPI* airportAPI_ = nullptr;
        Chat::ChatAPI* chatAPI_ = nullptr;
        Flightplan::FlightplanAPI* flightplanAPI_ = nullptr;
        Fsd::FsdAPI* fsdAPI_ = nullptr;
        PluginSDK::Logger::LoggerAPI* logger_ = nullptr;
        PluginSDK::ControllerData::ControllerDataAPI* controllerDataAPI_ = nullptr;
        Tag::TagInterface* tagInterface_ = nullptr;
        std::shared_ptr<NeoGUICommandProvider> CommandProvider_;

        // Tag Items
        void RegisterCommand();
        void unegisterCommand();
        
	    // TAG Items IDs
		std::string tagItemId_;
        
        // TAG Action IDs
        std::string tagActionId_;
    };
} // namespace neogui