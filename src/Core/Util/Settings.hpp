#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <SimpleIni.h>
#include <string>
#include <map>

/** @ingroup Core
 * @{
 */

/// Handles settings.
class Settings {
    public:
        /// Get a bool setting.
        /**
         * @param settingName Setting name.
         * @return The value of the setting
         */
        bool GetBool(std::string& settingName);
        
        /// Set a bool setting.
        /**
         * @param settingName Setting name.
         * @param value Value to set it to.
         */
        void SetBool(std::string& settingName, bool value);
        
        /// Get a long setting.
        /**
         * @param settingName Setting name.
         * @return The value of the setting
         */
        long GetLong(std::string& settingName);
        
        /// Set a long setting.
        /**
         * @param settingName Setting name.
         * @param value Value to set it to.
         */
        void SetLong(std::string& settingName, long value);
        
        /// Get a double setting.
        /**
         * @param settingName Setting name.
         * @return The value of the setting
         */
        double GetDouble(std::string& settingName);
        
        /// Set a double setting.
        /**
         * @param settingName Setting name.
         * @param value Value to set it to.
         */
        void SetDouble(std::string& settingName, double value);
        
        /// Get a string setting.
        /**
         * @param settingName Setting name.
         * @return The value of the setting
         */
        std::string GetString(std::string& settingName);
        
        /// Set a string setting.
        /**
         * @param settingName Setting name.
         * @param value Value to set it to.
         */
        void SetString(std::string& settingName, std::string& value);
        
        /// Save the settings to an INI file.
        virtual void Save() = 0;
        
    protected:
        /// Ini file.
        CSimpleIni ini;
        
        /// Add a bool setting.
        /**
         * @param settingName Setting name.
         * @param section INI section.
         * @param key INI key.
         * @param defaultValue Default value.
         */
        void AddBoolSetting(std::string& settingName, std::string& section, std::string& key, bool defaultValue);
        
        /// Add a long setting.
        /**
         * @param settingName Setting name.
         * @param section INI section.
         * @param key INI key.
         * @param defaultValue Default value.
         */
        void AddLongSetting(std::string& settingName, std::string& section, std::string& key, long defaultValue);
        
        /// Add a double setting.
        /**
         * @param settingName Setting name.
         * @param section INI section.
         * @param key INI key.
         * @param defaultValue Default value.
         */
        void AddDoubleSetting(std::string& settingName, std::string& section, std::string& key, double defaultValue);
        
        /// Add a string setting.
        /**
         * @param settingName Setting name.
         * @param section INI section.
         * @param key INI key.
         * @param defaultValue Default value.
         */
        void AddStringSetting(std::string& settingName, std::string& section, std::string& key, std::string& defaultValue);
        
    private:
        struct BoolSetting {
            std::string section;
            std::string key;
            bool defaultValue;
        };
        
        struct LongSetting {
            std::string section;
            std::string key;
            long defaultValue;
        };
        
        struct DoubleSetting {
            std::string section;
            std::string key;
            double defaultValue;
        };
        
        struct StringSetting {
            std::string section;
            std::string key;
            std::string defaultValue;
        };
        
        std::map<std::string, BoolSetting> boolSettings;
        std::map<std::string, LongSetting> longSettings;
        std::map<std::string, DoubleSetting> doubleSettings;
        std::map<std::string, StringSetting> stringSettings;
};

/** @} */

#endif
