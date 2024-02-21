#include "Pha_Unifier.h"

static struct
{
    String tag;
    String prefix;
}
tagPrefix[] =
{
    { "Percussion", "PERC" },
    { "Drum", "DRUM" },
    { "Keyboards", "KEY" },
    { "Organ", "ORG" },
    { "Lead", "LEAD" },
    { "ARP", "ARP" },
    { "Synth", "SYNTH" },
    { "Pad", "PAD" },
    { "Bass", "BASS" },
    { "String", "STR" },
    { "Wind", "WIND" },
    { "Bell", "BELL" },
    { "Pluck", "PLUCK" },
    { "FX", "SFX" },
};

static struct
{
    String word;
    String prefix;
}
wordPrefix[] =
{
    { "Brass", "BRASS" },
};

static struct
{
    String inPrefix;
    String outPrefix;
}
prefixPrefix[] =
{
    { "ARP ", "ARP" },
    { "BL ", "BASS" },
    { "BS ", "BASS" },
    { "CH ", "CHORD" },
    { "DR ", "DRUM" },
    { "KY ", "KEY" },
    { "LD ", "LEAD" },
    { "PD ", "PAD" },
    { "SC ", "ATMO" },
    { "SFX ", "SFX" },
    { "SQ ", "ARP" },
    { "SY ", "SYNTH" },

    { "ARP-", "ARP" },
    { "BAS-", "BASS" },
    { "KEY-", "KEY" },
    { "PAD-", "PAD" },
};

static struct
{
    String folder;
    String prefix;
}
folderPrefix[] =
{
    { "Arp", "BPM ARP" },
    { "ARP", "BPM ARP" },
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
    { "Keyboards", "KEY" },
    { "Keys", "KEY" },
    { "Keys & Plucks", "KEY" },
    { "Lead", "LEAD" },
    { "Leads", "LEAD" },
    { "Organ", "ORG" },
    { "Pad", "PAD" },
    { "Pads", "PAD" },
    { "Pad & Strings", "PAD" },
    { "Piano", "KEY" },
    { "Percusion", "DRUM" },
    { "Percussion", "DRUM" },
    { "Pluck", "PLUCK" },
    { "Sequencer", "BPM SEQ" },
    { "Soundscapes", "ATMO" },
    { "S-Fx", "SFX" },
    { "SFX", "SFX" },
    { "Stab", "SYNTH" },
    { "Strings", "STR" },
    { "Synth", "SYNTH" },
    { "Wind", "WIND" },
};


Pha_Unifier::Pha_Unifier()
{
    talPresetsFolder = File("C:\\Users\\owner\\AppData\\Roaming\\ToguAudioLine\\TAL-Pha\\presets");
    presetFileType = "talalpha";
    refPatch = File("E:\\PlugInGuru\\Unify\\Libraries\\Unified - TAL-Pha\\TAL-Pha Template.unify");

    // uncomment just one
    //String sourceFolderName("Factory Bank A");
    //String sourceFolderName("Factory Bank B");
    //String sourceFolderName("Factory Electric Himalaya");
    //String sourceFolderName("Factory emptyvessel");
    //String sourceFolderName("Factory Saif Sameer");
    String sourceFolderName("Factory TAL");
    //String sourceFolderName("Factory The Unfinished");

    sourceFolder = talPresetsFolder.getChildFile(sourceFolderName);
    libraryName = "Unified - TAL-Pha";
    outputFolder = unifyLibrariesFolder.getChildFile(libraryName).getChildFile("TEMP");
}

std::unique_ptr<XmlElement> Pha_Unifier::makeUnifiedPatch(File& sourcePresetFile)
{
    auto xml = parseXML(sourcePresetFile);
    jassert(xml);
    String tags = xml->getStringAttribute("tags");
    String auth = xml->getStringAttribute("author");
    if (auth == "-") authorName = "TAL Software GmbH";
    else authorName = auth;
    patchComment = "TAL-Pha factory patches by " + authorName;

    auto pXml = xml->getChildByName("programs");
    jassert(pXml);
    pXml = pXml->getChildByName("program");
    jassert(pXml);
    pXml->setAttribute("path", adjustPath(sourcePresetFile.getFullPathName()));
    patchName = pXml->getStringAttribute("programname");

    double volume = pXml->getDoubleAttribute("volume");
    pXml->setAttribute("volume", volumeScaling * volume);

    prefix.clear();
#if 0
    for (auto tp : tagPrefix)
    {
        if (tags.contains(tp.tag))
            prefix = tp.prefix;
    }
    for (auto wp : wordPrefix)
    {
        if (patchName.containsIgnoreCase(wp.word))
            prefix = wp.prefix;
    }
#elif 0
    for (auto pp : prefixPrefix)
    {
        if (patchName.startsWith(pp.inPrefix))
        {
            patchName = patchName.fromFirstOccurrenceOf(pp.inPrefix, false, true);
            prefix = pp.outPrefix;
        }
    }
#else
    String folderName = sourcePresetFile.getParentDirectory().getFileName();
    for (auto fp : folderPrefix)
    {
        if (fp.folder == folderName)
            prefix = fp.prefix;
    }

#endif
    if (prefix.isEmpty())
    {
        prefix = "UNKNOWN";
        DBG(patchName);
    }

    return xml;
}
