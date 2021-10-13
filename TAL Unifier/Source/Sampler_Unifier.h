#pragma once
#include "TAL_Unifier.h"

class Sampler_Unifier   : public TAL_Unifier
{
public:
    Sampler_Unifier();
    virtual ~Sampler_Unifier() = default;

    std::unique_ptr<XmlElement> makeUnifiedPatch(File& sourcePresetFile) override;
};
