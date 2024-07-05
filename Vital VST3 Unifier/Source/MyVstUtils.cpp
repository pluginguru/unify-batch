#include "MyVstUtils.h"

void MyVstConv::setChunkData(const void* data, const int size, bool /*isPreset*/)
{
    memoryBlock.setSize(size);
    memoryBlock.copyFrom(data, 0, size);
}

void MyVstConv::getChunkData(MemoryBlock& mb, bool /*isPreset*/, int /*maxSizeMB*/)
{
    mb.setSize(memoryBlock.getSize());
    mb.copyFrom(memoryBlock.getData(), 0, memoryBlock.getSize());
}

void MyVstConv::changeProgramName(int /*index*/, const String& newName)
{
    programName = newName;
}

void MyVstConv::setParamValue(int index, const float value)
{
    paramValues.set(index, value);
}

// Compares a magic value in either endianness.
static bool compareMagic(int32 magic, const char* name) noexcept
{
    return magic == (int32)ByteOrder::littleEndianInt(name)
        || magic == (int32)ByteOrder::bigEndianInt(name);
}

static int32 fxbName(const char* name) noexcept { return (int32)ByteOrder::littleEndianInt(name); }
static int32 fxbSwap(int32 x) noexcept { return (int32)ByteOrder::swapIfLittleEndian((uint32)x); }

static float fxbSwapFloat(const float x) noexcept
{
#ifdef JUCE_LITTLE_ENDIAN
    union { uint32 asInt; float asFloat; } n;
    n.asFloat = x;
    n.asInt = ByteOrder::swap(n.asInt);
    return n.asFloat;
#else
    return x;
#endif
}

bool MyVstConv::restoreProgramSettings(const fxProgram* const prog)
{
    if (compareMagic(prog->chunkMagic, "CcnK")
        && compareMagic(prog->fxMagic, "FxCk"))
    {
        changeProgramName(getCurrentProgram(), prog->prgName);

        for (int i = 0; i < fxbSwap(prog->numParams); ++i)
            //if (auto* param = getParameters()[i])
            //    param->setValue(fxbSwapFloat(prog->params[i]));
            setParamValue(i, fxbSwapFloat(prog->params[i]));

        return true;
    }

    return false;
}

void MyVstConv::createTempParameterStore(MemoryBlock& dest)
{
    auto numParameters = getNumParams();// getParameters().size();
    dest.setSize(64 + 4 * (size_t)numParameters);
    dest.fillWith(0);

    getCurrentProgramName().copyToUTF8((char*)dest.getData(), 63);

    auto p = (float*)(((char*)dest.getData()) + 64);

    for (int i = 0; i < numParameters; ++i)
        //if (auto* param = getParameters()[i])
        //    p[i] = param->getValue();
        p[i] = getParamValue(i);
}

void MyVstConv::restoreFromTempParameterStore(const MemoryBlock& m)
{
    changeProgramName(getCurrentProgram(), (const char*)m.getData());

    auto p = (float*)(((char*)m.getData()) + 64);
    auto numParameters = getNumParams();// getParameters().size();

    for (int i = 0; i < numParameters; ++i)
        //if (auto* param = getParameters()[i])
        //    param->setValue(p[i]);
        setParamValue(i, p[i]);
}

void MyVstConv::setParamsInProgramBlock(fxProgram* prog)
{
    auto numParams = getNumParams();// getParameters().size();

    prog->chunkMagic = fxbName("CcnK");
    prog->byteSize = 0;
    prog->fxMagic = fxbName("FxCk");
    prog->version = fxbSwap(fxbVersionNum);
    prog->fxID = fxbSwap(getUID());
    prog->fxVersion = fxbSwap(getVersionNumber());
    prog->numParams = fxbSwap(numParams);

    getCurrentProgramName().copyToUTF8(prog->prgName, sizeof(prog->prgName) - 1);

    for (int i = 0; i < numParams; ++i)
        //if (auto* param = getParameters()[i])
        //    prog->params[i] = fxbSwapFloat(param->getValue());
        prog->params[i] = fxbSwapFloat(getParamValue(i));
}

