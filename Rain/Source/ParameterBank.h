// ─── ParameterBank.h ─────────────────────────────────────────────────────────────
#pragma once
#include <array>
#include <atomic>
#include "ParameterIDs.h"

struct ParameterBank
{
    using Ptr = std::atomic<float>*;
    std::array<Ptr, static_cast<std::size_t>(ParamID::ID::Count)> ptrs{ nullptr };

    // Convenience accessor
    [[nodiscard]] inline Ptr operator[](ParamID::ID id) const noexcept
    {
        return ptrs[static_cast<std::size_t>(id)];
    }

    inline float get(ParamID::ID id,
        std::memory_order mo = std::memory_order_relaxed) const noexcept
    {
        return ptrs[static_cast<std::size_t>(id)]->load(mo);
    }

    void loadFromManager(class ParameterManager& mgr);
};
