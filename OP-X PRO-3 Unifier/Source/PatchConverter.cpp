#pragma once
#include "PatchConverter.h"

static char jucePrivateData[] =
"\x00\x00\x00\x00\x00\x00\x00\x00"
"JUCEPrivateData\x00\x01\x01"
"Bypass\x00\x01\x01\x03\x00\x1d\x00\x00\x00\x00\x00\x00\x00"
"JUCEPrivateData";

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::One_OPX_PRO3_Layer_xml);

#if 1
    String stateInfo = unifyPatchXml->getChildByName("Layer")
        ->getChildByName("Instrument")->getStringAttribute("stateInformation");

    MemoryBlock memBlock;
    memBlock.fromBase64Encoding(stateInfo);
    //File outFile("E:\\PlugInGuru\\Unify\\Presets\\Plugin States\\OPX Test.bin");
    //outFile.create();
    //outFile.replaceWithData(memBlock.getData(), memBlock.getSize());

    {
        String sdata((char*)(memBlock.getData()) + 8, int(memBlock.getSize()) - 9);
        auto vst3stateXml = parseXML(sdata);
        String iComponentDataStr = vst3stateXml->getChildByName("IComponent")->getAllSubText();
        memBlock.fromBase64Encoding(iComponentDataStr);
    }

    {
        String sdata((char*)(memBlock.getData()) + 4, int(memBlock.getSize()) - 4);
        auto opxStateXml = parseXML(sdata);
        File outFile("E:\\PlugInGuru\\Unify\\Presets\\Plugin States\\OPX Test.xml");
        opxStateXml->writeTo(outFile);
    }
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
        processFile("None", file, fileCount);
    }
    return fileCount;
}

void PatchConverter::processFile(String folderName, File file, int& fileCount)
{
    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFile(file.getFileName(), entry.getFile(), fileCount);
    }
    else if (file.getFileExtension() == ".taqsimpreset")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processSoloPreset(folderName, file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
}

XmlElement* PatchConverter::processSoloPreset(String folderName, File inFile, String& newPatchNameOrErrorMessage)
{
    String basicPatchName = inFile.getFileNameWithoutExtension();
    newPatchNameOrErrorMessage = folderName.toUpperCase() + " - " + basicPatchName;

    // take the input XML string, append the jucePrivateData, and encode as base64
    MemoryBlock memBlock;
    inFile.loadFileAsData(memBlock);
    memBlock.append(jucePrivateData, sizeof(jucePrivateData) - 1);
    String b64state = memBlock.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new SOLO binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, memBlock);
    b64state = memBlock.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", basicPatchName);
    auto instXml = layerXml->getChildByName("Instrument");
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    //pmXml->setAttribute("author", author);
    pmXml->setAttribute("category", folderName);
    pmXml->setAttribute("tags", folderName);
    //pmXml->setAttribute("comment", comment);

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
