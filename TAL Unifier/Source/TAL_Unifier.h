#pragma once
#include <JuceHeader.h>
#include "MyVstUtils.h"

class TAL_Unifier
{
public:
    TAL_Unifier();
    virtual ~TAL_Unifier() = default;

    void makeUnifiedLibrary();

    // subclasses must override this function
    virtual std::unique_ptr<XmlElement> makeUnifiedPatch(File& sourcePresetFile) = 0;

protected:
    String adjustPath(String path) { return path.replace("\\", "/"); }

    // Constructors set these up
    File unifyLibrariesFolder;
    File talPresetsFolder;
    String presetFileType;
    String libraryName, authorName;
    File refPatch, sourceFolder, outputFolder;
    double volumeScaling;

    // makeUnifiedPatch() modifies these
    String prefix, presetName, patchName, patchCategory, patchTags, patchComment;

#if 0
    // experimental: U-No-LX extracted parameter values
    double delayDryWet, reverbDryWet, arpRate;
#endif

private:
    MyVstConv vstConv;
};
