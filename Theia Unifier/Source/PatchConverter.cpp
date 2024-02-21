#pragma once
#include "PatchConverter.h"
#include "MyVstUtils.h"

static char jucePrivateData[] =
"\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"JUCEPrivateData\x00\x01\x01"
"Bypass\x00\x01\x01\x03\x00\x1d\x00\x00\x00\x00\x00\x00\x00"
"JUCEPrivateData";

PatchConverter::PatchConverter()
{
    unifyPatchXml = parseXML(BinaryData::One_Theia_Layer_xml);
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
    else if (file.getFileExtension() == ".hypr_combi")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processTheiaCombi(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
    else if (file.getFileExtension() == ".hypr_patch")
    {
        String newPatchNameOrErrorMessage;
        std::unique_ptr<XmlElement> patchXml(processTheiaPatch(file, newPatchNameOrErrorMessage));
        if (patchXml)
            saveUnifyPatch(file, newPatchNameOrErrorMessage, patchXml.get());
        else
            AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon, "Conversion error",
                file.getFileName() + ": " + newPatchNameOrErrorMessage);

        fileCount++;
    }
}

XmlElement* PatchConverter::processTheiaCombi(File inFile, String& newPatchNameOrErrorMessage)
{
    newPatchNameOrErrorMessage = inFile.getFileNameWithoutExtension();

    String comment = "Theia factory combi by Wavesequencer";
    String category, tags, author;
    {
        var json = JSON::parse(inFile);
        var metadata = json["Metadata"];

        author = metadata["patchAuthor"].toString();
        category = metadata["instrumentType"].toString();

        newPatchNameOrErrorMessage = category.toUpperCase() + " - " + metadata["patchName"].toString();

        String notes = metadata["patchNotes"].toString();
        //if (notes.isNotEmpty()) comment += "\n" + notes;

        tags = metadata["soundCategory1"].toString();
        String tag2 = metadata["soundCategory2"].toString();
        if (tag2.isNotEmpty()) tags += ", " + tag2;
        String tag3 = metadata["soundCategory3"].toString();
        if (tag3.isNotEmpty()) tags += ", " + tag3;
    }

    // take the input combi JSON string, append the jucePrivateData, and encode as base64
    MemoryBlock memBlock;
    inFile.loadFileAsData(memBlock);
    memBlock.append(jucePrivateData, sizeof(jucePrivateData) - 1);
    String b64state = memBlock.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new Theia binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, memBlock);
    b64state = memBlock.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", newPatchNameOrErrorMessage);
    auto instXml = layerXml->getChildByName("Instrument");
    if (hyperion)
    {
        auto pluginXml = instXml->getChildByName("PLUGIN");
        pluginXml->setAttribute("name", "Hyperion");
    }
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    pmXml->setAttribute("author", author);
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", comment);
    if (libraryName.isNotEmpty()) pmXml->setAttribute("library", libraryName);

    return patchXml;
}

XmlElement* PatchConverter::processTheiaPatch(File inFile, String& newPatchNameOrErrorMessage)
{
    newPatchNameOrErrorMessage = inFile.getFileNameWithoutExtension();

    // parse the Theia patch and extract various metadata
    var json = JSON::parse(inFile);
    var metadata = json["Metadata"];
    String author = metadata["patchAuthor"].toString();
    String category = metadata["instrumentType"].toString();
    String comment = "Theia factory patch by Wavesequencer";
    String notes = metadata["patchNotes"].toString();
    //if (notes.isNotEmpty()) comment += "\n" + notes;
    String background = metadata["theiaBackground"].toString();
    newPatchNameOrErrorMessage = category.toUpperCase() + " - " + metadata["patchName"].toString();
    String tags = metadata["soundCategory1"].toString();
    String tag2 = metadata["soundCategory2"].toString();
    if (tag2.isNotEmpty()) tags += ", " + tag2;
    String tag3 = metadata["soundCategory3"].toString();
    if (tag3.isNotEmpty()) tags += ", " + tag3;

    // copy the theiaBackground property into the output combi metadata
    var theiaTemplate = JSON::parse(BinaryData::Single_Theia_Patch_json);
    auto theiaObject = theiaTemplate.getDynamicObject();
    theiaObject->getProperty("Metadata").getDynamicObject()->setProperty("theiaBackground", background);

    // stuff the entire Theia patch JSON into output combi Synths array
    auto synthsArray = theiaObject->getProperty("Synths").getArray();
    synthsArray->add(json);
    // put the Theia patch uuid into the SynthUuids array
    auto uuidsArray = theiaObject->getProperty("SynthUuids").getArray();
    uuidsArray->add(metadata["patchUuid"]);

    // take the combi JSON string, append the jucePrivateData, and encode as base64
    String combiJson = JSON::toString(theiaTemplate);
    MemoryBlock memBlock;
    memBlock.append(combiJson.toRawUTF8(), combiJson.getNumBytesAsUTF8());
    memBlock.append(jucePrivateData, sizeof(jucePrivateData) - 1);
    String b64state = memBlock.toBase64Encoding();

    // assemble a VST3PluginState XML structure, with this base64 state-string
    std::unique_ptr<XmlElement> vst3stateXml(new XmlElement("VST3PluginState"));
    auto iComponentXml = new XmlElement("IComponent");
    vst3stateXml->addChildElement(iComponentXml);
    iComponentXml->addTextElement(b64state);

    // now base64 encode that entire XML string, as new Theia binary state
    AudioProcessor::copyXmlToBinary(*vst3stateXml, memBlock);
    b64state = memBlock.toBase64Encoding();

    // assemble new Unify patch XML
    auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
    auto layerXml = patchXml->getChildByName("Layer");
    layerXml->setAttribute("layerTitle", newPatchNameOrErrorMessage);
    auto instXml = layerXml->getChildByName("Instrument");
    if (hyperion)
    {
        auto pluginXml = instXml->getChildByName("PLUGIN");
        pluginXml->setAttribute("name", "Hyperion");
    }
    instXml->setAttribute("stateInformation", b64state);
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    pmXml->setAttribute("name", newPatchNameOrErrorMessage);
    pmXml->setAttribute("author", author);
    pmXml->setAttribute("category", category);
    pmXml->setAttribute("tags", tags);
    pmXml->setAttribute("comment", comment);
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
