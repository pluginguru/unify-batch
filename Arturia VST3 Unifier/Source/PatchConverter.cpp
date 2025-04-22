#pragma once
#include "PatchConverter.h"

static struct
{
    String type;
    String subtype;
    String prefix;
}
typeSubtypePrefix[] =
{
    // Factory presets
    { "Keys", "Evolving Keys" , "KEY" },
    { "Bass", "Plucked Bass" , "BASS" },
    { "Pad", "Evolving Pad" , "PAD" },
    { "Keys", "Classic Synth Keys" , "KEY" },
    { "Bass", "Bass Line" , "BASS" },
    { "Sequence", "Melodic Sequence" , "BPM SEQ" },
    { "Sound Effects", "Noise" , "SFX" },
    { "Pad", "Atmosphere" , "ATMO" },
    { "Pad", "Classic Synth Pad" , "PAD" },
    { "Bass", "Hard Bass" , "BASS" },
    { "Sequence", "Arpeggio" , "BPM ARP" },
    { "Synth Lead", "Soft Lead" , "LEAD" },
    { "Synth Lead", "Hard Lead" , "LEAD" },
    { "Sound Effects", "Machines" , "SFX" },
    { "Keys", "Lofi Keys" , "KEY" },
    { "Sound Effects", "Ambient SFX" , "AMB" },
    { "Synth Lead", "Poly Lead" , "LEAD" },
    { "Keys", "Plucked Keys" , "PLUCK" },
    { "Sound Effects", "Nature Sounds" , "AMB" },
    { "Sequence", "Noise Sequence" , "BPM SEQ" },
    { "Synth Lead", "Solo Lead" , "LEAD" },
    { "Sound Effects", "Sweeps" , "SWEEP" },
    { "Pad", "Bright Pad" , "PAD" },
    { "Brass & Winds", "Synth Brass" , "BRASS" },
    { "Organ", "Synth Organ" , "ORGAN" },
    { "Bass", "Sub Bass" , "BASS" },
    { "Strings", "Bowed Strings" , "STRING" },
    { "Vocal", "Synth Choir" , "CHOIR" },
    { "Strings", "String Ensemble" , "STRING" },
    { "Sequence", "Rhythmic Sequence" , "BPM SEQ" },
    { "Drums", "Clap" , "DRUM" },
    { "Drums", "Tom" , "DRUM" },
    { "Template", "" , "TEMPLATE" },
    { "Strings", "Plucked Strings" , "STRING" },
    { "Brass & Winds", "Orchestral Brass" , "BRASS" },
    { "Keys", "Percussive Keys" , "KEY" },
    { "Keys", "Bells" , "BELL" },
    { "Drums", "Kick" , "DRUM" },
    { "Strings", "Guitar" , "GTR" },
    { "Pad", "Choir" , "CHOIR" },
    { "Synth Lead", "Plucked Lead" , "LEAD" },
    { "Brass & Winds", "Woodwind" , "WIND" },
    { "Sound Effects", "Soundtrack" , "SFX" },
    { "Synth Lead", "Big Lead" , "LEAD" },
    { "Keys", "Mallets" , "PERC" },
    { "Synth Lead", "Dirty Lead" , "LEAD" },
    { "Drums", "Snare" , "DRUM" },
    { "Drums", "Percussion" , "PERC" },
    { "Synth Lead", "Percussive Lead" , "LEAD" },
    { "Sound Effects", "Creative SFX" , "SFX" },
    { "Sound Effects", "Hit" , "HIT" },
    { "Drums", "Creative Drums" , "DRUM" },
    { "Piano", "Grand Piano" , "KEY" },
    { "Pad", "Strings Pad" , "PAD" },
    { "Piano", "Upright Piano" , "KEY" },
    { "Piano", "Creative Piano" , "KEY" },
    { "Electric Piano", "Creative EP" , "KEY" },
    { "Drums", "Drum Loop" , "BPM DRUM" },
    { "Brass & Winds", "Creative Brass" , "BRASS" },
    { "Lead", "Dirty Lead" , "LEAD" },
    { "Organ", "Tonewheel Organ" , "ORGAN" },
    { "Drums", "Cymbal" , "DRUM" },
    { "Lead", "Percussive Lead" , "LEAD" },
    { "Lead", "Soft Lead" , "LEAD" },
    { "Lead", "Solo Lead" , "LEAD" },
    { "Lead", "Big Lead" , "LEAD" },
    { "Lead", "Synced Lead" , "LEAD" },
    { "Lead", "Hard Lead" , "LEAD" },
    { "Vocal", "Real Choir" , "CHOIR" },
    { "Bass", "Soft Bass" , "BASS" },
    { "Brass & Winds", "Winds Hybrid" , "WIND" },
    { "Lead", "Plucked Lead" , "PLUCK" },
    { "Piano", "Hybrid" , "KEY" },
    { "Lead", "Poly Lead" , "LEAD" },
    { "Vocal", "Vocoder" , "VOX" },
    { "Brass & Winds", "Winds Acoustic" , "WIND" },
    { "Electric Piano", "Digital Piano" , "KEY" },

#if 0
    // SabrePigs v1
    { "Custom", "Ambient Pad" , "AMB" },
    { "Custom", "Polyphonic Sequence" , "BPM SEQ" },
    { "Custom", "Vocal FX" , "VOX" },
    { "Electric Piano", "Classic EP" , "KEY" },
    { "Pad", "Ambient Pad" , "AMB" },
    { "Custom", "Transition" , "SFX" },
#endif

#if 0
    // Tom Wolfe's SynthVault
    { "Keys", "Hard Bass" , "BASS" },
    { "Pad", "Ambient Pad" , "PAD" },
    { "Pad", "Textural Pad" , "PAD" },
    { "Custom", "Ambient Pad" , "PAD" },
    { "Pad", "Granular Pad" , "PAD" },
    { "Pad", "Sampled Texture" , "TEXT" },
    { "Bass", "Bass Sequence" , "BPM SEQ" },
    { "Bass", "Ambient Pad" , "AMB" },
    { "Sequence", "Cinematic Sequence" , "BPM SEQ" },
    { "Electric Piano", "Classic EP" , "KEY" },
    { "Sequence", "Synth Sequence" , "BPM SEQ" },
    { "Sound Effects", "Ambient Pad" , "AMB" },
    { "Sequence", "Bass Sequence" , "BPM SEQ" },
    { "Bass", "Digital Bass" , "BASS" },
    { "Keys", "Textural Keys" , "KEY" },
#endif

#if 1
    // Celestial Resonance
    { "Pad", "Ambient Pad" , "PAD" },
    { "Pad", "Ambient SFX" , "SFX" },
    { "Keys", "Sc-Fi Keys" , "KEY" },
    { "Pad", "Drone" , "PAD" },
    { "Keys", "Sampled Keys" , "KEY" },
    { "Lead", "Classic Synth Keys" , "LEAD" },
    { "Organ", "Space Organ" , "ORGAN" },
    { "Pad", "Soundscape" , "PAD" },
#endif
};

