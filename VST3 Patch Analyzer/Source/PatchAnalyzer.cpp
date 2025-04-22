#include "PatchAnalyzer.h"

void analyzePatch(const String& path)
{
    File assetsDir("C:\\Users\\owner\\Documents\\GitHub\\unify-batch\\VST3 Patch Analyzer\\Assets");
    assetsDir.createDirectory();

    File patchFile(path);
    MemoryBlock mb;
    patchFile.loadFileAsData(mb);

    auto patchXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());

    auto layerXml = patchXml->getChildByName("Layer");
    auto instXml = layerXml->getChildByName("Instrument");
    auto stateInfoString = instXml->getStringAttribute("stateInformation");

    mb.reset();
    bool b64ok = mb.fromBase64Encoding(stateInfoString);

    auto vst3stateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    vst3stateXml->writeTo(assetsDir.getChildFile("vst3state.xml"));

    auto icDataString = vst3stateXml->getChildByName("IComponent")->getAllSubText();
    b64ok = mb.fromBase64Encoding(icDataString);
    assetsDir.getChildFile("ic.txt").replaceWithData(mb.getData(), mb.getSize());

    auto iecXml = vst3stateXml->getChildByName("IEditController");
    if (iecXml)
    {
        icDataString = iecXml->getAllSubText();
        b64ok = mb.fromBase64Encoding(icDataString);
        assetsDir.getChildFile("iec.txt").replaceWithData(mb.getData(), mb.getSize());
    }

#if 0
    // what follows is specific to Korg MultiPoly native VST3
    auto pluginStateXml = AudioProcessor::getXmlFromBinary(mb.getData(), mb.getSize());
    pluginStateXml->writeTo(assetsDir.getChildFile("pluginState.xml"));

    auto performanceXml = pluginStateXml->getChildByName("Performance");
    String performanceObjectInfo = performanceXml->getStringAttribute("performanceObjectInfo");
    String performanceObject = performanceXml->getStringAttribute("performanceObject");

    mb.fromBase64Encoding(performanceObjectInfo);
    assetsDir.getChildFile("performanceObjectInfo.dat").replaceWithData(mb.getData(), mb.getSize());

    mb.fromBase64Encoding(performanceObject);
    assetsDir.getChildFile("performanceObject.dat").replaceWithData(mb.getData(), mb.getSize());
#endif
}
