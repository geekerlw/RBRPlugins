#include "config.h"

namespace Config {

void Setting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;

    bool bValue = false;
    int iValue = 1;
    set_m_pluginOn(m_ini->Get("setting", "PluginOn", bValue));
}

}