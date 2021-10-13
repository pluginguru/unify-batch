#include "Synth1_Unifier.h"

Synth1_Unifier::Synth1_Unifier()
{
    //libraryName = "Unified - Synth1 Factory";
    //authorName = "Ichiro Toda";
    //patchComment = "Unified factory patches for Synth1 by Daichi Laboratory";
    authorName = "Madame Eljay";
    libraryName = "Unified - Synth1 " + authorName;// +" Patch Book";
    //libraryName = "Unified - Synth1 Madame Eljay";
    patchComment = "Unified patches for Synth1 by " + authorName;

    unifyLibrariesFolder = File("D:\\PlugInGuru\\Unify\\Libraries");

    presetsRootFolder = File("C:\\VST\\Synth1\\factory");
    presetFileType = "sy1";
    refPatch = File("D:\\Documents\\Github\\unify-batch\\Synth1 Unifier\\Synth1 1 SINE.unify");
    sourceFolder = presetsRootFolder;

    outputFolder = unifyLibrariesFolder.getChildFile(libraryName).getChildFile("Patches");
    prefix = "SY1";

    plugin.reset(instantiatePlugin("VST", "Synth1", "Daichi"));
    jassert(plugin);

    plugin->prepareToPlay(44100.0, 512);
}

void Synth1_Unifier::makeUnifiedLibrary()
{
    // Load reference patch XML
    MemoryBlock ref;
    refPatch.loadFileAsData(ref);
    auto patchXml = AudioProcessor::getXmlFromBinary(ref.getData(), int(ref.getSize()));
    jassert(patchXml);

    auto instXml = patchXml->getChildByName("Layer")->getChildByName("Instrument");
    jassert(instXml);
    refState.fromBase64Encoding(instXml->getStringAttribute("stateInformation"));

    // Get source items
    Array<File> items;
    for (DirectoryEntry entry : RangedDirectoryIterator(sourceFolder, true, "*." + presetFileType, File::findFiles))
    {
        File file = entry.getFile();
        items.addUsingDefaultSort(file);
    }

    // Process source items
    int count = 0;
    for (auto& item : items)
    {
        MemoryBlock vstState;
        getPatchDetailsAndVstState(item, vstState);

        auto metaXml = patchXml->getChildByName("PresetMetadata");
        metaXml->setAttribute("library", libraryName);
        metaXml->setAttribute("author", authorName);
        metaXml->setAttribute("name", prefix + " - " + patchName);
        metaXml->setAttribute("category", patchCategory);
        metaXml->setAttribute("tags", patchTags);
        metaXml->setAttribute("comment", patchComment);

        auto layerXml = patchXml->getChildByName("Layer");
        layerXml->setAttribute("layerTitle", patchName);
        auto instXml = layerXml->getChildByName("Instrument");
        instXml->setAttribute("stateInformation", vstState.toBase64Encoding());
        instXml->setAttribute("currentProgram", 0);

        MemoryBlock patch;
        AudioProcessor::copyXmlToBinary(*patchXml, patch);

        File outFile = outputFolder.getChildFile(File::createLegalFileName(prefix + " - " + patchName + ".unify"));
        outFile.create();
        outFile.replaceWithData(patch.getData(), patch.getSize());

        count++;
    }

    DBG(String(count) + " items processed");
}

void Synth1_Unifier::getPatchDetailsAndVstState(File& sourcePresetFile, MemoryBlock& vstState)
{
    int patchNum = sourcePresetFile.getFileNameWithoutExtension().getIntValue() - 1;
    vstConv.loadFromFXBFile(refState.getData(), refState.getSize());

    StringArray lines;
    sourcePresetFile.readLines(lines);
    patchName = lines[0].trim();

#if 0
    // the failed first attempt
    int* params = (int*)(vstConv.memoryBlock.getData());
    //params[647] = patchNum;
    params += 143;

    for (int i = 0; i <= 72; i++)
        params[2 * i] = 0;

    for (int i = 1; i < lines.size(); i++)
    {
        if (lines[i].containsChar(','))
        {
            StringArray parts = StringArray::fromTokens(lines[i], ",", "");
            jassert(parts.size() == 2);
            int index = parts[0].getIntValue();
            int value = parts[1].getIntValue();

            params[2 * index] = value;
        }
    }

    vstConv.saveToFXBFile(vstState, true);
#else
    // new "hybrid" technique
    AudioSampleBuffer audio;
    audio.setSize(2, 512);
    MidiBuffer midi;
    midi.addEvent(MidiMessage::programChange(1, patchNum), 0);
    plugin->processBlock(audio, midi);

    plugin->getStateInformation(vstState);
#endif
}

AudioPluginInstance* Synth1_Unifier::instantiatePlugin(String formatName, String pluginNamePrefix, String vendorName)
{
    AudioPluginFormatManager pluginFormatManager;
    pluginFormatManager.addDefaultFormats();

    // Get Unify's PluginList.xml
    PropertiesFile::Options options;
    options.applicationName = "PluginList";
    options.filenameSuffix = ".xml";
    options.folderName = "PlugInGuru";
    options.osxLibrarySubFolder = "Application Support";
    std::unique_ptr<PropertiesFile> knownPluginsFile;
    knownPluginsFile.reset(new PropertiesFile(options));

    std::unique_ptr<XmlElement> savedPluginList(knownPluginsFile->getXmlValue("pluginList"));
    if (savedPluginList)
    {
        KnownPluginList knownPluginList;
        knownPluginList.recreateFromXml(*savedPluginList);
        auto knownTypes = knownPluginList.getTypes();

        PluginDescription pid = knownTypes[0];
        for (auto& desc : knownTypes)
        {
            if (desc.pluginFormatName == formatName &&
                desc.name.startsWith(pluginNamePrefix) &&
                desc.manufacturerName == vendorName)
            {
                pid = desc;
                break;
            }
        }

        String errorMessage;
        return pluginFormatManager.createPluginInstance(pid, 44100.0, 512, errorMessage).release();
    }

    return nullptr;
}
