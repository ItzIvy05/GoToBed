#pragma once

namespace Gotobed::Patch {
    constexpr std::size_t SCAN_RANGE = 0x800;
    constexpr std::size_t VTABLE_RANGE = 16;

    template <REX::TStaticString S>
    std::uintptr_t FindUnique(std::string_view a_name, std::uintptr_t a_func) {
        auto pattern = REL::Pattern<S>();
        std::uintptr_t found = 0;
        std::size_t count = 0;

        for (std::size_t i = 0; i < SCAN_RANGE; ++i) {
            if (pattern.match(a_func + i)) {
                found = a_func + i;
                ++count;
            }
        }

        if (count != 1) {
            REX::FAIL("{}: expected 1 pattern match in function at {:X}, found {}. This game version is not supported.", a_name, a_func, count);
        }

        return found;
    }

    template <REX::TStaticString S, std::ptrdiff_t TARGET, class T>
    void Write(std::string_view a_name, std::uintptr_t a_func, T a_value) {
        REL::Relocation<std::uintptr_t> site{FindUnique<S>(a_name, a_func) + TARGET};
        site.write(a_value);
    }

    inline std::uintptr_t FindCallTo(std::string_view a_name, std::uintptr_t a_func, std::uintptr_t a_target) {
        std::uintptr_t found = 0;
        std::size_t count = 0;

        for (std::size_t i = 0; i < SCAN_RANGE; ++i) {
            auto at = a_func + i;

            if (*reinterpret_cast<const std::uint8_t*>(at) != 0xE8) {
                continue;
            }

            auto displacement = *reinterpret_cast<const std::int32_t*>(at + 1);

            if (at + 5 + displacement == a_target) {
                found = at;
                ++count;
            }
        }

        if (count != 1) {
            REX::FAIL("{}: expected 1 call in function at {:X}, found {}. This game version is not supported.", a_name, a_func, count);
        }

        return found;
    }

    inline std::size_t FindVfuncSlot(std::string_view a_name, std::uintptr_t a_vtbl, std::uintptr_t a_func) {
        for (std::size_t i = 0; i < VTABLE_RANGE; ++i) {
            if (*reinterpret_cast<const std::uintptr_t*>(a_vtbl + sizeof(void*) * i) == a_func) {
                return i;
            }
        }

        REX::FAIL("{}: function not found in the first {} vtable slots. This game version is not supported.", a_name, VTABLE_RANGE);

        return 0;
    }
}
