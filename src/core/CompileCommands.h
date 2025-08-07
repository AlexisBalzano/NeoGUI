#include <algorithm>
#include <string>

#include "NeoGUI.h"


namespace neogui {
void NeoGUI::RegisterCommand() {
    try
    {
        CommandProvider_ = std::make_shared<NeoGUICommandProvider>(this, logger_, chatAPI_, fsdAPI_);

        PluginSDK::Chat::CommandDefinition definition;
        definition.name = "gui version";
        definition.description = "return NeoGUI version";
        definition.lastParameterHasSpaces = false;
		definition.parameters.clear();

        versionCommandId_ = chatAPI_->registerCommand(definition.name, definition, CommandProvider_);
        
        definition.name = "gui show";
        definition.description = "toggle show/hide GUI window";
        definition.lastParameterHasSpaces = false;
        definition.parameters.clear();

        showCommandId_ = chatAPI_->registerCommand(definition.name, definition, CommandProvider_);

        definition.name = "gui add";
        definition.description = "add new GUI window";
        definition.lastParameterHasSpaces = false;
        definition.parameters.clear();
		definition.parameters.push_back({ "title", PluginSDK::Chat::ParameterType::String, true, true, 0, 0, 5 });

        addCommandId_ = chatAPI_->registerCommand(definition.name, definition, CommandProvider_);


        definition.name = "gui remove";
        definition.description = "remove GUI window";

        removeCommandId_ = chatAPI_->registerCommand(definition.name, definition, CommandProvider_);
    }
    catch (const std::exception& ex)
    {
        logger_->error("Error registering command: " + std::string(ex.what()));
    }
}

inline void NeoGUI::unegisterCommand()
{
    if (CommandProvider_)
    {
        chatAPI_->unregisterCommand(versionCommandId_);
		chatAPI_->unregisterCommand(showCommandId_);
		chatAPI_->unregisterCommand(addCommandId_);
        chatAPI_->unregisterCommand(removeCommandId_);

        CommandProvider_.reset();
	}
}

Chat::CommandResult NeoGUICommandProvider::Execute( const std::string &commandId, const std::vector<std::string> &args)
{
    if (commandId == neoGUI_->versionCommandId_)
    {
        std::string message = "Version " + std::string(PLUGIN_VERSION);
        neoGUI_->DisplayMessage(message);
        return { true, std::nullopt };
	}
    else if (commandId == neoGUI_->showCommandId_)
    {
        bool showing = neoGUI_->toggleShowWindow();
		std::string message = showing ? "GUI window is now visible." : "GUI window is now hidden.";
        neoGUI_->DisplayMessage(message);
        return { true, std::nullopt };
    }
    else if (commandId == neoGUI_->addCommandId_)
    {
		std::string message = "Adding " + args[0] + " GUI window.";
        neoGUI_->DisplayMessage(message);
		neoGUI_->requestNewWindow(args[0]);
        return { true, std::nullopt };
    }
    else if (commandId == neoGUI_->removeCommandId_)
    {
        bool success = neoGUI_->removeWindow(args[0]);
        if (!success) {
            std::string error = "Failed to remove GUI window: " + args[0];
            neoGUI_->DisplayMessage(error);
            return { true, std::nullopt };
		}
        std::string message = "Removed " + args[0] + " GUI window.";
        neoGUI_->DisplayMessage(message);
        return { true, std::nullopt };
    }
    else {
        std::string error = "Invalid command. Use .gui <command> <param>";
        neoGUI_->DisplayMessage(error);
        return { false, error };
    }

	return { true, std::nullopt };
}
}  // namespace neogui