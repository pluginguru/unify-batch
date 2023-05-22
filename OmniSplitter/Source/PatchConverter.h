#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String outputFolderPath;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(File, int& fileCount);
    bool processPatchXmlAndReturnNewPatchName(XmlElement*, String& newNameOrErrorMessage);
    String isolateOneOmnispherePart(String& stateInfo, int partIndex, float& mixLevel, float& panPos);

    union { float f; uint8 bytes[4]; };

private:
    File outputFolder;
    std::unique_ptr<XmlElement> initializedMultiXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
