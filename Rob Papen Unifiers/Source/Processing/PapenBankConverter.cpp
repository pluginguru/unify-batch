#include "PapenBankConverter.h"

//#define LIBRARY_NAME "Unified - Rob Papen BIT2"
//#define AUTHOR "Factory voicing"
//#define PREFIX "Synth 01"
//#define CATEGORY "Synth"
//#define TAGS ""
//#define COMMENT "BIT2 Factory presets"

PapenBankConverter::PapenBankConverter()
{
	unifyPatchXml = parseXML(BinaryData::patchTemplate_xml);
}

int PapenBankConverter::convertBank(File bankIndexFile, File outputFolder)
{
	auto bank = StringArray::fromLines(bankIndexFile.loadFileAsString());
	int fileCount = 0;

	for (auto& line : bank)
	{
		if (line.contains("||"))
		{
			String patchName = line.fromLastOccurrenceOf("||", false, false);
			DBG(patchName);
			if (prefix.isNotEmpty()) patchName = prefix + " - " + patchName;
			if (selectPreset) selectPreset(fileCount);
			std::unique_ptr<XmlElement> patchXml(convertPreset(patchName));
			if (patchXml)
			{
				saveUnifyPatch(outputFolder, patchName, patchXml.get());
				fileCount++;
			}
		}
	}

	return fileCount;
}

XmlElement* PapenBankConverter::convertPreset(String patchName)
{
	// get state blob and convert to base64
	MemoryBlock mb;
	plugin->getStateInformation(mb);

	// assemble new Unify patch XML
	auto patchXml = new XmlElement(*unifyPatchXml); // deep copy
	auto layerXml = patchXml->getChildByName("Layer");
	layerXml->setAttribute("layerTitle", patchName);
	auto instXml = layerXml->getChildByName("Instrument");
	instXml->setAttribute("stateInformation", mb.toBase64Encoding());
	auto pmXml = patchXml->getChildByName("PresetMetadata");
	pmXml->setAttribute("name", patchName);
	pmXml->setAttribute("author", author);
	pmXml->setAttribute("category", category);
	pmXml->setAttribute("tags", tags);
	pmXml->setAttribute("comment", comment);
	pmXml->setAttribute("library", libraryName);

	return patchXml;
}

void PapenBankConverter::saveUnifyPatch(File outputFolder, String patchName, XmlElement* patchXml)
{
	MemoryBlock outBlock;
	AudioProcessor::copyXmlToBinary(*patchXml, outBlock);

	// Save to output folder
	File outFile = outputFolder.getChildFile(patchName + ".unify");
	outFile.create();
	outFile.replaceWithData(outBlock.getData(), outBlock.getSize());
}
