#pragma once

namespace SneakFix {
    class FurnitureHandler : public RE::BSTEventSink<RE::TESFurnitureEvent> {
    public:
        static FurnitureHandler* GetSingleton();
        RE::BSEventNotifyControl ProcessEvent(const RE::TESFurnitureEvent* a_event, RE::BSTEventSource<RE::TESFurnitureEvent>*) override;

    private:
        FurnitureHandler() = default;
    };

    void Install();
}