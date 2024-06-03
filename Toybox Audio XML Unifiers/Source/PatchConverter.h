#pragma once
#include <JuceHeader.h>

#define LIBRARY_NAME "Unified - Toybox Buzz Zone"

class PatchConverter
{
public:
    String outputFolderPath;

    bool setLayerTitle = true;

    String libraryName = LIBRARY_NAME;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:
    void test();

    void processFile(File, int& fileCount);
    XmlElement* processPresetFile(File inFile, String& newPatchNameOrErrorMessage);

    void saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> unifyPatchXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
