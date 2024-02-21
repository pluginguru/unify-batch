#pragma once
#include "PatchConverter.h"

static char jucePrivateData[] =
"\x00\x00\x00\x00\x00\x00\x00\x00"
"JUCEPrivateData\x00\x01\x01"
"Bypass\x00\x01\x01\x03\x00\x1d\x00\x00\x00\x00\x00\x00\x00"
"JUCEPrivateData";

static void AnalyzeVST2State()
{
    //auto unifyPatchXml = parseXML(BinaryData::One_VST2_Layer_xml);
    //String stateInfo = unifyPatchXml->getChildByName("Layer")->getChildByName("Instrument")->getStringAttribute("stateInformation");
    //MemoryBlock memBlock;
    //memBlock.fromBase64Encoding(stateInfo);

    File patchFile("E:\\PlugInGuru\\Staging\\Subsonic Artz Dunes of Arrakis V1\\Subsonic Artz Sands of ARRAKIS Unify\\Libraries\\Subsonic Artz Sands of ARRAKIS\\Patches\\BL - Awareness narcotic.unify");
    MemoryBlock memBlock;
    patchFile.loadFileAsData(memBlock);
    auto vst2PatchXml = juce::AudioProcessor::getXmlFromBinary(memBlock.getData(), int(memBlock.getSize()));
    String vst2stateInfo = vst2PatchXml->getChildByName("Layer")->getChildByName("Instrument")->getStringAttribute("stateInformation");
    memBlock.fromBase64Encoding(vst2stateInfo);

    File outFile("E:\\PlugInGuru\\Dune Decoding\\Dune VST2 State Info.bin");
    outFile.create();
    outFile.replaceWithData(memBlock.getData(), memBlock.getSize());
}

static void AnalyzeVST3State()
{
    auto unifyPatchXml = parseXML(BinaryData::One_VST3_Layer_xml);
    String stateInfo = unifyPatchXml->getChildByName("Layer")
        ->getChildByName("Instrument")->getStringAttribute("stateInformation");

    MemoryBlock memBlock;
    memBlock.fromBase64Encoding(stateInfo);
    //File outFile("E:\\PlugInGuru\\Dune Decoding\\Dune VST3 Decoded VST3 state.bin");
    //outFile.create();
    //outFile.replaceWithData(memBlock.getData(), memBlock.getSize());

    {
        String sdata((char*)(memBlock.getData()) + 8, int(memBlock.getSize()) - 9);
        auto vst3stateXml = parseXML(sdata);
        String iComponentDataStr = vst3stateXml->getChildByName("IComponent")->getAllSubText();
        //File outFile("E:\\PlugInGuru\\Dune Decoding\\Dune VST3 IComponent data.bin");
        //outFile.create();
        //outFile.replaceWithText(iComponentDataStr);

        memBlock.fromBase64Encoding(iComponentDataStr);

        //memBlock.reset();
        //MemoryOutputStream ms(memBlock, true);
        //Base64::convertFromBase64(ms, iComponentDataStr);
        //ms.flush();

        File outFile("E:\\PlugInGuru\\Dune Decoding\\Dune VST3 Decoded IComponent data.bin");
        outFile.create();
        outFile.replaceWithData(memBlock.getData(), memBlock.getSize());
    }

    //{
    //    String sdata((char*)(memBlock.getData()) + 4, int(memBlock.getSize()) - 4);
    //    auto opxStateXml = parseXML(sdata);
    //    File outFile("E:\\PlugInGuru\\Unify\\Presets\\Plugin States\\Dune VST3 Test.xml");
    //    opxStateXml->writeTo(outFile);
    //}
}

PatchConverter::PatchConverter()
{
    //AnalyzeVST2State();
    //AnalyzeVST3State();
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
    else if (file.getFileExtension() == ".unify")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(convertVst2PatchToVst3PatchXml(folderName, file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
}

XmlElement* PatchConverter::convertVst2PatchToVst3PatchXml(String folderName, File inFile, String& newPatchNameOrErrorMessage)
{
    // load .unify patch as XML
    MemoryBlock memBlock;
    inFile.loadFileAsData(memBlock);
    auto vst2PatchXml = juce::AudioProcessor::getXmlFromBinary(memBlock.getData(), int(memBlock.getSize()));

    // extract metadata
    auto patchMetadataXml = new XmlElement(*(vst2PatchXml->getChildByName("PresetMetadata")));

    // get patch name
    newPatchNameOrErrorMessage = patchMetadataXml->getStringAttribute("name");

    // extract INST1 state data
    String vst2stateInfo = vst2PatchXml->getChildByName("Layer")->getChildByName("Instrument")->getStringAttribute("stateInformation");
    memBlock.fromBase64Encoding(vst2stateInfo);

    // VST2 state data is a Ccnk structure. At offset 156 is a big-endian 32-bit length value. Following that is the binary data.
    auto p = (uint8*)memBlock.getData() + 156;
    union { int i; uint8 b[4]; } len;
    len.b[3] = *p++;
    len.b[2] = *p++;
    len.b[1] = *p++;
    len.b[0] = *p++;

    // Assemble the VST3 version of the payload: little-endian 32-bit length followed by the binary data
    MemoryBlock payload(&len.i, 4);
    payload.append(p, len.i);
    String b64state = payload.toBase64Encoding();

    // prepare VST3 patch XML as deep copy of our one-layer patch XML
    auto patchXml = parseXML(BinaryData::One_VST3_Layer_xml);

    // swap in the metadata from the input VST2 patch
    patchXml->replaceChildElement(patchXml->getChildByName("PresetMetadata"), patchMetadataXml);

    // swap in the INST1 state data
    patchXml->getChildByName("Layer")->getChildByName("Instrument")->setAttribute("stateInformation", b64state);

    return patchXml.release();
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
