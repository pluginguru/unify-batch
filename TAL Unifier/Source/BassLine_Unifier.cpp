#include "BassLine_Unifier.h"

// Uncomment exactly one of these
//#define SEVENTYFOURL
//#define ABSTRACTCATS
//#define GOODWIN
//#define XENOX
#define SOLIDTRAX

BassLine_Unifier::BassLine_Unifier()
{
    talPresetsFolder = File("C:\\Users\\shane\\AppData\\Roaming\\ToguAudioLine\\TAL-BassLine-101\\presets");
    presetFileType = "bassline";
    refPatch = File("D:\\PlugInGuru\\Content Development\\Unified TAL\\BassLine Template.unify");

#ifdef SEVENTYFOURL
    libraryName = "Unified - TAL-BassLine-101 74L";
    authorName = "74L";
    volumeScaling = 0.8;
#endif

#ifdef ABSTRACTCATS
    libraryName = "Unified - TAL-BassLine-101 AbstractCats";
    authorName = "AbstractCats";
    volumeScaling = 0.8;
#endif

#ifdef GOODWIN
    libraryName = "Unified - TAL-BassLine-101 David Goodwin";
    authorName = "David Goodwin";
    volumeScaling = 0.8;
#endif

#ifdef XENOX
    libraryName = "Unified - TAL-BassLine-101 Xenox";
    authorName = "Frank 'XENOX' Neumann";
    volumeScaling = 0.8;
#endif

#ifdef SOLIDTRAX
    libraryName = "Unified - TAL-BassLine-101 Solidtrax";
    authorName = "Solidtrax";
    volumeScaling = 0.8;
#endif

    sourceFolder = talPresetsFolder.getChildFile(authorName);
    patchComment = "TAL-BassLine factory patches by " + authorName;
    outputFolder = unifyLibrariesFolder.getChildFile(libraryName).getChildFile("Patches");
}

static struct
{
    String folder;
    String prefix;
}
folderPrefix[] =
{
    { "Arp", "BPM ARP" },
    { "Arpeggiator", "BPM ARP" },
    { "Bass", "BASS" },
    { "Chords", "CHORD" },
    { "Drums", "DRUM" },
    { "Fx", "SFX" },
    { "FX", "SFX" },
    { "Keys", "KEY" },
    { "Layer 1", "layer1" },    // not a real prefix, just something to indicate the patch must be classified manually
    { "Layer 2", "layer2" },    // as above
    { "Lead", "LEAD" },
    { "Leads", "LEAD" },
    { "Pads", "PAD" },
    { "Piano", "KEY" },
    { "Percusion", "DRUM" },
    { "Sequencer", "BPM SEQ" },
    { "SFX", "SFX" },
};

std::unique_ptr<XmlElement> BassLine_Unifier::makeUnifiedPatch(File& sourcePresetFile)
{
    auto xml = parseXML(sourcePresetFile);
    jassert(xml);
    xml->setAttribute("version", "6.0");

    auto pXml = xml->getChildByName("programs");
    jassert(pXml);
    pXml = pXml->getChildByName("program");
    jassert(pXml);
    pXml->setAttribute("path", adjustPath(sourcePresetFile.getFullPathName()));
    patchName = pXml->getStringAttribute("programname");

    double volume = pXml->getDoubleAttribute("volume");
    pXml->setAttribute("volume", volumeScaling * volume);

    prefix = "UNKNOWN";
#if defined(SEVENTYFOURL) || defined(XENOX) || defined(SOLIDTRAX)
    String folderName = sourcePresetFile.getParentDirectory().getFileName();
    for (auto fp : folderPrefix)
    {
        if (fp.folder == folderName)
            prefix = fp.prefix;
    }
#endif

    return xml;
}
