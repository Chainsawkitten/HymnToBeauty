#include "Settings.hpp"

using namespace std;

bool Settings::GetBool(const string& settingName) {
    BoolSetting setting = boolSettings[settingName];
    
    return ini.GetBoolValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue);
}

void Settings::SetBool(const string& settingName, bool value) {
    BoolSetting setting = boolSettings[settingName];
    
    ini.SetBoolValue(setting.section.c_str(), setting.key.c_str(), value);
}

long Settings::GetLong(const string& settingName) {
    LongSetting setting = longSettings[settingName];
    
    return ini.GetLongValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue);
}

void Settings::SetLong(const string& settingName, long value) {
    LongSetting setting = longSettings[settingName];
    
    ini.SetLongValue(setting.section.c_str(), setting.key.c_str(), value);
}

double Settings::GetDouble(const string& settingName) {
    DoubleSetting setting = doubleSettings[settingName];
    
    return ini.GetDoubleValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue);
}

void Settings::SetDouble(const string& settingName, double value) {
    DoubleSetting setting = doubleSettings[settingName];
    
    ini.SetDoubleValue(setting.section.c_str(), setting.key.c_str(), value);
}

string Settings::GetString(const string& settingName) {
    StringSetting setting = stringSettings[settingName];
    
    return ini.GetValue(setting.section.c_str(), setting.key.c_str(), setting.defaultValue.c_str());
}

void Settings::SetString(const string& settingName, const string& value) {
    StringSetting setting = stringSettings[settingName];
    
    ini.SetValue(setting.section.c_str(), setting.key.c_str(), value.c_str());
}

void Settings::AddBoolSetting(const string& settingName, const string& section, const string& key, bool defaultValue) {
    BoolSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    boolSettings[settingName] = setting;
    
    bool value = ini.GetBoolValue(section.c_str(), key.c_str(), defaultValue);
    ini.SetBoolValue(section.c_str(), key.c_str(), value);
}

void Settings::AddLongSetting(const string& settingName, const string& section, const string& key, long defaultValue) {
    LongSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    longSettings[settingName] = setting;
    
    long value = ini.GetLongValue(section.c_str(), key.c_str(), defaultValue);
    ini.SetLongValue(section.c_str(), key.c_str(), value);
}

void Settings::AddDoubleSetting(const string& settingName, const string& section, const string& key, double defaultValue) {
    DoubleSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    doubleSettings[settingName] = setting;
    
    double value = ini.GetDoubleValue(section.c_str(), key.c_str(), defaultValue);
    ini.SetDoubleValue(section.c_str(), key.c_str(), value);
}

void Settings::AddStringSetting(const string& settingName, const string& section, const string& key, const string& defaultValue) {
    StringSetting setting;
    setting.section = section;
    setting.key = key;
    setting.defaultValue = defaultValue;
    
    stringSettings[settingName] = setting;
    
    const char* value = ini.GetValue(section.c_str(), key.c_str(), defaultValue.c_str());
    ini.SetValue(section.c_str(), key.c_str(), value);
}
