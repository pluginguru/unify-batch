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

    // subclasses must override this
    virtual void processPatchXml(XmlElement* patchXml) = 0;

private:
    File outputFolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
