#pragma once
#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::Unify_patch_xml);

    //test();
    //reveng();
}

void PatchConverter::test()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\GForce XML Unifiers\\Assets");

#ifdef PLUGIN_IS_BABYLON2
    MemoryBlock mb;
    File patchDir("E:\\PlugInGuru\\Unify\\Libraries\\Unified - WAP Babylon 2\\Patches");
    patchDir.getChildFile("BA Sine x Voice + Sq Sub 50.unify").loadFileAsData(mb);
   auto patchXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    auto layerXml = patchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    mb.reset();
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    vst3stateXml->writeTo(assetsDir.getChildFile("vst3state.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    assetsDir.getChildFile("ic.txt").replaceWithData(mb.getData(), mb.getSize());

    const char* start = (const char*)(mb.getData()) + BinaryData::head_binSize;
    size_t length = mb.getSize() - BinaryData::head_binSize - BinaryData::tail_binSize;
    assetsDir.getChildFile("Plugin State.xml").replaceWithData(start, length);

    String presetXmlStr(start, length);
    auto presetXml = parseXML(presetXmlStr);
    XmlElement::TextFormat format;
    auto singleLineFormat = format.singleLine();
    presetXml->writeTo(assetsDir.getChildFile("Plugin State short.xml"), singleLineFormat);
#else
    auto patchXml = parseXML(File(assetsDir).getChildFile("Unify Patch.xml"));

    // tear the sample patch apart to create the plugin-state XML file
    auto layerXml = patchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    vst3stateXml->writeTo(assetsDir.getChildFile("vst3state.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    assetsDir.getChildFile("ic.txt").replaceWithData(mb.getData(), mb.getSize());

    // the state XML file is only needed to observe how it differs from preset XML files
    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    presetXml->writeTo(assetsDir.getChildFile("Plugin State.xml"));
#endif
}

void PatchConverter::reveng()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\GForce XML Unifiers\\Assets");

    File sourceDir("C:\\Users\\owner\\AppData\\Roaming\\WAProduction\\Babylon 2\\WaveTables\\Single\\Factory - Tools 1");
    auto wtXml = parseXML(sourceDir.getChildFile("LFO_Mod_01.bws"));

    String wtData = wtXml->getStringAttribute("WT_Data");
    MemoryBlock mb;
    mb.fromBase64Encoding(wtData);

    assetsDir.getChildFile("WaveTable.dat").replaceWithData(mb.getData(), mb.getSize());
}

int PatchConverter::processFiles(const StringArray& filePaths)
{
#ifdef CHECK_CATEGORIES
    categories.clear();
#endif

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

#ifdef CHECK_CATEGORIES
    categories.sortNatural();
    for (auto c : categories) DBG(c);
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
#ifdef PLUGIN_IS_BABYLON2
    else if (file.getFileExtension() == ".bab")
#else
    else if (file.getFileExtension() == ".xml")
#endif
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
#ifndef CHECK_CATEGORIES
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);
#endif

        fileCount++;
    }
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    auto presetXml = parseXML(inFile);
#ifdef PLUGIN_IS_BABYLON2
    String presetName = presetXml->getStringAttribute("PresetName");

  #if 0 // all this turned out to be unnecessary once I got all 3 counts in the head right
    presetXml->setAttribute("Version", "2.2.0");
    presetXml->setAttribute("BuildNum", "241224");

    auto wtXml = presetXml->getChildByName("WaveTableEditor_1");
    if (wtXml && !(wtXml->hasAttribute("CurntPage")))
    {
        String modeAttr = wtXml->getStringAttribute("Mode");
        String numTblAttr = wtXml->getStringAttribute("NumTbl");
        String curntTableAttr = wtXml->getStringAttribute("CurntTable");
        String viewAbAttr = wtXml->getStringAttribute("ViewAB");
        String linkMultiAttr = wtXml->getStringAttribute("LinkMulti");
        wtXml->removeAllAttributes();
        wtXml->setAttribute("Mode", modeAttr);
        wtXml->setAttribute("NumTbl", numTblAttr);
        wtXml->setAttribute("CurntTable", curntTableAttr);
        wtXml->setAttribute("CurntPage", "0");
        wtXml->setAttribute("ViewAB", viewAbAttr);
        wtXml->setAttribute("LinkMulti", linkMultiAttr);
    }
    wtXml = presetXml->getChildByName("WaveTableEditor_2");
    if (wtXml && !(wtXml->hasAttribute("CurntPage")))
    {
        String modeAttr = wtXml->getStringAttribute("Mode");
        String numTblAttr = wtXml->getStringAttribute("NumTbl");
        String curntTableAttr = wtXml->getStringAttribute("CurntTable");
        String viewAbAttr = wtXml->getStringAttribute("ViewAB");
        String linkMultiAttr = wtXml->getStringAttribute("LinkMulti");
        wtXml->removeAllAttributes();
        wtXml->setAttribute("Mode", modeAttr);
        wtXml->setAttribute("NumTbl", numTblAttr);
        wtXml->setAttribute("CurntTable", curntTableAttr);
        wtXml->setAttribute("CurntPage", "0");
        wtXml->setAttribute("ViewAB", viewAbAttr);
        wtXml->setAttribute("LinkMulti", linkMultiAttr);
    }
    wtXml = presetXml->getChildByName("Filter");
    if (wtXml && !(wtXml->hasAttribute("F_FXOrder")))
        wtXml->setAttribute("F_FXOrder", "1");
  #endif
#else
    presetXml->setTagName("STATE");

    auto metadataXml = presetXml->getChildByName("metadata");
#if defined(PLUGIN_IS_IMPOSCAR3) || defined(PLUGIN_IS_OBONE) || defined(PLUGIN_IS_BASSSTATION) || defined(PLUGIN_IS_TVSPRO)
    metadataXml->setAttribute("path", inFile.getFullPathName());
#endif

    String presetName = metadataXml->getStringAttribute("name");
    String author = metadataXml->getStringAttribute("author");
    String collection = metadataXml->getStringAttribute("collection");
    String notes = metadataXml->getStringAttribute("notes") + "\nCollection: " + collection;
#endif

#ifdef CHECK_CATEGORIES
  #ifdef PLUGIN_IS_BABYLON2
    categories.addIfNotAlreadyThere(StringArray::fromTokens(presetName, false)[0]);
  #else
    categories.addIfNotAlreadyThere(metadataXml->getStringAttribute("category"), true);
  #endif
    return nullptr;
#endif

    String category, prefix;
#ifdef PLUGIN_IS_BABYLON2
    StringArray words = StringArray::fromTokens(presetName, false);
    category = words[0];
    if (category == "EXP1" || category == "XTD") category = words[1];
    getCategoryAndPrefix(category, category, prefix);
    newPatchNameOrErrorMessage = presetName;
#else
    getCategoryAndPrefix(metadataXml->getStringAttribute("category"), category, prefix);
    newPatchNameOrErrorMessage = prefix + " - " + presetName;
#endif

#ifndef PLUGIN_IS_BABYLON2
    String types = metadataXml->getStringAttribute("types");
    String timbres = metadataXml->getStringAttribute("timbres");
    String tags = (types + "," + timbres)
        .trimCharactersAtEnd(",").trimCharactersAtStart(",")
        .replace(",,", ",");
#endif

#if defined(PLUGIN_IS_IMPOSCAR3) || defined(PLUGIN_IS_OBONE)
    // at this point, presetXml is the state XML
    // convert state XML to binary and convert to base64
    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*presetXml, mb);
    auto b64state = mb.toBase64Encoding();
