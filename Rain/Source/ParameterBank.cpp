// ─── ParameterBank.cpp ───────────────────────────────────────────────────────────
#include "ParameterBank.h"
#include "ParameterManager.h"

void ParameterBank::loadFromManager(ParameterManager& mgr)
{
    for (std::size_t i = 0; i < ptrs.size(); ++i)
        ptrs[i] = mgr.getRawParameterValue(ParamID::Names[i]);
}