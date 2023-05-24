#pragma once
#include <JuceHeader.h>

class PatchConverter
{
public:
    String outputFolderPath;
    String patchNameSuffix;

    bool setLayerTitle = true;
    bool setLayerLevelAndPan = false;
    bool setLayerSplitDetails = false;
    bool retainPartLevelAndPan = true;
    bool retainPartSplitDetails = true;

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:	
    void processFile(File, int& fileCount);
    bool processPatchXmlAndReturnNewPatchName(XmlElement*, String& newNameOrErrorMessage);
    struct PartInfo
    {
        String partName;
        float mixLevel = 0.8f;
        float panPos = 0.5f;
        bool stackMode = false;
        bool splitDetailsValid = false;
        int lokey = 0, loFadeKey = 0, hiFadeKey = 127, hikey = 127;
        int left, right, leftRamp, rightRamp;

        bool hasFullKeyRange() { return (lokey == 0) && (hikey == 127); }

    };
    String isolateOneOmnispherePart(const String& stateInfo, int partIndex, PartInfo& partInfo);

    union { float f; uint8 bytes[4]; };

private:
    File outputFolder;
    std::unique_ptr<XmlElement> initializedMultiXml;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
