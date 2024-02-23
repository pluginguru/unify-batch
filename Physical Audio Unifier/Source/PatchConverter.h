#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String outputFolderPath;

    bool setLayerTitle = true;

#if 0
    String libraryName = "Unified - PA Derailer";
#else
    String libraryName;
#endif

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:
    void test();

    void processFile(File, int& fileCount);
    XmlElement* processPresetFile(File inFile, String& newPatchNameOrErrorMessage, int fileNumber);

    void saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> unifyPatchXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
