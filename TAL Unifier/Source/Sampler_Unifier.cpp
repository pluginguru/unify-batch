#include "Sampler_Unifier.h"

// Uncomment exactly one of these
//#define CLASSIC80S
//#define EMPTYVESSEL
//#define HOLLOWSUN
//#define KRAFTV1
#define KRAFTV2

Sampler_Unifier::Sampler_Unifier()
{
    talPresetsFolder = File("D:\\TAL Sampler Libraries");
    presetFileType = "talsmpl";
    refPatch = File("D:\\PlugInGuru\\Content Development\\Unified TAL Sampler\\BASS - DigiBass.unify");

#ifdef CLASSIC80S
    libraryName = "Unified - TAL-Sampler Classic 80s";
    refPatch = File("D:\\PlugInGuru\\Content Development\\Unified TAL Sampler\\BASS - DigiBass.unify");
    sourceFolder = talPresetsFolder.getChildFile("Factory Presets\\Classic 80s");
    authorName = "TAL Software GmbH";
    patchComment = "TAL-Sampler \"Classic 80s\" factory patches";
    volumeScaling = 0.8;
#endif

#ifdef EMPTYVESSEL
    libraryName = "Unified - TAL-Sampler EmptyVessel";
    sourceFolder = talPresetsFolder.getChildFile("Factory Presets\\EmptyVessel");
    authorName = "Greg Cole";
    patchComment = "TAL Sampler factory patches by Greg Cole | EmptyVessel";
    volumeScaling = 0.75;
#endif

#ifdef HOLLOWSUN
    libraryName = "Unified - TAL-Sampler Hollowsun";
    sourceFolder = talPresetsFolder.getChildFile("Hollowsun");
    authorName = "Stephen Howell";
    patchComment = "Unified patches for TAL Sampler by Stephen Howell | Hollowsun";
    volumeScaling = 0.8;
#endif

#ifdef KRAFTV1
    libraryName = "Unified - EK Digital Collection Vol.1";
    sourceFolder = talPresetsFolder.getChildFile("Espen Kraft Digital Collection Vol1 TAL Sample Pack");
    authorName = "Espen Kraft";
    patchComment = "Unified patches from Espen Kraft's Digital Collection Vol.1";
    volumeScaling = 0.75;
#endif

#ifdef KRAFTV2
    libraryName = "Unified - EK Digital Collection Vol.2";
    sourceFolder = talPresetsFolder.getChildFile("Espen Kraft Digital Collection Vol2 TAL Sample Pack Korg Poly61 and Poly800");
    authorName = "Espen Kraft";
    patchComment = "Unified patches from Espen Kraft's Digital Collection Vol.2";
    volumeScaling = 0.75;
#endif

    outputFolder = unifyLibrariesFolder.getChildFile(libraryName).getChildFile("Patches");
}

#if defined(HOLLOWSUN) || defined(EMPTYVESSEL)
static struct
{
    String folder;
    String prefix;
}
folderPrefix[] =
{
#ifdef HOLLOWSUN
    { "50s Sci-Fi", "SFX" },
    { "Ace Tone", "DRUM KIT" },
    { "Akai", "DRUM KIT" },
    { "Alesis", "DRUM KIT" },
    { "ARP", "" },  // blank 2nd item means try wordPrefix lookup
    { "Boss", "DRUM KIT" },
    { "C64 Drums", "DRUM KIT" },
    { "Casio", "" },
    { "Chapman Stick", "BASS" },
    { "Clavia Nord Modular", "DRUM KIT" },
    { "Crumar Performer", "STR" },
    { "Cymbals", "PERC" },
    { "E-Mu", "" },
    { "Elka Synthex", "STR" },
    { "Eminent 310", "STR" },
    { "Ensoniq SQ80", "" },
    { "Basses", "BASS" },   // Fairlight
    { "Mallets", "PERC" },  // Fairlight
    { "Others", "SFX" },    // Fairlight
    { "Strings", "STR" },   // Fairlight
    { "Winds", "WIND" },    // Fairlight
    { "Farfisa", "ORG" },
    { "Fender Rhodes", "KEY" },
    { "Fricke", "DRUM KIT" },
    { "Hammond", "ORG" },
    { "Clavinet", "KEY" },  // Hohner
    { "Pianet", "KEY" },    // Hohner
    { "Jen Bass Pedal", "BASS" },
    { "Kawai", "KEY" },
    { "Analogue", "" },     // Korg
    { "Digital", "" },      // Korg
    { "Linn", "DRUM KIT" },
    { "Mattel Synsonic", "DRUM KIT" },
    { "Mellotron and Chamberlin", "" },
    { "Modulus Monowave", "" },
    { "Moog", "" },
    { "MXR185 Drum Computer", "DRUM KIT" },
    { "Novation", "" },
    { "Oberheim DMX", "DRUM KIT" },
    { "Pearl", "DRUM KIT" },
    { "PPG Wave", "" },
    { "RMI Electrapiano", "KEY" },
    { "Analog Synths", "SYNTH" },       // Roland
    { "Digital Synths", "SYNTH" },      // Roland
    { "Drum Machines", "DRUM KIT" },    // Roland
    { "Sakata DMP48", "DRUM KIT" },
    { "Sequential Circuits", "" },
    { "Simmons", "DRUM KIT" },
    { "Sound Master SR88", "DRUM KIT" },
    { "Soundtech Rhythm 12", "KEY" },
    { "Stylophone", "SYNTH" },
    { "Theremin", "SYNTH" },
    { "Vako Orchestron", "" },
    { "Wurlitzer", "KEY" },
    { "CP70", "KEY" },          // Yamaha
    { "CS5", "SYNTH" },         // Yamaha
    { "CS80", "" },             // Yamaha
    { "DX7 Classic", "SYNTH" }, // Yamaha
    { "DX7II", "SYNTH" },       // Yamaha
    { "FS1R", "SYNTH" },        // Yamaha
    { "RX11", "DRUM KIT" },     // Yamaha
#endif

#ifdef EMPTYVESSEL
    { "Bass", "BASS" },
    { "Chords_Stabs", "CHORD" },
    { "Drums", "DRUM" },
    { "FX", "SFX" },
    { "Lead", "LEAD" },
    { "Pad", "PAD" },
    { "RhythmicLoops", "LOOP" },
#endif
};
#endif

