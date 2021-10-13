#pragma once
#include "PatchConverter.h"

/*  CategoryBasedConverter is a simple type of PatchConverter which looks up patch's "category"
 *  metadata string in a table, and if it finds a match, adds a corresponding prefix to the patch name.
*/

class CategoryBasedConverter    : public PatchConverter
{
public:
    CategoryBasedConverter() : PatchConverter() {}
    virtual ~CategoryBasedConverter() = default;

    void processPatchXml(XmlElement* patchXml) override;
};
