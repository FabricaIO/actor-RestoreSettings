/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2025 Sam Groveman
* 
* External libraries needed:
* ArduinoJSON: https://arduinojson.org/
*
* Contributors: Sam Groveman
*/
#pragma once
#include <Arduino.h>
#include <Actor.h>
#include <ArduinoJson.h>
#include <Configuration.h>

/// @brief Class providing the function to save and restore hub settings
class RestoreSettings : public Actor {
	protected:
		/// @brief Restorer configuration
		struct {
			/// @brief Auto restore hub settings on boot
			bool autoRestore = false;	
		} settings_restore_config;

		/// @brief Path to configuration file
		String config_path;

		/// @brief Path to file storing the hub settings
		String settings_file;

		bool saveSettings();
		bool restoreSettings();
	public:
		RestoreSettings(String Name, String configFile = "RestoreSettingsConfig.json");
		std::pair<bool, String> receiveAction(const int action, const String& payload = "");
		bool begin();
		String getConfig();
		bool setConfig(String config, bool save);
};
