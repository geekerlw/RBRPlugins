#pragma once

#include <Windows.h>
#include "LogUtil.h"
#include "..\Lib\SimpleINI\SimpleIni.h"
#include "..\RBR\D3D9Helpers.h"

namespace INIUtil {
  using std::string;

  class INIManager {
  public:
    INIManager(std::string filePath);
    ~INIManager();

    string Get(const string& section, const string& name, const string& defaultValue);
    int Get(const string& section, const string& name, int defaultValue);
    float Get(const string& section, const string& name, float defaultValue);
    bool Get(const string& section, const string& name, bool defaultValue);
    RECT Get(const string& section, const string& name, RECT& defaultValue);
    POINT Get(const string& section, const string& name, POINT& defaultValue);
    D3DCOLOR Get(const string& section, const string& name, D3DCOLOR& defaultValue);

    void Save(bool forceSave = false);
    bool IsSectionExist(const string& section);

  private:
    bool defaultOptionsSet;
    string filePath;
    CSimpleIniA ini;
  };
}
