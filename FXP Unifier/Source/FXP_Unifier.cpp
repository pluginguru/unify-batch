#include "FXP_Unifier.h"

FXP_Unifier::FXP_Unifier()
{
    unifyLibrariesFolder = File("D:\\PlugInGuru\\Unify\\Libraries");
}

void FXP_Unifier::makeUnifiedLibrary()
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
        instXml->setAttribute("currentProgram", vstConv.getCurrentProgram());

        MemoryBlock patch;
        AudioProcessor::copyXmlToBinary(*patchXml, patch);

        File outFile = outputFolder.getChildFile(prefix + " - " + patchName + ".unify");
        outFile.create();
        outFile.replaceWithData(patch.getData(), patch.getSize());

        count++;
    }

    DBG(String(count) + " items processed");
}
