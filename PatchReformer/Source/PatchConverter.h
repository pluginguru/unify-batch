#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String referencePatchPath;
    String outputFolderPath;
    bool updateLayer1Title;
    bool saveMidiFx, saveAudioFx;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(File, int& fileCount);

private:
    File outputFolder;

    std::unique_ptr<XmlElement> refPatchXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
