#include "config.h"

namespace Config {

void Setting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;

    bool bValue = false;

    set_m_pluginOn(m_ini->Get("setting", "PluginOn", bValue));
    set_m_showIn2D(m_ini->Get("setting", "ShowIn2D", bValue));
    set_m_showInVr(m_ini->Get("setting", "ShowInVr", bValue));
}

void Setting::IsCarConfigExist(const int carId) {
    return m_ini ? m_ini->IsSectionExist("car" + std::to_string(carId)) : false;
}

std::string Setting::GetCarConfigFolder(const int carId) {
    std::string sValue = "";
    return m_ini->Get("car" + std::to_string(carId), "TextureFolder", sValue);
}

void CarSetting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;
    
    bool bValue = false;
    float fValue = 0.0f;
    POINT pointValue = { 0, 0 };
    RECT rectValue = { 0, 0, 0, 0 };
    int fontValue = 10;
    D3DCOLOR colorValue = 0xFFFFFFFF;

    // display
    set_m_hudPos(m_ini->Get("display", "HudPos", pointValue));
    set_m_hudSize(m_ini->Get("display", "HudSize", pointValue));

    // vr position
    set_m_vrPositionX(m_ini->Get("vr", "PositionX", fValue));
    set_m_vrPositionY(m_ini->Get("vr", "PositionY", fValue));
    set_m_vrPositionZ(m_ini->Get("vr", "PositionZ", fValue));
    set_m_vrRotationX(m_ini->Get("vr", "RotationX", fValue));
    set_m_vrRotationY(m_ini->Get("vr", "RotationY", fValue));
    set_m_vrRotationZ(m_ini->Get("vr", "RotationZ", fValue));
    set_m_vrScale(m_ini->Get("vr", "Scale", fValue));

    // background
    set_m_backgroudShow(m_ini->Get("texture", "BackgroundShow", bValue));
    set_m_backgroundSrc(m_ini->Get("texture", "BackgroundSrcPos", rectValue));
    set_m_backgroundDst(m_ini->Get("texture", "BackgroundDstPos", rectValue));

    // gear
    set_m_gearShow(m_ini->Get("texture", "GearShow", bValue));
    for (int i = 0; i < MAX_GEAR_NUM; i++) {
        *(m_gearSrc + i) = m_ini->Get("texture", "GearSrcPos" + std::to_string(i), rectValue);
        *(m_gearDst + i) = m_ini->Get("texture", "GearDstPos" + std::to_string(i), rectValue);
    }

    // recmeter
    set_m_rpmShow(m_ini->Get("texture", "RpmShow", bValue));
    for (int i = 0; i < MAX_RPM_NUM; i++) {
        *(m_rpmSrc + i) = m_ini->Get("texture", "RpmSrcPos" + std::to_string(i), rectValue);
        *(m_rpmDst + i) = m_ini->Get("texture", "RpmDstPos" + std::to_string(i), rectValue);
    }

    // time
    set_m_timeShow(m_ini->Get("texture", "TimeShow", bValue));
    set_m_timeTextFontSize(m_ini->Get("texture", "TimeTextFontSize", fontValue));
    set_m_timeTextFontColor(m_ini->Get("texture", "TimeTextFontColor", colorValue));
    set_m_timeTextPos(m_ini->Get("texture", "TimeTextpos", pointValue));

    // speed meter
    set_m_speedShow(m_ini->Get("texture", "SpeedShow", bValue));
    set_m_speedTextFontSize(m_ini->Get("texture", "SpeedTextFontSize", fontValue));
    set_m_speedTextFontColor(m_ini->Get("texture", "SpeedTextFontColor", colorValue));
    set_m_speedTextPos(m_ini->Get("texture", "SpeedTextPos", pointValue));

    // distance
    set_m_distanceShow(m_ini->Get("texture", "DistanceShow", bValue));
    set_m_distanceTextFontSize(m_ini->Get("texture", "DistanceTextFontSize", fontValue));
    set_m_distanceTextFontColor(m_ini->Get("texture", "DistanceTextFontColor", colorValue));
    set_m_distancePos0(m_ini->Get("texture", "DistancePos0", pointValue));
    set_m_distancePos1(m_ini->Get("texture", "DistancePos1", pointValue));

    // engine temp
    set_m_engineTempShow(m_ini->Get("texture", "EngineTempShow", bValue));
    set_m_engineTempTextFontSize(m_ini->Get("texture", "EngineTempTextFontSize", fontValue));
    set_m_engineTempTextFontColor(m_ini->Get("texture", "EngineTempTextFontColor", colorValue));
    set_m_engineTempPos(m_ini->Get("texture", "EngineTempPos", pointValue));
}

}