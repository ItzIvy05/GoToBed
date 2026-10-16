#pragma once

namespace Gotobed::Offsets {
    namespace TESObjectREFR {
        constexpr REL::ID SetMarkerReserved{RELOCATION_ID(19765, 20160)};  // TESObjectREFR::SetMarkerReserved
    }

    namespace PlayerCharacter {
        constexpr REL::ID CanSleepWait{RELOCATION_ID(39371, 40443)};  // PlayerCharacter::CanSleepWait
    }

    namespace AIProcess {
        constexpr REL::ID OnSitSleepStateChange{RELOCATION_ID(38871, 39912)};  // AIProcess::OnSitSleepStateChange
        constexpr REL::ID sub_674B60{RELOCATION_ID(38317, 39292)};              // AIProcess::sub_674B60
    }

    namespace BGSProcedureSitSleepExecState {
        constexpr REL::ID ActivateTarget{RELOCATION_ID(28400, 29150)};  // BGSProcedureSitSleepExecState::ActivateTarget
    }
}
