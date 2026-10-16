#include "MenuOpenHandler.h"

#include "Offsets.h"
#include "Patch.h"
#include "Settings.h"

namespace Gotobed {
    namespace {
        std::uint32_t GetGamepadKeyCode(std::uint32_t a_idCode) {
            static std::unordered_map<std::uint32_t, std::uint32_t> map{
                {0x0001, 266},  // dpad up
                {0x0002, 267},  // dpad down
                {0x0004, 268},  // dpad left
                {0x0008, 269},  // dpad right
                {0x0010, 270},  // start
                {0x0020, 271},  // back
                {0x0040, 272},  // left thumb
                {0x0080, 273},  // right thumb
                {0x0100, 274},  // left shoulder
                {0x0200, 275},  // right shoulder
                {0x1000, 276},  // a
                {0x2000, 277},  // b
                {0x4000, 278},  // x
                {0x8000, 279},  // y
                {0x0009, 280},  // left trigger
                {0x000A, 281},  // right trigger
            };

            if (!map.contains(a_idCode)) {
                return 0;
            }

            return map.at(a_idCode);
        }

        bool IsKey(RE::ButtonEvent& a_event, std::int32_t a_key) {
            auto userEvents = RE::UserEvents::GetSingleton();

            if (a_key == -1) {
                return a_event.userEvent == userEvents->wait;
            }

            if (a_event.device == RE::INPUT_DEVICE::kKeyboard) {
                return static_cast<std::int32_t>(a_event.idCode) == a_key;
            }

            if (a_event.device == RE::INPUT_DEVICE::kGamepad) {
                return static_cast<std::int32_t>(GetGamepadKeyCode(a_event.idCode)) == a_key;
            }

            return false;
        }

        bool IsSleepKey(RE::ButtonEvent& a_event) {
            return IsKey(a_event, Settings::Get().keys.sleep);
        }

        bool IsServeTimeKey(RE::ButtonEvent& a_event) {
            return IsKey(a_event, Settings::Get().keys.serveTime);
        }

        void ServeSentenceCallback(std::uint8_t a_idx) {
            if (a_idx == 1) {
                RE::PlayerCharacter::GetSingleton()->ServePrisonTime();
            }
        }

        void ShowServeSentenceQuestion() {
            auto settings = RE::GameSettingCollection::GetSingleton();
            auto sServeSentenceQuestion = settings->GetSetting("sServeSentenceQuestion")->GetString();
            auto sYes = settings->GetSetting("sYes")->GetString();
            auto sNo = settings->GetSetting("sNo")->GetString();

            RE::MessageBoxMenu::Create(sServeSentenceQuestion, ServeSentenceCallback, 1, 0x19, 4, sYes, sNo);
        }
    }

    bool MenuOpenHandler::CanProcess_Orig(RE::InputEvent* a_event) {
        using func_t = decltype(&MenuOpenHandler::CanProcess_Orig);
        REL::Relocation<func_t> func{Offsets::MenuOpenHandler::CanProcess};
        return func(this, a_event);
    }

    bool MenuOpenHandler::CanProcess_Hook(RE::InputEvent* a_event) {
        if (a_event && a_event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
            if (IsSleepKey(*a_event->AsButtonEvent()) || IsServeTimeKey(*a_event->AsButtonEvent())) {
                return true;
            }
        }

        return CanProcess_Orig(a_event);
    }

    bool MenuOpenHandler::ProcessButton_Orig(RE::ButtonEvent* a_event) {
        using func_t = decltype(&MenuOpenHandler::ProcessButton_Orig);
        REL::Relocation<func_t> func{Offsets::MenuOpenHandler::ProcessButton};
        return func(this, a_event);
    }

    bool MenuOpenHandler::ProcessButton_Hook(RE::ButtonEvent* a_event) {
        if (a_event && a_event->IsDown()) {
            if (IsServeTimeKey(*a_event) && OnServeTimeButtonDown()) {
                return true;
            }

            if (IsSleepKey(*a_event) && OnSleepButtonDown()) {
                return true;
            }
        }

        return ProcessButton_Orig(a_event);
    }

    bool MenuOpenHandler::OnSleepButtonDown() {
        auto& settings = Settings::Get();

        if (settings.keys.sleepMod != -1) {
            auto kb = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();

            if (!kb->RE::BSInputDevice::IsPressed(static_cast<std::uint32_t>(settings.keys.sleepMod))) {
                return false;
            }
        }

        auto ui = RE::UI::GetSingleton();

        if (ui->GameIsPaused() || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
            return false;
        }

        auto player = RE::PlayerCharacter::GetSingleton();

        if (player->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kDying || player->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kDead) {
            return false;
        }

        if (player->actorState1.sitSleepState != RE::SIT_SLEEP_STATE::kIsSleeping) {
            return false;
        }

        RE::SleepWaitMenu::ToggleOpenMenu(true);

        return true;
    }

    bool MenuOpenHandler::OnServeTimeButtonDown() {
        auto& settings = Settings::Get();

        if (settings.keys.serveTimeMod != -1) {
            auto kb = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();

            if (!kb->RE::BSInputDevice::IsPressed(static_cast<std::uint32_t>(settings.keys.serveTimeMod))) {
                return false;
            }
        }

        auto ui = RE::UI::GetSingleton();

        if (ui->GameIsPaused() || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
            return false;
        }

        auto player = RE::PlayerCharacter::GetSingleton();

        if (player->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kDying || player->actorState1.lifeState == RE::ACTOR_LIFE_STATE::kDead) {
            return false;
        }

        if (player->jailSentence <= 0 || player->playerFlags.escaping) {
            return false;
        }

        ShowServeSentenceQuestion();

        return true;
    }

    void MenuOpenHandler::InstallHooks() {
        REL::Relocation<std::uintptr_t> vtbl{RE::VTABLE_MenuOpenHandler[0]};
        vtbl.write_vfunc(Patch::FindVfuncSlot("MenuOpenHandler::CanProcess", vtbl.address(), Offsets::MenuOpenHandler::CanProcess.address()), &MenuOpenHandler::CanProcess_Hook);
        vtbl.write_vfunc(Patch::FindVfuncSlot("MenuOpenHandler::ProcessButton", vtbl.address(), Offsets::MenuOpenHandler::ProcessButton.address()), &MenuOpenHandler::ProcessButton_Hook);
    }
}
