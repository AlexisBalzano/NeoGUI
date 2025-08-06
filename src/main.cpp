#include "NeoGUI.h"

extern "C" PLUGIN_API PluginSDK::BasePlugin *CreatePluginInstance()
{
    try
    {
        return new neogui::NeoGUI();
    }
    catch (const std::exception &e)
    {
        return nullptr;
    }
}