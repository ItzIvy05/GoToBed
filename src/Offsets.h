#pragma once

namespace Gotobed::Offsets {
    namespace TESFurniture {
        constexpr REL::ID Activate{RELOCATION_ID(17034, 17420)};  // TESFurniture::Activate
    }

    namespace TESObjectREFR {
        constexpr REL::ID SetMarkerReserved{RELOCATION_ID(19765, 20160)};  // TESObjectREFR::SetMarkerReserved
    }

    namespace Actor {
        constexpr REL::ID FinishLoadGame{RELOCATION_ID(36644, 37652)};  // Actor::FinishLoadGame
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

    namespace MenuOpenHandler {
        constexpr REL::ID CanProcess{RELOCATION_ID(51410, 52259)};     // MenuOpenHandler::CanProcess
        constexpr REL::ID ProcessButton{RELOCATION_ID(51400, 52249)};  // MenuOpenHandler::ProcessButton
    }
}
