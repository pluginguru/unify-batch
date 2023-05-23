#pragma once
#include "PatchConverter.h"
#include "MyVstUtils.h"

PatchConverter::PatchConverter()
{
    initializedMultiXml = parseXML(BinaryData::Omnisphere_Initialized_Multi_xml);
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
    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFile(entry.getFile(), fileCount);
    }
    else if (file.getFileExtension() == ".unify")
    {
        DBG(file.getFileName());

        MemoryBlock inBlock;
        file.loadFileAsData(inBlock);

        std::unique_ptr<XmlElement> patchXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
        String newPatchNameOrErrorMessage;
        if (processPatchXmlAndReturnNewPatchName(patchXml.get(), newPatchNameOrErrorMessage))
        {
            MemoryBlock outBlock;
            AudioProcessor::copyXmlToBinary(*patchXml, outBlock);

            if (outputFolderPath.isNotEmpty())
            {
                // Save to output folder
                File outFile = File(outputFolderPath).getChildFile(newPatchNameOrErrorMessage + ".unify");
                outFile.create();
                outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
            }
            else
            {
                // Save in original folder
                File outFile = file;
                if (patchNameSuffix.isNotEmpty())
                    outFile = file.getSiblingFile(newPatchNameOrErrorMessage + ".unify");
                outFile.create();
                outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
            }
        }
        else
        {
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Patch format error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);
        }

        fileCount++;
    }
}

bool PatchConverter::processPatchXmlAndReturnNewPatchName(XmlElement* patchXml, String& newNameOrErrorMessage)
{
    int layerCount = 0;
    for (auto layerXml : patchXml->getChildWithTagNameIterator("Layer"))
    {
        layerCount++;
        auto pluginXml = layerXml->getChildByName("Instrument")->getChildByName("PLUGIN");
        if (pluginXml->getStringAttribute("name") != "Omnisphere" || pluginXml->getStringAttribute("format") != "VST")
        {
            newNameOrErrorMessage = "INST layer must contain only Omnisphere VST";
            return false;
        }
    }
    if (layerCount != 1)
    {
        newNameOrErrorMessage = "Must have exactly one INST layer";
        return false;
    }

    // Duplicate 1st INST layer up to 8 times (modified) then remove it
    auto layerXml = patchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    for (int partIndex = 0; partIndex < 8; partIndex++)
    {
        PartInfo partInfo;
        String stateInfo = instXml->getStringAttribute("stateInformation");
        String newStateInfo = isolateOneOmnispherePart(stateInfo, partIndex, partInfo);
        if (newStateInfo.isNotEmpty())
        {
            auto newLayerXml = new XmlElement(*layerXml);   // deep copy
            if (setLayerTitle)
                newLayerXml->setAttribute("layerTitle", partInfo.partName);
            if (setLayerLevelAndPan)
            {
                newLayerXml->setAttribute("mixLevel", partInfo.mixLevel);
                newLayerXml->setAttribute("panPos", partInfo.panPos);
            }
            if (setLayerSplitDetails && partInfo.splitDetailsValid)
            {
                newLayerXml->setAttribute("lokey", partInfo.lokey);
                newLayerXml->setAttribute("hikey", partInfo.hikey);
                newLayerXml->setAttribute("loFadeKey", partInfo.loFadeKey);
                newLayerXml->setAttribute("hiFadeKey", partInfo.hiFadeKey);
            }
            newLayerXml->getChildByName("Instrument")->setAttribute("stateInformation", newStateInfo);
            patchXml->addChildElement(newLayerXml);
        }
    }
    patchXml->removeChildElement(layerXml, true);

    // update patch name
    XmlElement* pmXml = patchXml->getChildByName("PresetMetadata");
    newNameOrErrorMessage = pmXml->getStringAttribute("name").trim();
    if (patchNameSuffix.isNotEmpty()) newNameOrErrorMessage += " " + patchNameSuffix.trim();
    pmXml->setAttribute("name", newNameOrErrorMessage.trim());
    return true;
}

