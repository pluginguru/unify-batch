#include "Exporters.h"

UnifyKnownPluginsList::UnifyKnownPluginsList()
{
    PropertiesFile::Options options;
    options.applicationName = "PluginList";
    options.filenameSuffix = ".xml";
    options.folderName = "PlugInGuru";
    options.osxLibrarySubFolder = "Application Support";

    knownPluginsPropsFile.reset(new PropertiesFile(options));
    if (knownPluginsPropsFile)
        knownPluginsXml = knownPluginsPropsFile->getXmlValue("pluginList");
}

bool UnifyKnownPluginsList::replaceWith(XmlElement* newKnownPluginsXml)
{
    if (newKnownPluginsXml->getTagName() != "KNOWNPLUGINS") return false;

    knownPluginsXml.reset(new XmlElement(*newKnownPluginsXml)); // deep copy

    File propsFile = knownPluginsPropsFile->getFile();
    if (!propsFile.copyFileTo(propsFile.getSiblingFile("OLD PluginList.xml"))) return false;

    knownPluginsPropsFile->setValue("pluginList", newKnownPluginsXml);
    if (!knownPluginsPropsFile->save()) return false;

    return true;
}


SyndicateKnownPluginsList::SyndicateKnownPluginsList()
{
#ifdef __APPLE__
    knownPluginsXmlFile = File::getSpecialLocation(File::userApplicationDataDirectory)
        .getChildFile("WhiteElephantAudio/Syndicate").getChildFile("ScannedPlugins.txt");
#else
    knownPluginsXmlFile = File::getSpecialLocation(File::userDocumentsDirectory)
        .getChildFile("WhiteElephantAudio/Syndicate").getChildFile("Data").getChildFile("ScannedPlugins.txt");
#endif

    knownPluginsXml = parseXML(knownPluginsXmlFile);
}

bool SyndicateKnownPluginsList::replaceWith(XmlElement* newKnownPluginsXml)
{
    if (newKnownPluginsXml->getTagName() != "KNOWNPLUGINS") return false;

    knownPluginsXml.reset(new XmlElement(*newKnownPluginsXml)); // deep copy
    knownPluginsXmlFile.moveFileTo(knownPluginsXmlFile.getSiblingFile("OLD ScannedPlugins.txt"));
    return knownPluginsXml->writeTo(knownPluginsXmlFile);
}


KshmrChainKnownPluginsList::KshmrChainKnownPluginsList()
{
#ifdef __APPLE__
    knownPluginsXmlFile = File::getSpecialLocation(File::userApplicationDataDirectory)
        .getChildFile("Excite Audio").getChildFile("Chain").getChildFile("plugins.cache");
#else
    knownPluginsXmlFile = File::getSpecialLocation(File::commonApplicationDataDirectory)
        .getChildFile("Excite Audio").getChildFile("Chain").getChildFile("plugins64.cache");
#endif

    knownPluginsXml = parseXML(knownPluginsXmlFile);
}

bool KshmrChainKnownPluginsList::replaceWith(XmlElement* newKnownPluginsXml)
{
    if (newKnownPluginsXml->getTagName() != "cachedPlugins") return false;

    knownPluginsXml.reset(new XmlElement(*newKnownPluginsXml)); // deep copy
#ifdef __APPLE__
    knownPluginsXmlFile.moveFileTo(knownPluginsXmlFile.getSiblingFile("OLD plugins.cache"));
#else
    knownPluginsXmlFile.moveFileTo(knownPluginsXmlFile.getSiblingFile("OLD plugins64.cache"));
#endif
    return knownPluginsXml->writeTo(knownPluginsXmlFile);
}
