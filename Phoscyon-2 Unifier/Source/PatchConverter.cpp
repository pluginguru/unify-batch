#pragma once
#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::Best_Served_with_Beat_xml);
    phoscyon2stateXml = parseXML(BinaryData::Phoscyon2state_xml);

    //test();
}

void PatchConverter::test()
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\Phoscyon-2 Unifier\\Assets");

#if 0
    // tear the sample patch apart to create the Phoscyon2-state.xml plugin-state XML file
    auto layerXml = unifyPatchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    MemoryBlock mb;
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    //vst3stateXml->writeTo(assetsDir.getChildFile("vst3state.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    //File icDataFile = assetsDir.getChildFile("ic.txt");
    //icDataFile.replaceWithData(mb.getData(), mb.getSize());

    auto presetXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    presetXml->writeTo(assetsDir.getChildFile("Phoscyon2-state.xml"));
#endif

#if 1
    // test modifying the plugin-state XML to match a specific factory preset
    File inFile = assetsDir.getChildFile("Scene Presets").getChildFile("Best Served with Beat.ph2scn");
    String presetName = inFile.getFileNameWithoutExtension();
    auto presetXml = parseXML(inFile);

    // add name attribute to preset XML
    presetXml->setAttribute("name", presetName);

    // get a fresh copy of phoscyon2stateXml and swap in the modified preset XML
    std::unique_ptr<XmlElement> stateXml(new XmlElement(*phoscyon2stateXml));
    auto psXml = stateXml->getChildByName("ParametersState");
    psXml->replaceChildElement(psXml->getChildByName("Preset"), presetXml.release());

    stateXml->writeTo(assetsDir.getChildFile("scene.xml"));
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
    else if (file.getFileExtension() == ".ph2scn")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processPresetFile(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
#if 1
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);
#endif

        fileCount++;
    }
}

XmlElement* PatchConverter::processPresetFile(File inFile, String& newPatchNameOrErrorMessage)
{
    String presetName = inFile.getFileNameWithoutExtension();

    auto presetXml = parseXML(inFile);

    String author = presetXml->getStringAttribute("author");
    String tags = presetXml->getStringAttribute("tags").replace("|", ", ");
    jassert(!(tags.containsIgnoreCase("Arp") && tags.containsIgnoreCase("Seq")));

    // load preset file as text and parse the metadata
    String prefix = "BPM BASS";
    String category = "Bass";

    if (tags.containsIgnoreCase("Arp"))
    {
        prefix = "BPM ARP";
        category = "BPM, Arpeggiator, Bass";
    }
    else if (tags.containsIgnoreCase("Seq"))
    {
        prefix = "BPM SEQ";
        category = "BPM, Sequence, Bass";
    }
    newPatchNameOrErrorMessage = prefix + " - " + presetName;

    // add name attribute to preset XML
    presetXml->setAttribute("name", presetName);

    // get a fresh copy of phoscyon2stateXml and swap in the modified preset XML
    std::unique_ptr<XmlElement> stateXml(new XmlElement(*phoscyon2stateXml));
    auto psXml = stateXml->getChildByName("ParametersState");
    psXml->replaceChildElement(psXml->getChildByName("Preset"), presetXml.release());

    // convert state XML to binary and convert to base64
    MemoryBlock mb;
    AudioProcessor::copyXmlToBinary(*stateXml, mb);
    auto b64state = mb.toBase64Encoding();

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
    pmXml->setAttribute("author", author.isEmpty() ? "D16 Group" : author);
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", "Factory presets by D16 Group");
    if (libraryName.isNotEmpty()) pmXml->setAttribute("library", libraryName);

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
