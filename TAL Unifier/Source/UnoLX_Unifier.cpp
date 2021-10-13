#include "UnoLX_Unifier.h"

// Uncomment exactly one of these
//#define FMR
#define FN
//#define JUNO
//#define TAL

UnoLX_Unifier::UnoLX_Unifier()
{
    talPresetsFolder = File("C:\\Users\\shane\\AppData\\Roaming\\ToguAudioLine\\TAL-U-No-LX\\presets");
    presetFileType = "pjunoxl";
    refPatch = File("D:\\PlugInGuru\\Content Development\\Unified TAL\\U-No-LX Template.unify");
    authorName = "TAL Software GmbH";

#ifdef FMR
    String sourceFolderName("FMR Presets Bank");
#endif

#ifdef FN
    String sourceFolderName("FN Presets Bank");
#endif

#ifdef JUNO
    String sourceFolderName("Juno Factory Bank A");
    volumeScaling = 1.8;
#endif

#ifdef TAL
    String sourceFolderName("TAL Presets Bank");
    volumeScaling = 2.0;
#endif

    sourceFolder = talPresetsFolder.getChildFile(sourceFolderName);
    libraryName = "Unified - TAL-U-No-LX " + sourceFolderName;
    patchComment = "TAL-U-No-LX factory patches by " + authorName;
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
    { "Brass", "BRASS" },
    { "Chord", "CHORD" },
    { "Chords", "CHORD" },
    { "Drones", "DRONE" },
    { "Drum", "DRUM" },
    { "Drums", "DRUM" },
    { "Fx", "SFX" },
    { "FX", "SFX" },
    { "Guitar", "GTR" },
    { "Keyboard", "KEY" },
    { "Keys", "KEY" },
    { "Keys & Plucks", "KEY" },
    { "Layer 1", "layer1" },    // not a real prefix, just something to indicate the patch must be classified manually
    { "Layer 2", "layer2" },    // as above
    { "Lead", "LEAD" },
    { "Leads", "LEAD" },
    { "Organ", "ORG" },
    { "Pad", "PAD" },
    { "Pads", "PAD" },
    { "Pad & Strings", "PAD" },
    { "Piano", "KEY" },
    { "Percusion", "DRUM" },
    { "Percussion", "DRUM" },
    { "Sequencer", "BPM SEQ" },
    { "S-Fx", "SFX" },
    { "SFX", "SFX" },
    { "Stab", "SYNTH" },
    { "Strings", "STR" },
    { "Synth", "SYNTH" },
    { "Wind", "WIND" },
};

static struct
{
    String word;
    String prefix;
}
wordPrefix[] =
{
    { "Performance", "KEY" },
    { "Piano", "KEY" },
    { "Clav", "KEY" },
    { "Lead", "LEAD" },
    { "LEA", "LEAD" },
    { "ARP", "ARP" },
    { "Synth", "SYNTH" },
    { "SY", "SYNTH" },
    { "KEY", "KEY" },
    { "Pad", "PAD" },
    { "PAD", "PAD" },
    { "PD", "PAD" },
    { "Bass", "BASS" },
    { "BAS", "BASS" },
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

std::unique_ptr<XmlElement> UnoLX_Unifier::makeUnifiedPatch(File& sourcePresetFile)
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
