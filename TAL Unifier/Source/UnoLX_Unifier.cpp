#include "UnoLX_Unifier.h"

// Uncomment exactly one of these
//#define FMR
//#define FN
//#define JUNO
//#define TAL
#define SS

UnoLX_Unifier::UnoLX_Unifier()
{
    talPresetsFolder = File("C:\\Users\\owner\\AppData\\Roaming\\ToguAudioLine\\TAL-U-No-LX\\presets");
    presetFileType = "pjunoxl";
    refPatch = File("E:\\PlugInGuru\\Unify\\Libraries\\Unified - TAL-U-No-LX Opulenta Plus\\Patches\\TEMPLATE.unify");
    authorName = "TAL Software GmbH";


#ifdef FMR
    String sourceFolderName("FMR Presets Bank");
    libraryName = "Unified - TAL-U-No-LX " + sourceFolderName;
#endif

#ifdef FN
    String sourceFolderName("FN Presets Bank");
    libraryName = "Unified - TAL-U-No-LX " + sourceFolderName;
#endif

#ifdef JUNO
    String sourceFolderName("Juno Factory Bank A");
    volumeScaling = 1.8;
    libraryName = "Unified - TAL-U-No-LX " + sourceFolderName;
#endif

#ifdef TAL
    String sourceFolderName("TAL Presets Bank");
    volumeScaling = 2.0;
    libraryName = "Unified - TAL-U-No-LX " + sourceFolderName;
#endif

#ifdef SS
    String sourceFolderName("Opulenta Plus - Saif Sameer");
    authorName = "Saif Sameer";
    libraryName = "Unified - TAL-U-No-LX Opulenta Plus";
#endif

    patchComment = "TAL-U-No-LX patches by " + authorName;
    sourceFolder = talPresetsFolder.getChildFile(sourceFolderName);
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
    { "Drone", "DRONE" },
    { "Drums", "DRUM" },
    { "Drum", "DRUM" },
    { "Fx", "SFX" },
    { "FX", "SFX" },
    { "Guitar", "GTR" },
    { "Keyboard", "KEY" },
    { "Keys & Plucks", "KEY" },
    { "Keys", "KEY" },
    { "Layer 1", "layer1" },    // not a real prefix, just something to indicate the patch must be classified manually
    { "Layer 2", "layer2" },    // as above
    { "Leads", "LEAD" },
    { "Lead", "LEAD" },
    { "Motion", "MOTION" },
    { "Organ", "ORG" },
    { "Pads", "PAD" },
    { "Pad & Strings", "PAD" },
    { "Pad", "PAD" },
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

static String redundantPrefixes[] =
{
    "ARP ",
    "BR ",
    "BS ",
    "BSQ ",
    "DN ",
    "DR ",
    "KY ",
    "LD ",
    "MO ",
    "PD ",
    "SQ ",
    "SY ",
};

std::unique_ptr<XmlElement> UnoLX_Unifier::makeUnifiedPatch(File& sourcePresetFile)
{
    auto xml = parseXML(sourcePresetFile);
    jassert(xml);
    //xml->setAttribute("version", "6.0");

    auto pXml = xml->getChildByName("programs");
    jassert(pXml);
    pXml = pXml->getChildByName("program");
    jassert(pXml);
    pXml->setAttribute("path", adjustPath(sourcePresetFile.getFullPathName()));
    patchName = presetName = pXml->getStringAttribute("programname");

#if 0
    // experimental
    delayDryWet = pXml->getDoubleAttribute("delayDryWet");
    reverbDryWet = pXml->getDoubleAttribute("reverbDryWet");
    arpRate = pXml->getDoubleAttribute("arprate");
#endif

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

    for (auto& rp : redundantPrefixes)
    {
        if (patchName.startsWith(rp))
        {
            patchName = patchName.substring(rp.length()).trim();
            while (patchName.startsWithChar('-'))
                patchName = patchName.substring(1).trim();
            break;
        }
    }

    return xml;
}
