#include "SneakFix.h"

namespace {
    void ResyncSneakState(RE::PlayerCharacter* a_player, const char* a_context) {
        bool graphSneaking = false;
        if (!a_player->GetGraphVariableBool("IsSneaking", graphSneaking)) {
            return;
        }

        const bool stateSneaking = a_player->ActorState::IsSneaking();
        if (graphSneaking == stateSneaking) {
            return;
        }

        const char* resyncEvent = stateSneaking ? "SneakStart" : "SneakStop";
        REX::INFO("{}: graph sneaking={} but actor state sneaking={}, sent {} ({})", a_context, graphSneaking, stateSneaking, resyncEvent, a_player->NotifyAnimationGraph(resyncEvent));
    }
}

namespace SneakFix {
    FurnitureHandler* FurnitureHandler::GetSingleton() {
        static FurnitureHandler singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl FurnitureHandler::ProcessEvent(const RE::TESFurnitureEvent* a_event, RE::BSTEventSource<RE::TESFurnitureEvent>*) {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (a_event && player && a_event->actor.get() == player) {
            const bool entering = a_event->type.get() == RE::TESFurnitureEvent::FurnitureEventType::kEnter;
            ResyncSneakState(player, entering ? "furniture enter" : "furniture exit");
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    void Install() {
        auto* holder = RE::ScriptEventSourceHolder::GetSingleton();
        if (!holder) {
            REX::ERROR("ScriptEventSourceHolder unavailable, sneak fix not installed");
            return;
        }

        holder->AddEventSink<RE::TESFurnitureEvent>(FurnitureHandler::GetSingleton());
        REX::INFO("furniture event sink installed");
    }
}