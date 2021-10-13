#pragma once
#include "TAL_Unifier.h"

class BassLine_Unifier : public TAL_Unifier
{
public:
    BassLine_Unifier();
    virtual ~BassLine_Unifier() = default;

    std::unique_ptr<XmlElement> makeUnifiedPatch(File& sourcePresetFile) override;
};
