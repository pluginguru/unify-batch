#pragma once
#include "FXP_Unifier.h"

class impOSCar2_Unifier : public FXP_Unifier
{
public:
    impOSCar2_Unifier();
    virtual ~impOSCar2_Unifier() = default;

    void getPatchDetailsAndVstState(File& sourcePresetFile, MemoryBlock& vstState) override;
};
