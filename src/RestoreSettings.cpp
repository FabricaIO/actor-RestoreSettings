#include"RestoreSettings.h"

/// @brief Creates a restore settings actor
/// @param Name The device name
/// @param configFile Name of the config file to use
RestoreSettings::RestoreSettings(String Name, String configFile) : Actor(Name) {
	config_path = "/settings/act/" + configFile;
	settings_file = String("/settings/act/") + "settings" + configFile;
}

/// @brief Starts a sensor trigger 
/// @return True on success
bool RestoreSettings::begin() {
	Description.type = "settings";
	Description.actions = {{"save", 0}, {"restore", 1}};
	bool result = false;
	// Check if file exists
	if (!checkConfig(config_path)) {
		// Set defaults
		result = setConfig(getConfig(), true);
		if (result) {
			result = saveSettings();
		}
	} else {
		// Load settings
		result = setConfig(Storage::readFile(config_path), false);
		if (result) {
			// Check if needs to auto restore settings
			if (settings_restore_config.autoRestore) {
				result = restoreSettings();
			}
		}
	}
	return result;
}

/// @brief Receives an action
/// @param action The action to process
/// @param payload Payload to be passed to triggered action
/// @return JSON response
std::pair<bool, String> RestoreSettings::receiveAction(const int action, const String& payload) {
	if (action == 0) {		
		if (saveSettings()) {
			return { true, R"({"success": true})" }; 
		} else {
			return { true, R"({"success": false, "Response": "Error saving settings"})" }; 
		}
	} else if (action == 1) {
		if (restoreSettings()) {
			return { true, R"({"success": true})" }; 
		} else {
			return { true, R"({"success": false, "Response": "Error loading settings"})" }; 
		}
	}
	return { true, R"({"success": false, "Response": "Bad action"})" }; 
}

/// @brief Gets the current config
/// @return A JSON string of the config
String RestoreSettings::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;

	doc["autoRestore"] = settings_restore_config.autoRestore;

	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool RestoreSettings::setConfig(String config, bool save) {
		JsonDocument doc;
		// Deserialize file contents
		DeserializationError error = deserializeJson(doc, config);
		// Test if parsing succeeds.
		if (error) {
			Logger.print(F("Deserialization failed: "));
			Logger.println(error.f_str());
			return false;
		}

		settings_restore_config.autoRestore = doc["autoRestore"].as<bool>();

		if (save) {
			return saveConfig(config_path, config);
		}
		return true;
}

/// @brief Saves the current hub settings to a file
/// @return True on success
bool RestoreSettings::saveSettings() {
	Logger.println("Saving current settings...");
	return Storage::writeFile(settings_file, Configuration::getConfig());
}

/// @brief Restores the saved configuration
/// @return True on success
bool RestoreSettings::restoreSettings() {
	Logger.println("Restoring saved settings...");
	if (Storage::fileExists(settings_file)) {
		if (Configuration::saveConfig(Storage::readFile(settings_file))) {
			return Configuration::loadConfig();
		}
		return false;
	} else {
		Logger.println("No saved settings file");
		return false;
	}
}