static bool alreadyKnown(const String& type, const String& subtype)
{
    for (auto& entry : typeSubtypePrefix)
    {
        if (entry.type == type && entry.subtype == subtype)
            return true;
    }
    return false;
}

static String getPrefix(const String& type, const String& subtype)
{
    for (auto& entry : typeSubtypePrefix)
    {
        if (entry.type == type && entry.subtype == subtype)
            return entry.prefix;
    }
    if (type.isEmpty() && subtype.isEmpty())
    {
        //DBG("  no type/subtype");
    }
    else
        DBG("  type=\"" + type + "\", subtype=\"" + subtype + "\"");
    return "UNKNOWN";
}

PatchConverter::PatchConverter()
{
    MemoryBlock mb;
    File assetsFolder("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\Arturia VST3 Unifier\\Assets");
    assetsFolder.getChildFile("Unify patch.unify").loadFileAsData(mb);

    unifyPatchXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    //test();
}

void PatchConverter::test()
{
    File assetsFolder("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\U-He VST3 Unifier\\Assets");
#if 1
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    //File outFile = assetsFolder.getChildFile("test.xml");
    //vst3stateXml->writeTo(outFile);

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    File icDataFile = assetsFolder.getChildFile("ic.txt");
    icDataFile.replaceWithData(mb.getData(), mb.getSize());

    auto ecDataString = vst3stateXml->getChildByName("IEditController")->getAllSubText();
    b64ok = mb.fromBase64Encoding(ecDataString);
    File ecDataFile = assetsFolder.getChildFile("ec.txt");
    ecDataFile.replaceWithData(mb.getData(), mb.getSize());
#endif

#if 0
    MemoryBlock mb;
    File unifyPatchFile("E:\\PlugInGuru\\Unify\\Libraries\\Unified - PA Derailer\\Patches\\ATMO - Gazer.unify");
    unifyPatchFile.loadFileAsData(mb);
    auto patchXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    auto layerXml = patchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");
    bool b64ok = mb.fromBase64Encoding(stateInfoString);
    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    auto icXml = vst3stateXml->getChildByName("IComponent");
    auto icDataString = icXml->getAllSubText();
    mb.setSize(0);
    b64ok = mb.fromBase64Encoding(icDataString);
    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    DBG(presetXml->toString());
#endif
}

