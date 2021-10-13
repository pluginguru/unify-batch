#include "J8_Unifier.h"

// Uncomment exactly one of these
//#define FACTORY
//#define JUPITER
//#define POLICH
//#define EMPTYVESSEL
//#define SOLIDTRAX
#define LUFTRUM

J8_Unifier::J8_Unifier()
{
    talPresetsFolder = File("C:\\Users\\shane\\AppData\\Roaming\\ToguAudioLine\\TAL-J-8\\presets");
    presetFileType = "pjp8";
    refPatch = File("D:\\PlugInGuru\\Content Development\\Unified TAL\\J8 Template.unify");

#ifdef FACTORY
    String sourceFolderName("TAL Factory");
    authorName = "TAL Software GmbH";
#endif

#ifdef JUPITER
    String sourceFolderName("Jupiter 8 Factory");
    authorName = "Jupiter 8 Factory";
#endif

#ifdef POLICH
    String sourceFolderName("Dave Polich");
    authorName = "Dave Polich";
#endif

#ifdef EMPTYVESSEL
    String sourceFolderName("emptyvessel");
    authorName = "Greg Cole | EmptyVessel";
#endif

#ifdef SOLIDTRAX
    String sourceFolderName("Solidtrax");
    authorName = "Solidtrax";
#endif

#ifdef LUFTRUM
    String sourceFolderName("Luftrum 24");
    authorName = "Luftrum";
#endif

    sourceFolder = talPresetsFolder.getChildFile(sourceFolderName);
    libraryName = "Unified - TAL-J-8 " + sourceFolderName;
    patchComment = "TAL-J-8 factory patches by " + authorName;
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
    { "Arpeggios", "BPM ARP" },
    { "Bass", "BASS" },
    { "Bass & Basslines", "BASS" },
    { "Chord", "CHORD" },
    { "Chords", "CHORD" },
    { "Code Elektro", "" },
    { "Drum", "DRUM" },
    { "Drums", "DRUM" },
    { "Fx", "SFX" },
    { "FX", "SFX" },
    { "Keys", "KEY" },
    { "Keys & Plucks", "KEY" },
    { "Layer 1", "layer1" },    // not a real prefix, just something to indicate the patch must be classified manually
    { "Layer 2", "layer2" },    // as above
    { "Lead", "LEAD" },
    { "Leads", "LEAD" },
    { "Mono Leads", "LEAD" },
    { "Pad", "PAD" },
    { "Pads", "PAD" },
    { "Pad & Strings", "SYNTH" },
    { "Piano", "KEY" },
    { "Percusion", "DRUM" },
    { "Poly Sytnhs", "KEY" },
    { "Sequencer", "BPM SEQ" },
    { "SFX", "SFX" },
    { "Split", "SPLIT" },
    { "Stab", "SYNTH" },
};

static struct
{
    String word;
    String prefix;
}
wordPrefix[] =
{
    { "AP", "ARP" },
    { "BL", "BPM BASS" },
    { "BS", "BASS" },
    { "DR", "DRUM" },
    { "KY", "KEY" },
    { "LD", "LEAD" },
    { "PD", "PAD" },

    { "Performance", "KEY" },
    { "Piano", "KEY" },
    { "Clav", "KEY" },
    { "Lead", "LEAD" },
    { "Synth", "SYNTH" },
    { "SY", "SYNTH" },
    { "Pad", "PAD" },
    { "PD", "PAD" },
    { "Bass", "BASS" },
    { "Slap", "BASS" },
    { "Brass", "BRASS" },
    { "String", "STR" },
    { "Bell", "BELL" },
    { "Choir", "VOX" },
    { "Vox", "VOX" },
    { "Kalimba", "PLUCK" },
    { "Harp", "PLUCK" },
    { "PL", "PLUCK" },

    { "DDD-1", "DRUM KIT" },
    { "Drumulator", "DRUM KIT" },
    { "Guitar", "GTR" },
    { "Clarinet", "WIND" },
    { "Shakuhachi", "WIND" },
    { "Horn", "Brass" },
    { "Flute", "WIND" },
    { "Fuzz", "BASS" },
    { "Oboe", "WIND" },
    { "Trombone", "BRASS" },
    { "Trumpet", "BRASS" },
    { "Angels", "VOX" },
    { "Voice", "VOX" },
    { "Hammond", "ORG" },
    { "Organ", "ORG" },
    { "Drums", "DRUM KIT" },
    { "Violin", "STR" },
};

std::unique_ptr<XmlElement> J8_Unifier::makeUnifiedPatch(File& sourcePresetFile)
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

    prefix.clear();
    String folderName = sourcePresetFile.getParentDirectory().getFileName();
    for (auto fp : folderPrefix)
    {
        if (fp.folder == folderName)
            prefix = fp.prefix;
    }
    if (prefix.isEmpty()) for (auto fp : wordPrefix)
    {
        if (patchName.contains(fp.word))
            prefix = fp.prefix;
    }
    if (prefix.isEmpty())
    {
        prefix = "UNKNOWN";
        DBG(patchName);
    }

    return xml;
}
