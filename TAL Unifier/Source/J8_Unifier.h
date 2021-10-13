#pragma once
#include "TAL_Unifier.h"

class J8_Unifier : public TAL_Unifier
{
public:
    J8_Unifier();
    virtual ~J8_Unifier() = default;

    std::unique_ptr<XmlElement> makeUnifiedPatch(File& sourcePresetFile) override;
};
