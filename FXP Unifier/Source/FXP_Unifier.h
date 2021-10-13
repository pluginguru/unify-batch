#pragma once
#include <JuceHeader.h>
#include "MyVstUtils.h"

class FXP_Unifier
{
public:
    FXP_Unifier();
    virtual ~FXP_Unifier() = default;

    void makeUnifiedLibrary();

    // subclasses must override this function
    virtual void getPatchDetailsAndVstState(File& sourcePresetFile, MemoryBlock& vstState) = 0;

protected:
    String adjustPath(String path) { return path.replace("\\", "/"); }

    // Constructors set these up
    File unifyLibrariesFolder;
    File presetsRootFolder;
    String presetFileType;
    String libraryName, authorName;
    File refPatch, sourceFolder, outputFolder;

    // getPatchDetailsAndVstState() modifies these
    String prefix, patchName, patchCategory, patchTags, patchComment;

private:
    MyVstConv vstConv;
};
