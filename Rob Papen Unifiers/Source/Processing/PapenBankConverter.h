#pragma once
#include <JuceHeader.h>

class PapenBankConverter	: public Thread
{
public:
	PapenBankConverter();

	File bankIndexFile, outputFolder;
	String libraryName, author, prefix, category, tags, comment;

	void run() override;
	int getCount() { return fileCount; }

protected:
	std::unique_ptr<AudioPluginInstance> plugin;

	std::function<void(int)> selectPreset;

	XmlElement* convertPreset(String patchName);
	void saveUnifyPatch(File outputFolder, String patchName, XmlElement* patchXml);

	int fileCount;

private:
	std::unique_ptr<XmlElement> unifyPatchXml;
};
