#pragma once
#include <JuceHeader.h>

class UnifyKnownPluginsList
{
public:
	UnifyKnownPluginsList();

	XmlElement* getXml() { return knownPluginsXml.get(); }
	bool replaceWith(XmlElement* newKnownPluginsXml);

private:
	std::unique_ptr<PropertiesFile> knownPluginsPropsFile;
	std::unique_ptr<XmlElement> knownPluginsXml;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnifyKnownPluginsList)
};


class SyndicateKnownPluginsList
{
public:
	SyndicateKnownPluginsList();

	XmlElement* getXml() { return knownPluginsXml.get(); }
	bool replaceWith(XmlElement* newKnownPluginsXml);

private:
	File knownPluginsXmlFile;
	std::unique_ptr<XmlElement> knownPluginsXml;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyndicateKnownPluginsList)
};
