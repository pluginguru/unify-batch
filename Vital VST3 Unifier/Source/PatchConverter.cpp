#pragma once
#include "PatchConverter.h"
#include "MyVstUtils.h"

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::Unify_patch_xml);

    //test();
}

void PatchConverter::test()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\Vital VST3 Unifier\\Assets");

#if 1
    // tear the sample patch apart to create the plugin-state XML file
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    //assetsDir.getChildFile("vst3state.xml").create();
    //vst3stateXml->writeTo(assetsDir.getChildFile("vst3state.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    //assetsDir.getChildFile("ic.txt").create();
    //assetsDir.getChildFile("ic.txt").replaceWithData(mb.getData(), mb.getSize());
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
    else if (file.getFileExtension() == ".vital")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());

        fileCount++;
    }
}

static char first16bytes[] = "VstW\x00\x00\x00\x08\x00\x00\x00\x01\x00\x00\x00\x00";
static String tuningJson(",\"tuning\":{\"default\":true,\"mapping_name\":\"\",\"reference_midi_note\":0.0,\"scale\":[0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0],\"scale_start_midi_note\":60,\"tuning_name\":\"\"}");

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    // load patch JSON and tack on the "tuning" tags at the end
    String patchJson = inFile.loadFileAsString().trimEnd().dropLastCharacters(1) + tuningJson + "}";

    // parse out some metadata
    var json = JSON::parse(patchJson);
    String presetAuthor = json.getProperty("author", "Vital Audio").toString();
    String presetComments = json.getProperty("comments", "").toString();
    String presetName = json.getProperty("preset_name", inFile.getFileNameWithoutExtension()).toString();
    String presetStyle = json.getProperty("preset_style", "").toString();
    String category, prefix;
    getCategoryAndPrefix(presetStyle, category, prefix);
    newPatchNameOrErrorMessage = prefix + " - " + presetName;

    // put patch JSON into a MemoryBlock
    MemoryBlock mb(patchJson.getCharPointer(), patchJson.getNumBytesAsUTF8());

    // wrap in a VST "FBCh" chunk in mb
    MyVstConv vstConv;
    vstConv.setChunkData(patchJson.getCharPointer(), patchJson.getNumBytesAsUTF8(), true);
    vstConv.saveToFXBFile(mb, true);

    // prepend Vital's 16-byte binary header and convert to base64
    MemoryBlock mb2(first16bytes, 16);
    mb2.setSize(16 + mb.getSize());
    mb2.copyFrom(mb.getData(), 16, mb.getSize());
    String b64state = mb2.toBase64Encoding();

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
    layerXml->setAttribute("layerTitle", presetName);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    pmXml->setAttribute("author", presetAuthor.isNotEmpty() ? presetAuthor : "Vital Audio");
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("comment", presetComments.isNotEmpty() ? presetComments : "Factory presets by Vital Audio");
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

static struct { String presetStyle, category, prefix; } categoryTable[] =
{
    // Not yet used. If you want better categories than are encoded in Vital's "preset_style" attribute,
    // you'll need to analyze all the attribute values actually used in your preset collection and define
    // appropriate Unify category/prefix strings for each of them here. The following are example entries
    // used for a completely different plug-in.

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
};

void PatchConverter::getCategoryAndPrefix(String presetStyle, String& category, String& prefix)
{
    // Activate this code only after you have prepared a complete categoryTable[] above.
#if 0
    for (auto& cte : categoryTable)
    {
        if (cte.presetStyle == presetStyle)
        {
            category = cte.category;
            prefix = cte.prefix;
            return;
        }
    }
#endif

    category = presetStyle;
    prefix = presetStyle.isEmpty() ? "UNKNOWN" : presetStyle.toUpperCase();
}
