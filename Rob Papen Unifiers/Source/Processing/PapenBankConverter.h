#pragma once
#include <JuceHeader.h>

class PapenBankConverter
{
public:
	PapenBankConverter();

	String libraryName, author, prefix, category, tags, comment;

	int convertBank(File bankIndexFile, File outputFolder);

protected:
	std::unique_ptr<AudioPluginInstance> plugin;

	std::function<void(int)> selectPreset;

	XmlElement* convertPreset(String patchName);
	void saveUnifyPatch(File outputFolder, String patchName, XmlElement* patchXml);

private:
	std::unique_ptr<XmlElement> unifyPatchXml;
};