#ifdef CLASSIC80S
static struct
{
    String inPrefix;
    String outPrefix;
}
prefixPrefix[] =
{
    { "BL_", "BELL" },
    { "BR_", "BRASS" },
    { "BS_", "BASS" },
    { "CH_", "VOX" },
    { "DR_", "DRUM KIT" },
    { "FL_", "FLUTE" },
    { "OH_", "HIT" },
    { "PC_", "PERC" },
    { "PD_", "PAD" },
    { "PN_", "KEY" },
    { "SY_", "SYNTH" },
};
#endif

#if defined(KRAFTV1) || defined(KRAFTV2) || defined(HOLLOWSUN)
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
    { "Synth", "SYNTH" },
    { "Pad", "PAD" },
    { "Bass", "BASS" },
    { "Slap", "BASS" },
    { "Brass", "BRASS" },
    { "String", "STR" },
    { "Bell", "BELL" },
    { "Choir", "VOX" },
    { "Vox", "VOX" },
    { "Kalimba", "PLUCK" },
    { "Harp", "PLUCK" },

    // Hollowsun
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
#endif

std::unique_ptr<XmlElement> Sampler_Unifier::makeUnifiedPatch(File& sourcePresetFile)
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
#if defined(HOLLOWSUN) || defined(EMPTYVESSEL)
    itemFolder = item.getParentDirectory();
    for (auto fp : folderPrefix)
    {
        if (fp.folder == item.getParentDirectory().getFileName())
            prefix = fp.prefix;
    }
#endif
#if defined(CLASSIC80S)
    for (auto fp : prefixPrefix)
    {
        if (patchName.startsWith(fp.inPrefix))
            prefix = fp.outPrefix;
    }
    patchName = patchName.substring(3);
#endif
#if defined(HOLLOWSUN) || defined(KRAFTV1) || defined(KRAFTV2)
    if (prefix.isEmpty())
    {
        for (auto fp : wordPrefix)
        {
            if (patchName.contains(fp.word))
                prefix = fp.prefix;
        }
    }
#endif
    if (prefix.isEmpty()) prefix = "UNKNOWN";

#ifdef HOLLOWSUN
    patchTags = item.getParentDirectory().getFileName();
#endif

    for (int i = 0; i < 4; i++)
    {
        auto pSmp = pXml->getChildByName("samplelayer" + String(i));
        jassert(pSmp);
        pSmp = pSmp->getChildByName("multisamples");
        jassert(pSmp);
        for (auto* pms : pSmp->getChildWithTagNameIterator("multisample"))
        {
            String url = pms->getStringAttribute("url");
            if (url.isEmpty() || !url.containsChar('.')) break;
            File sampleFile = sourcePresetFile.getParentDirectory().getChildFile(url);
            pms->setAttribute("url", adjustPath(sampleFile.getFullPathName()));
            pms->setAttribute("urlRelativeToPresetDirectory", adjustPath(sampleFile.getRelativePathFrom(talPresetsFolder)));
        }
    }

    return xml;
}