bool MyVstConv::loadFromFXBFile (const void* const data, const size_t dataSize)
{
    if (dataSize < 28)
        return false;

    auto set = (const fxSet*) data;

    if ((! compareMagic (set->chunkMagic, "CcnK")) || fxbSwap (set->version) > fxbVersionNum)
        return false;

    if (compareMagic (set->fxMagic, "FxBk"))
    {
        // bank of programs
        //DBG("loadFromFXBFile: Bank of programs FxBk");
        if (fxbSwap (set->numPrograms) >= 0)
        {
            auto oldProg = getCurrentProgram();
            auto numParams = fxbSwap (((const fxProgram*) (set->programs))->numParams);
            auto progLen = (int) sizeof (fxProgram) + (numParams - 1) * (int) sizeof (float);

            for (int i = 0; i < fxbSwap (set->numPrograms); ++i)
            {
                if (i != oldProg)
                {
                    auto prog = (const fxProgram*) (((const char*) (set->programs)) + i * progLen);

                    if (((const char*) prog) - ((const char*) set) >= (ssize_t) dataSize)
                        return false;

                    if (fxbSwap (set->numPrograms) > 0)
                        setCurrentProgram (i);

                    if (! restoreProgramSettings (prog))
                        return false;
                }
            }

            if (fxbSwap (set->numPrograms) > 0)
                setCurrentProgram (oldProg);

            auto prog = (const fxProgram*) (((const char*) (set->programs)) + oldProg * progLen);

            if (((const char*) prog) - ((const char*) set) >= (ssize_t) dataSize)
                return false;

            if (! restoreProgramSettings (prog))
                return false;
        }
    }
    else if (compareMagic (set->fxMagic, "FxCk"))
    {
        // single program
        //DBG("loadFromFXBFile: Single program FxCk");
        auto prog = (const fxProgram*) data;

        if (! compareMagic (prog->chunkMagic, "CcnK"))
            return false;

        changeProgramName (getCurrentProgram(), prog->prgName);

        for (int i = 0; i < fxbSwap (prog->numParams); ++i)
            //if (auto* param = getParameters()[i])
            //    param->setValue (fxbSwapFloat (prog->params[i]));
            setParamValue(i, fxbSwapFloat(prog->params[i]));
    }
    else if (compareMagic (set->fxMagic, "FBCh"))
    {
        // non-preset chunk
        //DBG("loadFromFXBFile: Non-preset chunk FBCh");
        auto cset = (const fxChunkSet*) data;

        if ((size_t) fxbSwap (cset->chunkSize) + sizeof (fxChunkSet) - 8 > (size_t) dataSize)
            return false;

        setChunkData (cset->chunk, fxbSwap (cset->chunkSize), false);
    }
    else if (compareMagic (set->fxMagic, "FPCh"))
    {
        // preset chunk
        //DBG("loadFromFXBFile: Preset chunk FPCh");
        auto cset = (const fxProgramSet*) data;

        if ((size_t) fxbSwap (cset->chunkSize) + sizeof (fxProgramSet) - 8 > (size_t) dataSize)
            return false;

        setChunkData (cset->chunk, fxbSwap (cset->chunkSize), true);

        changeProgramName (getCurrentProgram(), cset->name);
    }
    else
    {
        return false;
    }

    return true;
}

