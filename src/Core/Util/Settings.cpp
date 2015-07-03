#include "Settings.hpp"

using namespace std;

Settings::Settings() {
    ini.SetUnicode(true);
}

bool Settings::GetBool(string& settingName) {
    BoolSetting setting = boolSettings[settingName];
    
    return ini.GetBoolValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue);
}

void Settings::SetBool(string& settingName, bool value) {
    BoolSetting setting = boolSettings[settingName];
    
    ini.SetBoolValue(setting.section.c_str(), setting.key.c_str(), value);
}

long Settings::GetLong(string& settingName) {
    LongSetting setting = longSettings[settingName];
    
    return ini.GetLongValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue);
}

void Settings::SetLong(string& settingName, long value) {
    LongSetting setting = longSettings[settingName];
    
    ini.SetLongValue(setting.section.c_str(), setting.key.c_str(), value);
}

double Settings::GetDouble(string& settingName) {
    DoubleSetting setting = doubleSettings[settingName];
    
    return ini.GetDoubleValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue);
}

void Settings::SetDouble(string& settingName, double value) {
    DoubleSetting setting = doubleSettings[settingName];
    
    ini.SetDoubleValue(setting.section.c_str(), setting.key.c_str(), value);
}

string Settings::GetString(string& settingName) {
    StringSetting setting = stringSettings[settingName];
    
    return ini.GetValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue.c_str());
}

void Settings::SetString(string& settingName, string& value) {
    StringSetting setting = stringSettings[settingName];
    
    ini.SetValue(setting.section.c_str(), setting.key.c_str(), value.c_str());
}

void Settings::AddBoolSetting(string& settingName, string& section, string& key, bool defaultValue) {
    BoolSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    boolSettings[settingName] = setting;
}

void Settings::AddLongSetting(string& settingName, string& section, string& key, long defaultValue) {
    LongSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    longSettings[settingName] = setting;
}

void Settings::AddDoubleSetting(string& settingName, string& section, string& key, double defaultValue) {
    DoubleSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    doubleSettings[settingName] = setting;
}

void Settings::AddStringSetting(string& settingName, string& section, string& key, string& defaultValue) {
    StringSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    stringSettings[settingName] = setting;
}
