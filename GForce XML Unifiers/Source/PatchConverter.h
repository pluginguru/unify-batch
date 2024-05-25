#pragma once
#include <JuceHeader.h>

// Uncomment only one of these
//#define PLUGIN_IS_IMPOSCAR3
#define PLUGIN_IS_AXXESS

#if defined(PLUGIN_IN_IMPOSCAR3)
    #define LIBRARY_NAME "Unified - GForce impOSCar3"
#elif defined(PLUGIN_IS_AXXESS)
    #define LIBRARY_NAME "Unified - GForce AXXESS"
#endif

//#define CHECK_CATEGORIES

class PatchConverter
{
public:
    String outputFolderPath;

    bool setLayerTitle = true;

#if 1
    String libraryName = LIBRARY_NAME;
#else
    String libraryName;
#endif

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:
    void test();

    void getCategoryAndPrefix(String, String&, String&);

    void processFile(File, int& fileCount);
    XmlElement* processPresetFile(File inFile, String& newPatchNameOrErrorMessage);

    void saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> unifyPatchXml;

#ifdef CHECK_CATEGORIES
    StringArray categories;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
