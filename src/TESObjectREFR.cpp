#include "TESObjectREFR.h"

#include "Offsets.h"
#include "Patch.h"
#include "Settings.h"

namespace Gotobed {
    bool TESObjectREFR::SetMarkerReserved_Orig(std::uint32_t a_marker, RE::Actor* a_actor, bool a_reserved, bool a_ignoreUsed) {
        using func_t = decltype(&TESObjectREFR::SetMarkerReserved_Orig);
        REL::Relocation<func_t> func{Offsets::TESObjectREFR::SetMarkerReserved};
        return func(this, a_marker, a_actor, a_reserved, a_ignoreUsed);
    }

    bool TESObjectREFR::SetMarkerReserved_Hook(std::uint32_t a_marker, RE::Actor* a_actor, bool a_reserved, bool a_ignoreUsed) {
        auto middleHigh = a_actor->currentProcess->middleHigh;

        if (middleHigh && middleHigh->reservationSlot != a_marker) {
            if (middleHigh->reservationSlot != 0xFFFFFFFF) {
                SetMarkerReserved_Orig(middleHigh->reservationSlot, nullptr, false, false);
            }

            middleHigh->reservationSlot = a_marker;
        }

        return SetMarkerReserved_Orig(a_marker, a_actor, a_reserved, a_ignoreUsed);
    }

    void TESObjectREFR::InstallHooks() {
        auto& settings = Settings::Get();

        if (!settings.fixes.multipleMarkersReservation) {
            return;
        }

        auto site = Patch::FindCallTo("multiple markers reservation", Offsets::BGSProcedureSitSleepExecState::ActivateTarget.address(), Offsets::TESObjectREFR::SetMarkerReserved.address());
        REL::Relocation<std::uintptr_t> call{site};
        call.write_call<5>(&TESObjectREFR::SetMarkerReserved_Hook);
    }
}
