#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String outputFolderPath;

    bool setLayerTitle = true;

    // These are experimental, would require changes to GUI
#if 1
    bool hyperion = true;
    String libraryName = "WS Hyperion - Pick n Mix";
#else
    bool hyperion = false;
    String libraryName = "WS Theia - Pick n Mix";
#endif

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(File, int& fileCount);
    XmlElement* processTheiaCombi(File inFile, String& newPatchNameOrErrorMessage);
    XmlElement* processTheiaPatch(File inFile, String& newPatchNameOrErrorMessage);

    void saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> unifyPatchXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
