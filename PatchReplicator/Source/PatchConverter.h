#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String referencePatchPath;
    String outputFolderPath;
    bool updateLayerTitles;

public:
    PatchConverter();
    int processFiles(String patchNamePattern, const StringArray& sfzPaths);

protected:	
    void processFile(File sfzFile, String patchNamePattern, int& fileCount);
    String convertGuruSamplerState(String stateInfo, String newSfzName);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> refPatchXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
