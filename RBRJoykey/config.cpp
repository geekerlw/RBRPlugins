#include "config.h"

namespace Config {

void Setting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;

    bool bValue = false;
    std::string strValue = "";

    set_m_pluginOn(m_ini->Get("setting", "PluginOn", bValue));
    
    set_m_keyEsc(m_ini->Get("keymap", "Esc", strValue));
    set_m_keyEnter(m_ini->Get("keymap", "Enter", strValue));
    set_m_keyUp(m_ini->Get("keymap", "Up", strValue));
    set_m_keyDown(m_ini->Get("keymap", "Down", strValue));
    set_m_keyLeft(m_ini->Get("keymap", "Left", strValue));
    set_m_keyRight(m_ini->Get("keymap", "Right", strValue));
}

}