#pragma once
#include "PatchConverter.h"

/*  KorgPatchFinalizer is a simple type of PatchConverter which looks up patch's "category"
 *  metadata string in a table, and if it finds a match, adds a corresponding prefix to the patch name.
*/

class KorgPatchFinalizer    : public PatchConverter
{
public:
    KorgPatchFinalizer();
    virtual ~KorgPatchFinalizer() = default;

    void processPatchXml(XmlElement* patchXml) override;

protected:
    OwnedArray<StringArray> tsvData;

    StringArray* lookupPatchName(String& name);
};