bool MyVstConv::saveToFXBFile (MemoryBlock& dest, bool isFXB, int maxSizeMB)
{
    auto numPrograms = getNumPrograms();
    auto numParams = getNumParams();// getParameters().size();

    if (usesChunks())
    {
        MemoryBlock chunk;
        getChunkData (chunk, ! isFXB, maxSizeMB);

        if (isFXB)
        {
            auto totalLen = sizeof (fxChunkSet) + chunk.getSize() - 8;
            dest.setSize (totalLen, true);

            auto set = (fxChunkSet*) dest.getData();
            set->chunkMagic = fxbName ("CcnK");
            set->byteSize = 0;
            set->fxMagic = fxbName ("FBCh");
            set->version = fxbSwap (fxbVersionNum);
            set->fxID = fxbSwap (getUID());
            set->fxVersion = fxbSwap (getVersionNumber());
            set->numPrograms = fxbSwap (numPrograms);
            set->chunkSize = fxbSwap ((int32) chunk.getSize());

            chunk.copyTo (set->chunk, 0, chunk.getSize());
        }
        else
        {
            auto totalLen = sizeof (fxProgramSet) + chunk.getSize() - 8;
            dest.setSize (totalLen, true);

            auto set = (fxProgramSet*) dest.getData();
            set->chunkMagic = fxbName ("CcnK");
            set->byteSize = 0;
            set->fxMagic = fxbName ("FPCh");
            set->version = fxbSwap (fxbVersionNum);
            set->fxID = fxbSwap (getUID());
            set->fxVersion = fxbSwap (getVersionNumber());
            set->numPrograms = fxbSwap (numPrograms);
            set->chunkSize = fxbSwap ((int32) chunk.getSize());

            getCurrentProgramName().copyToUTF8 (set->name, sizeof (set->name) - 1);
            chunk.copyTo (set->chunk, 0, chunk.getSize());
        }
    }
    else
    {
        if (isFXB)
        {
            auto progLen = (int) sizeof (fxProgram) + (numParams - 1) * (int) sizeof (float);
            auto len = (sizeof (fxSet) - sizeof (fxProgram)) + (size_t) (progLen * jmax (1, numPrograms));
            dest.setSize (len, true);

            auto set = (fxSet*) dest.getData();
            set->chunkMagic = fxbName ("CcnK");
            set->byteSize = 0;
            set->fxMagic = fxbName ("FxBk");
            set->version = fxbSwap (fxbVersionNum);
            set->fxID = fxbSwap (getUID());
            set->fxVersion = fxbSwap (getVersionNumber());
            set->numPrograms = fxbSwap (numPrograms);

            MemoryBlock oldSettings;
            createTempParameterStore (oldSettings);

            auto oldProgram = getCurrentProgram();

            if (oldProgram >= 0)
                setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + oldProgram * progLen));

            for (int i = 0; i < numPrograms; ++i)
            {
                if (i != oldProgram)
                {
                    setCurrentProgram (i);
                    setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + i * progLen));
                }
            }

            if (oldProgram >= 0)
                setCurrentProgram (oldProgram);

            restoreFromTempParameterStore (oldSettings);
        }
        else
        {
            dest.setSize (sizeof (fxProgram) + (size_t) ((numParams - 1) * (int) sizeof (float)), true);
            setParamsInProgramBlock ((fxProgram*) dest.getData());
        }
    }

    return true;
}

XmlElement* decodeVstPluginState(AudioPluginInstance* instance, MemoryBlock& memoryBlock)
{
    MyVstConv vstConv;
    vstConv.loadFromFXBFile(memoryBlock.getData(), memoryBlock.getSize());

    if (vstConv.paramValues.size() > 0)
        return nullptr;

    std::unique_ptr<XmlElement> xml = AudioProcessor::getXmlFromBinary(vstConv.memoryBlock.getData(), vstConv.memoryBlock.getSize());
    if (!xml)
        return nullptr;
    else
        return xml.release();
}

void encodeVstPluginState(XmlElement* xml, AudioPluginInstance* instance, MemoryBlock& memoryBlock)
{
    MemoryBlock stateInformation;
    AudioProcessor::copyXmlToBinary(*xml, stateInformation);
    encodeVstPluginState(stateInformation, instance, memoryBlock);
}

void encodeVstPluginState(MemoryBlock& stateInformation, AudioPluginInstance* instance, MemoryBlock& vstState)
{
    auto totalLen = sizeof(MyVstConv::fxProgramSet) + stateInformation.getSize() - 8;
    vstState.reset();
    vstState.setSize(totalLen, true);

    auto set = (MyVstConv::fxProgramSet*)vstState.getData();
    set->chunkMagic = fxbName("CcnK");
    set->byteSize = 0;
    set->fxMagic = fxbName("FPCh");
    set->version = fxbSwap(MyVstConv::fxbVersionNum);
    set->fxID = fxbSwap(instance->getPluginDescription().uniqueId);
    set->fxVersion = fxbSwap(MyVstConv::fxbVersionNum);
    set->numPrograms = fxbSwap(instance->getNumPrograms());
    String progName("default");
    progName.copyToUTF8(set->name, sizeof(set->name) - 1);
    set->chunkSize = fxbSwap((int32)stateInformation.getSize());
    stateInformation.copyTo(set->chunk, 0, stateInformation.getSize());
}

