#include "config.h"

namespace Config {

void Setting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;

    bool bValue = false;
    set_m_showInVr(m_ini->Get("setting", "ShowInVr", bValue));
}

void CarSetting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;
    
    std::string section = "car" + std::to_string(m_carid);
    std::string sValue = "";
    bool bValue = false;
    float fValue = 0.0f;
    RECT rectValue = { 0, 0, 0, 0 };

    // setting
    set_m_textureFile(m_ini->Get(section, "textureFile", sValue));
    set_m_hubPos(m_ini->Get(section, "HubPos", rectValue));
    set_m_scalex(m_ini->Get(section, "ScaleX", fValue));
    set_m_scalex(m_ini->Get(section, "ScaleY", fValue));

    // background
    set_m_backgroudShow(m_ini->Get(section, "BackgroundShow", bValue));
    set_m_backgroundSrc(m_ini->Get(section, "BackgroundSrcPos", rectValue));
    set_m_backgroundDst(m_ini->Get(section, "BackgroundDstPos", rectValue));

    // gear
    set_m_gearShow(m_ini->Get(section, "GearShow", bValue));
    for (int i = 0; i < MAX_GEAR_NUM; i++) {
        *(m_gearSrc + i) = m_ini->Get(section, "GearSrcPos" + std::to_string(i), rectValue);
        *(m_gearDst + i) = m_ini->Get(section, "GearDstPos" + std::to_string(i), rectValue);
    }

    // recmeter
    set_m_rpmShow(m_ini->Get(section, "RpmShow", bValue));
    for (int i = 0; i < MAX_RPM_NUM; i++) {
        *(m_rpmSrc + i) = m_ini->Get(section, "RpmSrcPos" + std::to_string(i), rectValue);
        *(m_rpmDst + i) = m_ini->Get(section, "RpmDstPos" + std::to_string(i), rectValue);
    }

    // time
    set_m_timeShow(m_ini->Get(section, "TimeShow", bValue));
    set_m_timeTextArea(m_ini->Get(section, "TimeTextArea", rectValue));
    set_m_timeTextSplitWidth(m_ini->Get(section, "TimeTextSplitWidth", fValue));
    set_m_timeTextPos(m_ini->Get(section, "TimeTextpos", rectValue));

    // speed meter
    set_m_speedShow(m_ini->Get(section, "SpeedShow", bValue));
    set_m_speedTextArea(m_ini->Get(section, "SpeedTextArea", rectValue));
    set_m_speedTextSplitWidth(m_ini->Get(section, "SpeedTextSplitWidth", fValue));
    set_m_speedTextPos(m_ini->Get(section, "SpeedTextPos", rectValue));

    // distance
    set_m_distanceShow(m_ini->Get(section, "DistanceShow", bValue));
    set_m_distanceTextArea(m_ini->Get(section, "DistanceTextArea", rectValue));
    set_m_distanceTextSplitWidth(m_ini->Get(section, "DistanceTextSplitWidth", fValue));
    set_m_distancePos0(m_ini->Get(section, "DistancePos0", rectValue));
    set_m_distancePos1(m_ini->Get(section, "DistancePos1", rectValue));

    // engine temp
    set_m_engineTempShow(m_ini->Get(section, "EngineTempShow", bValue));
    set_m_engineTempTextArea(m_ini->Get(section, "EngineTempTextArea", rectValue));
    set_m_engineTempTextSplitWidth(m_ini->Get(section, "EngineTempTextSplitWidth", fValue));
    set_m_engineTempPos(m_ini->Get(section, "EngineTempPos", rectValue));

    // warning
    set_m_warningIconShow(m_ini->Get(section, "WarningIconShow", bValue));
    set_m_warningIconArea(m_ini->Get(section, "WarningIconArea", rectValue));
    set_m_warningIconSplitWidth(m_ini->Get(section, "WarningIconSplitWidth", fValue));
    set_m_warningIconPos(m_ini->Get(section, "WarningIconPos", rectValue));
}

}