#elif defined(PLUGIN_IS_TVSPRO)
    auto pdXml = new XmlElement("processor_data");
    pdXml->setAttribute("ReverbPreset", "0.0");
    pdXml->setAttribute("ReverbLock", "0");
    pdXml->setAttribute("DelayLock", "0");
    pdXml->setAttribute("PhasorLock", "0");
    pdXml->setAttribute("BendRangeLock", "0");
    pdXml->setAttribute("KeyboardModeLock", "0");
    pdXml->setAttribute("SemALock", "0");
    pdXml->setAttribute("SemBLock", "0");
    pdXml->setAttribute("SequencerALock", "0");
    pdXml->setAttribute("SequencerBLock", "0");
    pdXml->setAttribute("sequencerZoom", "0");
    pdXml->setAttribute("SemAFlipped", "0");
    pdXml->setAttribute("SemBFlipped", "0");
    pdXml->setAttribute("SemGroupOffset", "0");
    pdXml->setAttribute("SeqGroupOffset", "0");
    pdXml->setAttribute("isBrowserVisible", "0");
    pdXml->setAttribute("GlobalTempo", "120.0");
    pdXml->setAttribute("ReferenceTune", "440.0");
    presetXml->addChildElement(pdXml);

    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*presetXml, mb);
    mb.append(BinaryData::tail_bin, BinaryData::tail_binSize);
    auto b64state = mb.toBase64Encoding();
