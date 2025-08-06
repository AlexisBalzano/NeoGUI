#pragma once
#include "SDK.h"
#include "NeoGUI.h"

using namespace PluginSDK;

namespace neogui {

class NeoGUI;

class NeoGUICommandProvider : public PluginSDK::Chat::CommandProvider
{
public:
    NeoGUICommandProvider(neogui::NeoGUI *neoGUI, PluginSDK::Logger::LoggerAPI *logger, Chat::ChatAPI *chatAPI, Fsd::FsdAPI *fsdAPI)
            : neoGUI_(neoGUI), logger_(logger), chatAPI_(chatAPI), fsdAPI_(fsdAPI) {}
		
	Chat::CommandResult Execute(const std::string &commandId, const std::vector<std::string> &args) override;

private:
    Logger::LoggerAPI *logger_ = nullptr;
    Chat::ChatAPI *chatAPI_ = nullptr;
    Fsd::FsdAPI *fsdAPI_ = nullptr;
    NeoGUI *neoGUI_ = nullptr;
};
}  // namespace neogui