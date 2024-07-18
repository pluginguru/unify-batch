#include "TAL_Unifier.h"

TAL_Unifier::TAL_Unifier()
    : volumeScaling(1.0)
{
    unifyLibrariesFolder = File("E:\\PlugInGuru\\Unify\\Libraries");
}

void TAL_Unifier::makeUnifiedLibrary()
{
    // Load reference patch XML
    MemoryBlock ref;
    refPatch.loadFileAsData(ref);
    auto patchXml = AudioProcessor::getXmlFromBinary(ref.getData(), int(ref.getSize()));
    jassert(patchXml);

    // Get source items
    Array<File> items;
    for (DirectoryEntry entry : RangedDirectoryIterator(sourceFolder, true, "*." + presetFileType, File::findFiles))
    {
        File file = entry.getFile();
        items.addUsingDefaultSort(file);
    }

    // Process source items
    int count = 0;
    File itemFolder = sourceFolder;
    for (auto& item : items)
    {
        auto xml = makeUnifiedPatch(item);
        jassert(xml);

#if 0
        // TESTING: save new XML to a file
        File xmlFile(outputFolder.getChildFile(prefix + " - " + patchName + ".xml"));
        xmlFile.create();
        xml->writeTo(xmlFile);
#endif

        // put new instrument state into a new MemoryBlock, then wrap for VST
        MemoryBlock state;
        AudioProcessor::copyXmlToBinary(*xml, state);
        vstConv.setChunkData(state.getData(), int(state.getSize()), true);
        MemoryBlock vstState;
        vstConv.saveToFXBFile(vstState, true);

        auto metaXml = patchXml->getChildByName("PresetMetadata");
        metaXml->setAttribute("library", libraryName);
        metaXml->setAttribute("author", authorName);
        metaXml->setAttribute("name", prefix + " - " + patchName);
        metaXml->setAttribute("category", patchCategory);
        metaXml->setAttribute("tags", patchTags);
        metaXml->setAttribute("comment", patchComment);

        auto layerXml = patchXml->getChildByName("Layer");
        layerXml->setAttribute("layerTitle", presetName);
        auto instXml = layerXml->getChildByName("Instrument");
        instXml->setAttribute("stateInformation", vstState.toBase64Encoding());

#if 0
        // Experimental: transfer U-No-LX parameters to macros 2 and 3
        for (auto macroXml : patchXml->getChildWithTagNameIterator("MacroParameter"))
        {
            int index = macroXml->getIntAttribute("index");
            if (index == 1)
            {
                auto linkXml = macroXml->getChildByName("link");
                auto curveXml = linkXml->getChildByName("curve");
                auto segXml = curveXml->getChildByName("Segment");
                segXml->setAttribute("fyMax", delayDryWet);
            }
            else if (index == 2)
            {
                auto linkXml = macroXml->getChildByName("link");
                auto curveXml = linkXml->getChildByName("curve");
                auto segXml = curveXml->getChildByName("Segment");
                segXml->setAttribute("fyMax", reverbDryWet);
            }
            else if (index == 3)
            {
                auto linkXml = macroXml->getChildByName("link");
                auto curveXml = linkXml->getChildByName("curve");
                auto segXml = curveXml->getChildByName("Segment");
                segXml->setAttribute("fyMax", arpRate);
            }
            else if (index > 3) break;
        }
#endif

        MemoryBlock patch;
        AudioProcessor::copyXmlToBinary(*patchXml, patch);

        File outFile = outputFolder.getChildFile(prefix + " - " + patchName + ".unify");
        outFile.create();
        outFile.replaceWithData(patch.getData(), patch.getSize());

        count++;
    }

    DBG(String(count) + " items processed");
}
