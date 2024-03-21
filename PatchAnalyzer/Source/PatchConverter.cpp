#include "PatchConverter.h"

PatchConverter::PatchConverter()
{
}

int PatchConverter::processFiles(const StringArray& sfzPaths)
{
    if (outputFolderPath.isNotEmpty())
    {
        outputFolder = File(outputFolderPath);
        outputFolder.createDirectory();
    }

    int fileCount = 0;
    for (auto sfzPath : sfzPaths)
    {
        File file(sfzPath);
        processFileOrFolder(file, fileCount);
    }
    return fileCount;
}

void PatchConverter::processFileOrFolder(File file, int& fileCount)
{
    if (file.isDirectory())
    {
        for (DirectoryEntry entry : RangedDirectoryIterator(file, false, "*", File::findFilesAndDirectories))
            processFileOrFolder(entry.getFile(), fileCount);
    }
    else if (file.getFileExtension() == ".unify")
    {
        File outFile;
        if (outputFolderPath.isEmpty())
            outFile = file.getSiblingFile(file.getFileNameWithoutExtension() + ".txt");
        else
            outFile = File(outputFolderPath).getChildFile(file.getFileNameWithoutExtension() + ".txt");
        FileOutputStream outStream(outFile);

        MemoryBlock mb;
        jassert(file.loadFileAsData(mb));
        auto patchXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
        processPatchXml(patchXml.get(), outStream, 0);
        fileCount++;
    }
}

static String indented(String str, int indentLevel)
{
    const int indentSpaces = 4;
    if (indentLevel == 0) return str;
    return str.paddedLeft(' ', str.length() + indentLevel * indentSpaces);
}

void PatchConverter::processPatchXml(XmlElement* patchXml, FileOutputStream& outStream, int indentLevel)
{
    auto pmXml = patchXml->getChildByName("PresetMetadata");
    auto patchName = pmXml->getStringAttribute("name");
    outStream.writeText(indented("Patch name: " + patchName + "\n", indentLevel), false, false, nullptr);

    int layerNumber = 0;
    for (auto* layerXml : patchXml->getChildWithTagNameIterator("MidiLane"))
    {
        auto layerTitle = layerXml->getStringAttribute("layerTitle");
        outStream.writeText("\n" + indented("MIDI" + String(++layerNumber) + ": " + layerTitle + '\n', indentLevel), false, false, nullptr);
        processLayerXml(layerXml, outStream, 1 + indentLevel);
    }

    layerNumber = 0;
    for (auto* layerXml : patchXml->getChildWithTagNameIterator("Layer"))
    {
        auto layerTitle = layerXml->getStringAttribute("layerTitle");
        outStream.writeText("\n" + indented("INST" + String(++layerNumber) + ": " + layerTitle + '\n', indentLevel), false, false, nullptr);
        processLayerXml(layerXml, outStream, 1 + indentLevel);
    }

    layerNumber = 0;
    for (auto* layerXml : patchXml->getChildWithTagNameIterator("AudioFxLane"))
    {
        auto layerTitle = layerXml->getStringAttribute("layerTitle");
        String layerName = "AUX" + String(layerNumber++);
        if (layerName == "AUX0") layerName = "MASTER";
        outStream.writeText("\n" + indented(layerName + ": " + layerTitle + '\n', indentLevel), false, false, nullptr);
        processLayerXml(layerXml, outStream, 1 + indentLevel);
    }
}

void PatchConverter::processLayerXml(XmlElement* layerXml, FileOutputStream& outStream, int indentLevel)
{
    auto midiInsertsXml = layerXml->getChildByName("MidiInserts");
    if (midiInsertsXml)
    {
        for (auto* midiInsertXml : midiInsertsXml->getChildWithTagNameIterator("InsertEffect"))
            processPluginXml("m ", midiInsertXml, outStream, indentLevel);
    }

    auto instXml = layerXml->getChildByName("Instrument");
    if (instXml)
        processPluginXml("i ", instXml, outStream, indentLevel);

    auto audioInsertsXml = layerXml->getChildByName("AudioInserts");
    if (audioInsertsXml)
    {
        for (auto* audioInsertXml : audioInsertsXml->getChildWithTagNameIterator("InsertEffect"))
            processPluginXml("a ", audioInsertXml, outStream, indentLevel);
    }
}

void PatchConverter::processPluginXml(String prefix, XmlElement* itemXml, FileOutputStream& outStream, int indentLevel)
{
    auto pluginXml = itemXml->getChildByName("PLUGIN");
    auto pluginName = pluginXml->getStringAttribute("name");
    auto pluginFormat = pluginXml->getStringAttribute("format");
    outStream.writeText(indented(prefix + pluginName + " [" + pluginFormat + "]\n", indentLevel), false, false, nullptr);

    if (pluginFormat == "Built-In")
    {
        if (pluginName == "Unify")
        {
            MemoryBlock inBlock;
            String stateInfo = itemXml->getStringAttribute("stateInformation");
            inBlock.fromBase64Encoding(stateInfo);

            auto subPatchXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
            processPatchXml(subPatchXml.get(), outStream, 1 + indentLevel);
        }

        else if (pluginName == "ComboBox")
        {
            MemoryBlock inBlock;
            String stateInfo = itemXml->getStringAttribute("stateInformation");
            inBlock.fromBase64Encoding(stateInfo);

            auto comboXml = AudioProcessor::getXmlFromBinary(inBlock.getData(), inBlock.getSize());
            processComboXml(comboXml.get(), outStream, 1 + indentLevel);
        }
    }
}

void PatchConverter::processComboXml(XmlElement* comboXml, FileOutputStream& outStream, int indentLevel)
{
    for (auto* itemXml : comboXml->getChildWithTagNameIterator("FILTER"))
        processPluginXml("", itemXml, outStream, indentLevel);
}