#elif defined(PLUGIN_IS_AXXESS)
    auto pdXml = new XmlElement("processor_data");
    pdXml->setAttribute("GlobalTempo", "120.0");
    pdXml->setAttribute("isExtended", "1");
    pdXml->setAttribute("isBrowserVisible", "0");
    pdXml->setAttribute("ReverbLock", "0");
    pdXml->setAttribute("DelayLock", "0");
    pdXml->setAttribute("DistortionLock", "0");
    pdXml->setAttribute("bendRangeLock", "0");
    pdXml->setAttribute("ReverbPreset", "0");
    pdXml->setAttribute("SequencerSliderView", "0");
    pdXml->setAttribute("SequencerRecord", "0");
    pdXml->setAttribute("SequencerPlayMode", "0");
    pdXml->setAttribute("KeyboardHold", "0");
    presetXml->addChildElement(pdXml);

    MemoryBlock encodedXml;
    AudioProcessor::copyXmlToBinary(*presetXml, encodedXml);
    
    MemoryBlock mb(BinaryData::head_bin, BinaryData::head_binSize);
    int byteCount = encodedXml.getSize() + BinaryData::tail_binSize;
    uint8* bp = ((uint8*)mb.getData()) + 174;
    *bp++ = (byteCount >> 8) & 0xFF;
    *bp++ = byteCount & 0xFF;
    byteCount += 152;
    bp = ((uint8*)mb.getData()) + 22;
    *bp++ = (byteCount >> 8) & 0xFF;
    *bp++ = byteCount & 0xFF;

    mb.append(encodedXml.getData(), encodedXml.getSize());
    mb.append(BinaryData::tail_bin, BinaryData::tail_binSize);
    auto b64state = mb.toBase64Encoding();
#elif defined(PLUGIN_IS_BASSSTATION)
    auto pdXml = new XmlElement("perInstanceSettings");
    pdXml->setAttribute("referenceToneState", "0");
    pdXml->setAttribute("referenceToneTuning", "440.0");
    pdXml->setAttribute("globalTempo", "120.0");
    pdXml->setAttribute("keySeqView", "0");
    pdXml->setAttribute("isBrowserVisible", "0");
    pdXml->setAttribute("oscilloscopeVisible", "0");
    pdXml->setAttribute("selectedParamId", "OscsMixerLevel");
    pdXml->setAttribute("selectedXlfoParamId", "LfoDepth");
    pdXml->setAttribute("selectedXadsrParamId", "EnvAmp");
    pdXml->setAttribute("coronasAreVisible", "0");
    pdXml->setAttribute("seqLock", "0");
    pdXml->setAttribute("seqNoteMacroView", "0");
    pdXml->setAttribute("hpfLock", "0");
    pdXml->setAttribute("distortionLock", "0");
    pdXml->setAttribute("chorusLock", "0");
    pdXml->setAttribute("delayLock", "0");
    pdXml->setAttribute("reverbLock", "0");
    pdXml->setAttribute("pitchBendRangeLock", "0");
    presetXml->addChildElement(pdXml);

    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*presetXml, mb);
    auto b64state = mb.toBase64Encoding();
