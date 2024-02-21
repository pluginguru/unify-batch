#pragma once
#include "TAL_Unifier.h"

class Pha_Unifier : public TAL_Unifier
{
public:
    Pha_Unifier();
    virtual ~Pha_Unifier() = default;

    std::unique_ptr<XmlElement> makeUnifiedPatch(File& sourcePresetFile) override;
};