XmlElement* getVstPluginState(AudioPluginInstance* instance)
{
    MemoryBlock memoryBlock;
    instance->getCurrentProgramStateInformation(memoryBlock);

    MyVstConv vstConv;
    vstConv.loadFromFXBFile(memoryBlock.getData(), memoryBlock.getSize());

    XmlElement* xml;

    if (vstConv.paramValues.size() > 0)
    {
        xml = new XmlElement("pluginParams");
        xml->setAttribute("pluginName", instance->getName());
        int progNum = instance->getCurrentProgram();
        xml->setAttribute("currentProgram", progNum);
        xml->setAttribute("programName", instance->getProgramName(progNum));

        instance->refreshParameterList();
        auto& params = instance->getParameters();
        for (int i = 0; i < params.size(); i++)
        {
            auto p = params[i];
            XmlElement* paramXml = new XmlElement("param");
            xml->addChildElement(paramXml);
            paramXml->setAttribute("index", i);
            String paramName = p->getName(100);
            if (paramName.isEmpty()) paramName = "p" + String(i);
            paramXml->setAttribute("name", paramName);
            paramXml->setAttribute("value", double(p->getValue()));
        }
    }
    else
    {
        xml = new XmlElement("pluginPreset");
        xml->setAttribute("pluginName", instance->getName());
        xml->setAttribute("stateInformation", vstConv.memoryBlock.toBase64Encoding());
        int progNum = instance->getCurrentProgram();
        xml->setAttribute("currentProgram", progNum);
        xml->setAttribute("programName", instance->getProgramName(progNum));
    }

    return xml;
}

void setVstPluginState(AudioPluginInstance* instance, XmlElement* xml, String& errorMessage)
{
    if (xml->hasTagName("pluginParams"))
    {
        if (xml->getStringAttribute("pluginName") != instance->getName())
            errorMessage = "Selected file does not contain parameter data for " + instance->getName();
        else
        {
            int progNum = xml->getIntAttribute("currentProgram", 0);
            instance->setCurrentProgram(progNum);
            instance->changeProgramName(progNum, xml->getStringAttribute("programName"));

            instance->refreshParameterList();
            auto& params = instance->getParameters();
            for (auto paramXml : xml->getChildWithTagNameIterator("PARAM"))
            {
                int index = paramXml->getIntAttribute("index");
                float value = float(paramXml->getDoubleAttribute("value"));
                params[index]->setValue(value);
            }
        }
    }
    else if (xml->hasTagName("pluginPreset"))
    {
        if (xml->getStringAttribute("pluginName") != instance->getName())
            errorMessage = "Selected file does not contain preset data for " + instance->getName();
        else
        {
            int progNum = xml->getIntAttribute("currentProgram", 0);
            String progName = xml->getStringAttribute("programName");
            instance->setCurrentProgram(progNum);

            MemoryBlock stateInformation;
            stateInformation.fromBase64Encoding(xml->getStringAttribute("stateInformation"));

            auto totalLen = sizeof(MyVstConv::fxProgramSet) + stateInformation.getSize() - 8;
            MemoryBlock memoryBlock;
            memoryBlock.setSize(totalLen, true);

            auto set = (MyVstConv::fxProgramSet*)memoryBlock.getData();
            set->chunkMagic = fxbName("CcnK");
            set->byteSize = 0;
            set->fxMagic = fxbName("FPCh");
            set->version = fxbSwap(MyVstConv::fxbVersionNum);
            set->fxID = fxbSwap(instance->getPluginDescription().uniqueId);
            set->fxVersion = fxbSwap(MyVstConv::fxbVersionNum);
            set->numPrograms = fxbSwap(instance->getNumPrograms());
            progName.copyToUTF8(set->name, sizeof(set->name) - 1);
            set->chunkSize = fxbSwap((int32)stateInformation.getSize());
            stateInformation.copyTo(set->chunk, 0, stateInformation.getSize());

            instance->setCurrentProgramStateInformation(memoryBlock.getData(), int(memoryBlock.getSize()));
            instance->changeProgramName(progNum, progName);
        }
    }
    else
    {
        errorMessage = "Selected file does not contain parameter or state data";
    }
}
