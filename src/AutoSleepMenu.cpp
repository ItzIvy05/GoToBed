#include "AutoSleepMenu.h"

#include "MenuOpenHandler.h"
#include "Settings.h"

namespace Gotobed::AutoSleepMenu {
    namespace {
        constexpr std::uint32_t TIMEOUT_MS = 20000;

        bool watching = false;
        std::uint32_t startTime = 0;

        void Poll() {
            auto player = RE::PlayerCharacter::GetSingleton();
            auto sitSleepState = player->actorState1.sitSleepState;

            if (RE::GetDurationOfApplicationRunTime() - startTime > TIMEOUT_MS || sitSleepState == RE::SIT_SLEEP_STATE::kIsSitting) {
                watching = false;
                return;
            }

            if (sitSleepState == RE::SIT_SLEEP_STATE::kIsSleeping) {
                if (MenuOpenHandler::TryOpenServeTimeQuestion() || MenuOpenHandler::TryOpenSleepMenu()) {
                    watching = false;
                    return;
                }
            }

            SKSE::GetTaskInterface()->AddTask(Poll);
        }

        class FurnitureHandler : public RE::BSTEventSink<RE::TESFurnitureEvent> {
        public:
            static FurnitureHandler* GetSingleton() {
                static FurnitureHandler singleton;
                return &singleton;
            }

            RE::BSEventNotifyControl ProcessEvent(const RE::TESFurnitureEvent* a_event, RE::BSTEventSource<RE::TESFurnitureEvent>*) override {
                auto player = RE::PlayerCharacter::GetSingleton();

                if (a_event && !watching && Settings::Get().features.autoSleepMenu && a_event->actor.get() == player && a_event->type.get() == RE::TESFurnitureEvent::FurnitureEventType::kEnter) {
                    watching = true;
                    startTime = RE::GetDurationOfApplicationRunTime();
                    SKSE::GetTaskInterface()->AddTask(Poll);
                }

                return RE::BSEventNotifyControl::kContinue;
            }

        private:
            FurnitureHandler() = default;
        };
    }

    void Install() {
        auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink<RE::TESFurnitureEvent>(FurnitureHandler::GetSingleton());
    }
}