#elif defined(PLUGIN_IS_BABYLON2)
    // for Babylon2 we use the preset XML, but it must be shortened to a single line
    XmlElement::TextFormat format;
    auto singleLineFormat = format.singleLine();
    String presetXmlStr = presetXml->toString(singleLineFormat);
    MemoryBlock shortXml(presetXmlStr.toRawUTF8(), presetXmlStr.getNumBytesAsUTF8());

    // start with binary header
    MemoryBlock mb(BinaryData::head_bin, BinaryData::head_binSize);

    // 4-byte, big-endian total byte count at offset 0x14, of bytes from 0x18 to end
    union { uint32 u; uint8 b[4]; } bc;
    bc.u = BinaryData::head_binSize + shortXml.getSize() + BinaryData::tail_binSize - 0x18;
    uint8* bp = ((uint8*)mb.getData()) + 0x14;
    *bp++ = bc.b[3];
    *bp++ = bc.b[2];
    *bp++ = bc.b[1];
    *bp++ = bc.b[0];

    // same thing minus 98 at offset 0xAC
    bc.u -= 0x98;
    bp = ((uint8*)mb.getData()) + 0xAC;
    *bp++ = bc.b[3];
    *bp++ = bc.b[2];
    *bp++ = bc.b[1];
    *bp++ = bc.b[0];

    // 4-byte, little-endian byte count of just XML data at offset 0xB4
    bp = ((uint8*)mb.getData()) + 0xB4;
    *((uint32*)bp) = (uint32)(shortXml.getSize());

    // follow with plaintext preset XML
    mb.append(shortXml.getData(), shortXml.getSize());

    // finally the binary tail
    mb.append(BinaryData::tail_bin, BinaryData::tail_binSize);

    // base64-encode the lot
    auto b64state = mb.toBase64Encoding();
#endif

    //MemoryBlock mb;
    //String b64state;

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, mb);
    b64state = mb.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", newPatchNameOrErrorMessage);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
#ifdef PLUGIN_IS_BABYLON2
    pmXml->setAttribute("author", "WA Production");
    pmXml->setAttribute("comment", "Factory presets by WA Production");
#else
    pmXml->setAttribute("author", author.isNotEmpty() ? author : "GForce Software");
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", notes.isNotEmpty() ? notes : "Factory presets by GForce Software");
#endif
    pmXml->setAttribute("library", libraryName);

    return patchXml;
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

#ifdef PLUGIN_IS_BABYLON2
static struct { String catFromPreset, category, prefix; } categoryTable[] =
{
    { "AR", "Arpeggio", "BPM ARP" },
    { "AT", "Atmosphere", "ATMO" },
    { "BA", "Bass", "BASS" },
    { "BS", "Bass", "BASS" },
    { "CH", "Chord", "CHORD" },
    { "CN", "Cinematic", "CINE" },
    { "DR", "Drum", "DRUM" },
    { "FX", "SFX", "SFX" },
    { "KY", "Keyboard", "KEY" },
    { "LD", "Lead", "LEAD" },
    { "OR", "Organ", "ORGAN" },
    { "ORN", "Oriental", "ORIENT" },
    { "PAD", "Pad", "PAD" },
    { "PD", "Pad", "PAD" },
    { "PL", "Pluck", "PLUCK" },
    { "SQ", "Sequence", "BPM SEQ" },
    { "SY", "Synth", "SYNTH" },
    { "00", "Template", "_INIT" },
};
#else
static struct { String catFromPreset, category, prefix; } categoryTable[] =
{
    { "Arpegs", "Arpeggio", "BPM ARP" },
    { "Sequences", "Sequence", "BPM SEQ" },
    { "Textures", "Texture", "TEXT" },
    { "Basses", "Bass", "BASS" },
    { "Bells", "Bell", "BELL" },
    { "Chords", "Chord", "CHORD" },
    { "Effects", "SFX", "SFX" },
    { "Keys", "Keyboard", "KEY" },
    { "Leads", "Lead", "LEAD" },
    { "Organ", "Organ", "ORGAN" },
    { "Pads", "Pad", "PAD" },
    { "Hits", "Drum", "DRUM" },
    { "Sequence & Bass", "Sequence", "BPM BASS" },
    { "Sequence & Lead", "Sequence", "BPM LEAD" },
    { "Sequence & Pad", "Sequence", "BPM PAD" },
    { "Sequences: Dual", "Sequence", "BPM SEQ" },
    { "Sequences: Mono", "Sequence", "BPM SEQ" },
    { "Splits", "Sequence", "SPLIT" },
};
#endif

void PatchConverter::getCategoryAndPrefix(String categoryFromPreset, String& category, String& prefix)
{
#ifdef PLUGIN_IS_TVSPRO
    if (categoryFromPreset.isEmpty())
    {
        category = "Template";
        prefix = "TEMPLATE";
        return;
    }
#endif

    for (auto& cte : categoryTable)
    {
        if (cte.catFromPreset == categoryFromPreset)
        {
            category = cte.category;
            prefix = cte.prefix;
            return;
        }
    }

    category = "";
    prefix = "UNKNOWN";
}
