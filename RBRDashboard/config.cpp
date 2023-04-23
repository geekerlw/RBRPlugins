#include "config.h"

namespace Config {

void Setting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;

    bool bValue = false;
    set_m_showIn2D(m_ini->Get("setting", "ShowIn2D", bValue));
    set_m_showInVr(m_ini->Get("setting", "ShowInVr", bValue));
}

void CarSetting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;
    
    std::string section = "car" + std::to_string(m_carid);
    std::string sValue = "";
    bool bValue = false;
    float fValue = 0.0f;
    POINT pointValue = { 0, 0 };
    RECT rectValue = { 0, 0, 0, 0 };
    int fontValue = 10;
    D3DCOLOR colorValue = 0xFFFFFFFF;

    // setting
    set_m_textureFile(m_ini->Get(section, "TextureFile", sValue));
    set_m_hudPos(m_ini->Get(section, "HudPos", pointValue));
    set_m_hudSize(m_ini->Get(section, "HudSize", pointValue));
    set_m_scalex(m_ini->Get(section, "ScaleX", fValue));
    set_m_scaley(m_ini->Get(section, "ScaleY", fValue));

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
    set_m_timeTextFontSize(m_ini->Get(section, "TimeTextFontSize", fontValue));
    set_m_timeTextFontColor(m_ini->Get(section, "TimeTextFontColor", colorValue));
    set_m_timeTextPos(m_ini->Get(section, "TimeTextpos", pointValue));

    // speed meter
    set_m_speedShow(m_ini->Get(section, "SpeedShow", bValue));
    set_m_speedTextFontSize(m_ini->Get(section, "SpeedTextFontSize", fontValue));
    set_m_speedTextFontColor(m_ini->Get(section, "SpeedTextFontColor", colorValue));
    set_m_speedTextPos(m_ini->Get(section, "SpeedTextPos", pointValue));

    // distance
    set_m_distanceShow(m_ini->Get(section, "DistanceShow", bValue));
    set_m_distanceTextFontSize(m_ini->Get(section, "DistanceTextFontSize", fontValue));
    set_m_distanceTextFontColor(m_ini->Get(section, "DistanceTextFontColor", colorValue));
    set_m_distancePos0(m_ini->Get(section, "DistancePos0", pointValue));
    set_m_distancePos1(m_ini->Get(section, "DistancePos1", pointValue));

    // engine temp
    set_m_engineTempShow(m_ini->Get(section, "EngineTempShow", bValue));
    set_m_engineTempTextFontSize(m_ini->Get(section, "EngineTempTextFontSize", fontValue));
    set_m_engineTempTextFontColor(m_ini->Get(section, "EngineTempTextFontColor", colorValue));
    set_m_engineTempPos(m_ini->Get(section, "EngineTempPos", pointValue));
}

}