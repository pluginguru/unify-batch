#pragma once
#include <JuceHeader.h>
#include "MyVstUtils.h"

class Synth1_Unifier
{
public:
    Synth1_Unifier();

    void makeUnifiedLibrary();
    void getPatchDetailsAndVstState(File& sourcePresetFile, MemoryBlock& vstState);

protected:
    String adjustPath(String path) { return path.replace("\\", "/"); }
    AudioPluginInstance* instantiatePlugin(String formatName, String pluginNamePrefix, String vendorName);

    // Constructors set these up
    File unifyLibrariesFolder;
    File presetsRootFolder;
    String presetFileType;
    String libraryName, authorName;
    File refPatch, sourceFolder, outputFolder;

    // getPatchDetailsAndVstState() modifies these
    String prefix, patchName, patchCategory, patchTags, patchComment;

private:
    MemoryBlock refState;
    MyVstConv vstConv;
    std::unique_ptr<AudioPluginInstance> plugin;
};
