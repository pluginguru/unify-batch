#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String outputFolderPath;

    bool setLayerTitle = true;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(String, File, int& fileCount);
    void getCategoryAndPrefix(String folderName, String& category, String& prefix);
    XmlElement* convertFxpPresetToVst3PatchXml(String folderName, File inFile, String& newPatchNameOrErrorMessage);
    XmlElement* convertVst2PatchToVst3PatchXml(String folderName, File inFile, String& newPatchNameOrErrorMessage);
    void saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> unifyPatchXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
