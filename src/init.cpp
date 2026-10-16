#include "init.h"

#include "MenuOpenHandler.h"
#include "Offsets.h"
#include "Patch.h"
#include "Settings.h"
#include "SettingsPapyrus.h"
#include "TESObjectREFR.h"

namespace Gotobed {
    void Init() {
        Settings::Get().Read();

        REL::Relocation<std::uintptr_t> playerVtbl{RE::VTABLE_PlayerCharacter[0]};
        auto playerUpdate = *reinterpret_cast<const std::uintptr_t*>(playerVtbl.address() + sizeof(void*) * 0xAD);  // Actor::Update

        // don't show sleep menu on bed activation
        Patch::Write<"84 C0 0F 84 ?? ?? ?? ?? B1 01 E8 ?? ?? ?? ?? B0 01 E9 ?? ?? ?? ??", 8>("sleep menu on bed activation", Offsets::TESFurniture::Activate.address(), static_cast<std::uint16_t>(0x0CEB));
        // don't show serve time message on bed activation
        Patch::Write<"83 B9 ?? ?? ?? ?? 00 76 ?? F6 81 ?? ?? ?? ?? 40 75 ??", 7>("serve time message on bed activation", Offsets::TESFurniture::Activate.address(), static_cast<std::uint8_t>(0xEB));
        // get up from bed using movement keys
        Patch::Write<"25 00 C0 03 00 3D 00 C0 00 00 0F 85 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ??", 3>("get up from bed using movement keys", playerUpdate, static_cast<std::uint8_t>(0x02));
        // don't allow sleeping if bed is reserved
        Patch::Write<"48 85 F6 0F 84 ?? ?? ?? ?? B2 01 48 8B CE E8 ?? ?? ?? ?? 84 C0 75 ??", 10>("sleeping in a reserved bed", Offsets::PlayerCharacter::CanSleepWait.address(), static_cast<std::uint8_t>(0x00));
        // don't pause character animations while sleeping
        Patch::Write<"49 8B 47 08 48 85 C0 74 ?? C7 80 ?? ?? ?? ?? 00 00 A0 40 B2 01", 7>("pause animations while sleeping 1", Offsets::AIProcess::OnSitSleepStateChange.address(), static_cast<std::uint8_t>(0xEB));
        Patch::Write<"3D 00 40 01 00 72 ?? 81 E1 00 C0 03 00 81 F9 00 C0 01 00 77 ??", 5>("pause animations while sleeping 2", Offsets::AIProcess::sub_674B60.address(), static_cast<std::uint8_t>(0xEB));
        Patch::Write<"25 00 C0 03 00 3D 00 C0 01 00 75 ?? 48 8B 8E ?? ?? ?? ?? 8B 96 ?? ?? ?? ??", 10>("pause animations while sleeping 3", Offsets::Actor::FinishLoadGame.address(), static_cast<std::uint8_t>(0xEB));

        MenuOpenHandler::InstallHooks();
        TESObjectREFR::InstallHooks();

        // papyrus
        SettingsPapyrus::Register();
    }
}