int PatchConverter::processFiles(const StringArray& filePaths)
{
    if (outputFolderPath.isNotEmpty())
    {
        outputFolder = File(outputFolderPath);
        outputFolder.createDirectory();
    }

    int fileCount = 0;
    for (auto filePath : filePaths)
    {
        File file(filePath);
        processFile(file, fileCount);
    }

#ifdef LIST_TYPES_AND_SUBTYPES
    for (auto tst : typesAndSubtypes)
    {
        String type = tst.upToFirstOccurrenceOf("|", false, false);
        String subtype = tst.fromFirstOccurrenceOf("|", false, false);
        DBG("    { \"" + type + "\", \"" + subtype + "\" , \"PREFIX\" },");
    }
#endif
    return fileCount;
}

void PatchConverter::processFile(File file, int& fileCount)
{
    DBG(file.getFileName());

    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFile(entry.getFile(), fileCount);
    }
    else if (file.getFileExtension().isEmpty())
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        //else
        //    AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
        //        file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
}

static void skip(char* &p, int times = 1)
{
    for (int i = 0; i < times; i++)
    {
        while (*p != ' ') p++;
        while (*p == ' ') p++;
    }
}

static void skipNumbers(char*& p)
{
    char* q = p;
    skip(q);
    while (isdigit(*p) && isdigit(*q))
    {
        if (atoi(p) == 1 && atoi(q) == 0) break;
        p = q;
        skip(q);
    }
}

void PatchConverter::getMetadata(MemoryBlock mb)
{
    String prevItemString;
    String characteristics;

    tags.clear();
    type.clear();
    subtype.clear();
    comment.clear();

    char* p = (char*)(mb.getData());
    skip(p, 2);
    for (int itemCount = 0; itemCount < 50; itemCount++)
    {
        // all items start with a number which may be a character-count or a float
        int floatVal = atof(p);
        int charCount = atoi(p);
        skip(p);

        if (charCount == 0)
        {
            skipNumbers(p);
            prevItemString = "0";   // experimental - for Tom Wolfe's SynthVault
        }
        else if (charCount == 10 && isdigit(*p))
        {
            skip(p, 4);
        }
        else if (charCount == 22 && isdigit(*p))
        {
            // ignore
        }
        else if (charCount < 1000)
        {
            String itemString(p, charCount);
            p += charCount;
            while (*p == ' ') p++;

            if (itemCount == 1)
                presetName = itemString;
            else if (itemCount == 2)
                collection = itemString;
            else if (itemCount == 4)
                author = itemString;

            if (prevItemString == "0" && comment.isEmpty())
                comment = itemString;

            if (prevItemString == "Characteristics")
                characteristics = itemString;
            else if (prevItemString == "Type")
            {
                type = itemString;
                break;
            }
            else if (prevItemString == "Subtype")
                subtype = itemString;

            prevItemString = itemString;
        }
    }

#ifdef LIST_TYPES_AND_SUBTYPES
    if (!alreadyKnown(type, subtype))
    {
        if (typesAndSubtypes.addIfNotAlreadyThere(type + "|" + subtype))
            DBG("   NEW type \"" + type + "\", subtype \"" + subtype + "\"");
    }
#endif

    // parse characteristics string to get tags
    StringArray tagsArray;
    auto parts = StringArray::fromTokens(characteristics, ";", "\"");
    for (auto part : parts)
    {
        String heading = part.upToFirstOccurrenceOf(",", false, false);
        String list = part.fromFirstOccurrenceOf(",", false, false);
        tagsArray.addArray(StringArray::fromTokens(list, "|", "\""));
    }
    tags = tagsArray.joinIntoString(";");
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    String presetName = inFile.getFileNameWithoutExtension();

    MemoryBlock mb;
    inFile.loadFileAsData(mb);
    getMetadata(mb);

    jassert(type.isNotEmpty());

#ifdef LIST_TYPES_AND_SUBTYPES
    return nullptr;
#else

#ifdef DIVINE_PATCH_PREFIXES
    prefix = getPrefix(type, subtype);
    newPatchNameOrErrorMessage = prefix + " - " + presetName;
#else
    newPatchNameOrErrorMessage = presetName;
#endif

    // convert to base64
    auto b64state = mb.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);
    //auto eComponentXml = new XmlElement("IEditController");
    //vst3stateXml->addChildElement(eComponentXml);
    //eComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, mb);
    b64state = mb.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", presetName);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    pmXml->setAttribute("author", author.isEmpty() ? "Arturia" : author);
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", comment);
    if (libraryName.isNotEmpty()) pmXml->setAttribute("library", libraryName);

    return patchXml;
#endif
}

void PatchConverter::saveUnifyPatch(File inFile, String patchName, XmlElement* patchXml)
{
    MemoryBlock outBlock;
    AudioProcessor::copyXmlToBinary(*patchXml, outBlock);

    if (outputFolderPath.isNotEmpty())
    {
        // Save to output folder
        File outFile = File(outputFolderPath).getChildFile(patchName + ".unify");
        outFile.create();
        outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
    }
}
