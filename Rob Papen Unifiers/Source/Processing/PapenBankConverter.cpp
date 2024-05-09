#include "PapenBankConverter.h"

struct CategoryBits
{
	bool isLead;
	bool isBass;
	bool isSubBass;
	bool isSynth;
	bool isKey;
	bool isArpSeq;
	bool isArp;
	bool isArpChord;
	bool isSFX;
	bool isDrum;
	bool isPercussion;
	bool isMelodicPercussion;
	bool isBrass;
	bool isPad;
	bool isChord;
	bool isScreech;
	bool isWobble;
	bool isMisc;

	CategoryBits(String mask)
	{
		isLead =				(mask[0] == '1');
		isBass =				(mask[1] == '1');
		isSubBass =				(mask[2] == '1');
		isSynth =				(mask[3] == '1');
		isKey =					(mask[4] == '1');
		isArpSeq =				(mask[5] == '1');
		isArp =					(mask[6] == '1');
		isArpChord =			(mask[7] == '1');
		isSFX =					(mask[8] == '1');
		isDrum =				(mask[9] == '1');
		isPercussion =			(mask[10] == '1');
		isMelodicPercussion =	(mask[11] == '1');
		isBrass =				(mask[12] == '1');
		isPad =					(mask[13] == '1');
		isChord =				(mask[14] == '1');
		isScreech =				(mask[15] == '1');
		isWobble =				(mask[16] == '1');
		isMisc =				(mask[17] == '1');
	}

	String getPrefix()
	{
		if (isLead) return "LEAD";
		if (isBass || isSubBass) return "BASS";
		if (isSFX) return "SFX";
		if (isDrum) return "DRUM";
		if (isPad) return "PAD";
		if (isPercussion) return "PERC";
		if (isMelodicPercussion) return "MPERC";
		if (isBrass) return "BRASS";
		if (isChord) return "CHORD";
		if (isKey) return "KEY";
		if (isSynth) return "SYNTH";
		if (isArpSeq) return "SEQ";
		if (isArp || isArpChord) return "ARP";
		if (isScreech) return "SCRCH";
		if (isWobble) return "WOB";
		if (isMisc) return "MISC";
		return "UNKOWN";
	}

	String getCategories()
	{
		StringArray cats;
		if (isLead) cats.add("Lead");
		if (isBass || isSubBass) cats.add("Bass");
		if (isSFX) cats.add("SFX");
		if (isDrum) cats.add("Drum");
		if (isPad) cats.add("Pad");
		if (isPercussion) cats.add("Perc");
		if (isMelodicPercussion) cats.add("Melodic Perc");
		if (isBrass) cats.add("Brass");
		if (isChord) cats.add("Chord");
		if (isKey) cats.add("Key");
		if (isSynth) cats.add("Synth");
		if (isArpSeq) cats.add("Sequence");
		if (isArp || isArpChord) cats.add("Arp");
		if (isScreech) cats.add("Screech");
		if (isWobble) cats.add("Wobble");
		if (isMisc) cats.add("Misc");

		return cats.joinIntoString(",");
	}
};

PapenBankConverter::PapenBankConverter()
	: Thread("ConvertThread")
	, fileCount(0)
{
	unifyPatchXml = parseXML(BinaryData::patchTemplate_xml);
}

void PapenBankConverter::run()
{
	auto bank = StringArray::fromLines(bankIndexFile.loadFileAsString());
	fileCount = 0;

	String patchName;
	for (auto& line : bank)
	{
		if (threadShouldExit()) break;

		line = line.trimEnd();
		if (line.isEmpty()) break;

		if (line.contains("||"))
		{
			// first line of a pair: contains preset name
			patchName = line.fromLastOccurrenceOf("||", false, false);
			DBG(patchName);
		}
		else
		{
			// second line of a pair: contains 66-bit binary mask for categories
			CategoryBits cbits(line);

			// interpret category mask
			prefix = cbits.getPrefix();
			category = cbits.getCategories();

			// convert the patch
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
