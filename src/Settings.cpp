#include "Settings.h"

#include <fstream>
#include <iomanip>

namespace Gotobed {
    namespace {
        std::filesystem::path GetSettingsPath() {
            std::vector<wchar_t> buf(4096);
            auto size = REX::W32::GetModuleFileNameW(nullptr, buf.data(), static_cast<std::uint32_t>(buf.size()));

            if (size == 0 || size == buf.size()) {
                REX::ERROR("error getting settings path: {}", REX::W32::GetLastError());
                return "";
            }

            return std::filesystem::path(buf.data()).replace_filename(L"Data\\SKSE\\Plugins\\gotobed.json");
        }
    }

    Settings& Settings::Get() {
        static Settings settings;
        return settings;
    }

    void Settings::Read() {
        try {
            std::ifstream f(GetSettingsPath());
            *this = json::parse(f).get<Settings>();
        } catch (json::exception& e) {
            REX::ERROR("error reading settings: {}", e.what());
        }
    }

    void Settings::Write() {
        try {
            std::ofstream f(GetSettingsPath());
            f << std::setw(4) << json(*this);
        } catch (json::exception& e) {
            REX::ERROR("error writing settings: {}", e.what());
        }
    }

    void to_json(json& a_json, Settings::Fixes const& a_fixes) {
        a_json["multipleMarkersReservation"] = a_fixes.multipleMarkersReservation;
    }

    void from_json(json const& a_json, Settings::Fixes& a_fixes) {
        a_json.at("multipleMarkersReservation").get_to(a_fixes.multipleMarkersReservation);
    }

    void to_json(json& a_json, Settings::Features const& a_features) {
        a_json["autoSleepMenu"] = a_features.autoSleepMenu;
    }

    void from_json(json const& a_json, Settings::Features& a_features) {
        a_features.autoSleepMenu = a_json.value("autoSleepMenu", a_features.autoSleepMenu);
    }

    void to_json(json& a_json, Settings::Keys const& a_keys) {
        a_json["sleep"] = a_keys.sleep;
        a_json["sleepMod"] = a_keys.sleepMod;
        a_json["serveTime"] = a_keys.serveTime;
        a_json["serveTimeMod"] = a_keys.serveTimeMod;
    }

    void from_json(json const& a_json, Settings::Keys& a_keys) {
        a_json.at("sleep").get_to(a_keys.sleep);
        a_json.at("sleepMod").get_to(a_keys.sleepMod);
        a_json.at("serveTime").get_to(a_keys.serveTime);
        a_json.at("serveTimeMod").get_to(a_keys.serveTimeMod);
    }

    void to_json(json& a_json, Settings const& a_settings) {
        a_json["fixes"] = a_settings.fixes;
        a_json["features"] = a_settings.features;
        a_json["keys"] = a_settings.keys;
    }

    void from_json(json const& a_json, Settings& a_settings) {
        a_json.at("fixes").get_to(a_settings.fixes);
        a_json.at("keys").get_to(a_settings.keys);

        if (a_json.contains("features")) {
            a_json.at("features").get_to(a_settings.features);
        }
    }
}
