#pragma once
#include <JuceHeader.h>

// Uncomment only one of these
//#define PLUGIN_IS_IMPOSCAR3
//#define PLUGIN_IS_AXXESS
//#define PLUGIN_IS_OBONE
//#define PLUGIN_IS_BASSSTATION
#define PLUGIN_IS_TVSPRO
//#define PLUGIN_IS_BABYLON2

#if defined(PLUGIN_IS_IMPOSCAR3)
    #define LIBRARY_NAME "Unified - GForce impOSCar3"
#elif defined(PLUGIN_IS_AXXESS)
    #define LIBRARY_NAME "Unified - GForce AXXESS"
#elif defined(PLUGIN_IS_OBONE)
    #define LIBRARY_NAME "Unified - GForce OB-1"
#elif defined(PLUGIN_IS_BASSSTATION)
    #define LIBRARY_NAME "Unified - GForce Bass Station"
#elif defined(PLUGIN_IS_TVSPRO)
    #define LIBRARY_NAME "Unified - GForce TVS Pro"
#elif defined(PLUGIN_IS_BABYLON2)
    #define LIBRARY_NAME "Unified - WAP Babylon 2"
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
    void reveng();

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
