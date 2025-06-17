#pragma once
#include <JuceHeader.h>

//#define LIST_TYPES_AND_SUBTYPES
#ifndef LIST_TYPES_AND_SUBTYPES
  #define LOOK_UP_PATCH_PREFIXES
  #define FORCE_TYPE_SUBTYPE_COMMENTS
#endif

class PatchConverter
{
public:
    String outputFolderPath;

    //String libraryName = "Unified - Arturia Pigments";
    //String libraryName = "Unified - SabrePigs";
    //String libraryName = "Unified - Tom Wolfe's SynthVault";
    //String libraryName = "RMS Pigments - Celestial Resonance";
    //String libraryName = "MUM Pigments - Mystic Moonlight";
    //String libraryName = "MUM Pigments - Spice";
    //String libraryName = "Trajectoire Pigments - Shades Blue";
    //String libraryName = "Trajectoire Pigments - Shades Green";
    //String libraryName = "Trajectoire Pigments - Shades Complete";
    //String libraryName = "Fractal Sounds Pigments - Cloudlands";
    //String libraryName = "Fractal Sounds Pigments - Liquid Synths";
    //String libraryName = "Fractal Sounds Pigments - Psytrance Fusion";
    //String libraryName = "Fractal Sounds Pigments - Psytrance Fusion 2";
    //String libraryName = "Fractal Sounds Pigments - Shamanic Voices";
    String libraryName = "Fractal Sounds Pigments - Vapor Voices";

public:
    PatchConverter();
    int processFiles(const StringArray& filePaths);

protected:
    void test();

    void getMetadata(MemoryBlock mb);
    void processFile(File, int& fileCount);
    XmlElement* processPresetFile(File inFile, String& newPatchNameOrErrorMessage);

    void saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml);

private:
    File outputFolder;
    std::unique_ptr<XmlElement> unifyPatchXml;

    String presetName;
    String collection;
    String author;
    String comment;
    String tags;

    String prefix;
    String category;

    String type, subtype;

#ifdef LIST_TYPES_AND_SUBTYPES
    StringArray typesAndSubtypes;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchConverter)
};
