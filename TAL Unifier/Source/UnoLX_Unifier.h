#pragma once
#include "TAL_Unifier.h"

class UnoLX_Unifier : public TAL_Unifier
{
public:
    UnoLX_Unifier();
    virtual ~UnoLX_Unifier() = default;

    std::unique_ptr<XmlElement> makeUnifiedPatch(File& sourcePresetFile) override;
};
