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
    knownPluginsXmlFile = File::getSpecialLocation(File::userDocumentsDirectory)
        .getChildFile("WhiteElephantAudio/Syndicate").getChildFile("Data").getChildFile("ScannedPlugins.txt");

    knownPluginsXml = parseXML(knownPluginsXmlFile);
}

bool SyndicateKnownPluginsList::replaceWith(XmlElement* newKnownPluginsXml)
{
    if (newKnownPluginsXml->getTagName() != "KNOWNPLUGINS") return false;

    knownPluginsXml.reset(new XmlElement(*newKnownPluginsXml)); // deep copy
    knownPluginsXmlFile.moveFileTo(knownPluginsXmlFile.getSiblingFile("OLD ScannedPlugins.txt"));
    return knownPluginsXml->writeTo(knownPluginsXmlFile);
}