String PatchConverter::isolateOneOmnispherePart(const String& stateInfo, int partIndex, PartInfo& partInfo)
{
    // recover XML from Base64-encoded, VST-encoded state string
    MemoryBlock inBlock;
    inBlock.fromBase64Encoding(stateInfo);
    MyVstConv conv;
    conv.loadFromFXBFile(inBlock.getData(), inBlock.getSize());
    auto s = String::createStringFromData(conv.memoryBlock.getData(), conv.memoryBlock.getSize());
    auto omniXml = parseXML(s);
    jassert(omniXml);

    // get part level and pan amount (as hexadecimal strings)
    auto mebpbXml = omniXml->getChildByName("SynthMasterEngineParamBlock")->getChildByName("MasterEngineBaseParamBlock");
    String partLevelHex = mebpbXml->getStringAttribute("pLevel" + String(partIndex));
    union { float f; uint8 bytes[4]; } data;
    data.bytes[3] = partLevelHex.substring(0, 2).getHexValue32() & 0xFF;
    data.bytes[2] = partLevelHex.substring(2, 4).getHexValue32() & 0xFF;
    data.bytes[1] = partLevelHex.substring(4, 6).getHexValue32() & 0xFF;
    data.bytes[0] = partLevelHex.substring(6, 8).getHexValue32() & 0xFF;
    float dB = (data.f < 0.75f) ? 17.378f * logf(data.f) + 5.0317f : 9.54f * (data.f - 0.75f);
    partInfo.mixLevel = Decibels::decibelsToGain(dB, -80.0f);
    String partPanHex = mebpbXml->getStringAttribute("pPan" + String(partIndex));
    data.bytes[3] = partPanHex.substring(0, 2).getHexValue32() & 0xFF;
    data.bytes[2] = partPanHex.substring(2, 4).getHexValue32() & 0xFF;
    data.bytes[1] = partPanHex.substring(4, 6).getHexValue32() & 0xFF;
    data.bytes[0] = partPanHex.substring(6, 8).getHexValue32() & 0xFF;
    partInfo.panPos = data.f;

    // get part key range and fade limits
    partInfo.loFadeKey = partInfo.lokey = 0;
    partInfo.hiFadeKey = partInfo.hikey = 127;
    partInfo.splitDetailsValid = false;
    auto stackRampsXml = omniXml->getChildByName("InputMapper")->getChildByName("StackRamps");
    if (stackRampsXml)
    {
        for (auto spXml : stackRampsXml->getChildWithTagNameIterator("SplitPart"))
        {
            int pi = spXml->getIntAttribute("Part");
            if (pi == partIndex)
            {
                partInfo.splitDetailsValid = true;
                partInfo.left = spXml->getIntAttribute("Left");
                partInfo.leftRamp = spXml->getIntAttribute("LeftRamp");
                partInfo.right = spXml->getIntAttribute("Right");
                partInfo.rightRamp = spXml->getIntAttribute("RightRamp");
                partInfo.lokey = partInfo.left;
                partInfo.hikey = partInfo.right;
                partInfo.loFadeKey = partInfo.left + partInfo.leftRamp;
                partInfo.hiFadeKey = partInfo.right - partInfo.rightRamp;
                break;
            }
        }
    }
    else DBG("No InputMapper/StackRamps");

    // locate desired part (SynthSubEngine tag)
    XmlElement* partXml = nullptr;
    int pi = 0;
    for (auto pXml : omniXml->getChildWithTagNameIterator("SynthSubEngine"))
    {
        if (pi == partIndex)
        {
            partXml = pXml;
            break;
        }
        pi++;
    }

    // Check part's ENTRYDESCR; if part is unused, return empty string
    auto seXml = partXml->getChildByName("SynthEngine");
    jassert(seXml);
    auto sengXml = seXml->getChildByName("SYNTHENG");
    if (!sengXml) sengXml = seXml->getChildByName("SYNTHENG2"); // support newer Omni presets
    jassert(sengXml);
    auto partEntryDescr = sengXml->getChildByName("ENTRYDESCR");
    partInfo.partName = partEntryDescr->getStringAttribute("name");
    String partEntryDescrAttribValueData = partEntryDescr->getStringAttribute("ATTRIB_VALUE_DATA");
    if (partEntryDescrAttribValueData == "size=0;") return String();

    // replace main ENTRYDESCR tag with deep copy of part's ENTRYDESCR
    auto mainEntryDescr = omniXml->getChildByName("ENTRYDESCR");
    auto copyOfPartEntryDescr = new XmlElement(*partEntryDescr);    // deep copy
    omniXml->replaceChildElement(mainEntryDescr, copyOfPartEntryDescr);

    // prepare new Omnisphere preset XML as deep copy of initialized-Multi
    std::unique_ptr<XmlElement> newOmniXml(new XmlElement(*initializedMultiXml));           // deep copy

    // replace first part with deep copy of desired part
    auto firstPartXml = newOmniXml->getChildByName("SynthSubEngine");
    auto copyOfPartXml = new XmlElement(*partXml);                                          // deep copy
    newOmniXml->replaceChildElement(firstPartXml, copyOfPartXml);

    if (retainPartLevelAndPan)
    {
        mebpbXml = newOmniXml->getChildByName("SynthMasterEngineParamBlock")->getChildByName("MasterEngineBaseParamBlock");
        mebpbXml->setAttribute("pLevel0", partLevelHex);
        mebpbXml->setAttribute("pPan0", partPanHex);
    }

    if (partInfo.splitDetailsValid && retainPartSplitDetails)
    {
        auto inputMapperXml = newOmniXml->getChildByName("InputMapper");
        inputMapperXml->setAttribute("mapMode", "3d75c28f");
        stackRampsXml = inputMapperXml->getChildByName("StackRamps");
        auto part1SplitXml = stackRampsXml->getChildByName("SplitPart");
        part1SplitXml->setAttribute("Left", partInfo.left);
        part1SplitXml->setAttribute("LeftRamp", partInfo.leftRamp);
        part1SplitXml->setAttribute("Right", partInfo.right);
        part1SplitXml->setAttribute("RightRamp", partInfo.rightRamp);
    }

    // convert XML back to VST-encoded, Base64-encoded form
    conv.memoryBlock.setSize(0);
    MemoryOutputStream stream(conv.memoryBlock, false);
    s = newOmniXml->toString();
    stream.writeText(s, false, false, nullptr);
    stream.flush();

    MemoryBlock outBlock;
    conv.saveToFXBFile(outBlock, true);
    return outBlock.toBase64Encoding();
}
