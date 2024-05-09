#include "WrapperProcessor.h"
#include "PapenUnifierEditor.h"

#define PLUGIN_NAME "BIT2"
#define PLUGIN_TYPE "VST"
#define PLUGIN_MFR "Rob Papen"

#define PRESET_CHANGE_TIME_MILLISECONDS 400

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
   AudioPluginFormatManager pluginFormatManager;
    pluginFormatManager.addDefaultFormats();

    // Get AudioPluginHost's PluginList.xml
    PropertiesFile::Options options;
#if 1
    options.applicationName = "PluginList";
    options.filenameSuffix = ".xml";
    options.folderName = "PlugInGuru";
    options.osxLibrarySubFolder = "Application Support";
#else
    options.applicationName = "Juce Audio Plugin Host";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Preferences";
#endif
    std::unique_ptr<PropertiesFile> knownPluginsFile;
    knownPluginsFile.reset(new PropertiesFile(options));

    std::unique_ptr<XmlElement> savedPluginList(knownPluginsFile->getXmlValue("pluginList"));
    if (savedPluginList)
    {
        KnownPluginList knownPluginList;
        knownPluginList.recreateFromXml(*savedPluginList);
        auto knownTypes = knownPluginList.getTypes();

        // Find the entry for chosen plug-in
        PluginDescription pid = knownTypes[0];
        for (auto& desc : knownTypes)
        {
            if (desc.pluginFormatName == PLUGIN_TYPE &&
                desc.name.startsWithIgnoreCase(PLUGIN_NAME) &&
                desc.manufacturerName.startsWithIgnoreCase(PLUGIN_MFR))
            {
                pid = desc;
                break;
            }
        }

        String errorMessage;
        AudioPluginInstance* instance = pluginFormatManager.createPluginInstance(pid, 44100.0, 512, errorMessage).release();
        if (instance)
            return new WrapperProcessor(instance);
        
        DBG("Error instantiating " + pid.name);
        DBG(" ->" + errorMessage);
    }

    return nullptr;
}

WrapperProcessor::WrapperProcessor(AudioPluginInstance* processorToUse)
    : AudioProcessor(getBusesPropertiesFromProcessor(processorToUse))
    , progNum(-1)
{
    plugin.reset(processorToUse);
    pluginParams = plugin->getParameters();
    for (auto param : pluginParams)
    {
        int paramIndex = param->getParameterIndex();
        String name = param->getName(100);
        float value = param->getValue();
        DBG(String(paramIndex) + ": " + name + " = " + String(value));
    }

    selectPreset = [this](int presetIndex)
        {
            progNum = presetIndex;
            Thread::sleep(PRESET_CHANGE_TIME_MILLISECONDS);
        };
}

WrapperProcessor::~WrapperProcessor()
{
    releaseResources();
}

AudioProcessor::BusesProperties WrapperProcessor::getBusesPropertiesFromProcessor(AudioProcessor* processor)
{
    BusesProperties retval;

    for (int dir = 0; dir < 2; ++dir)
    {
        const bool isInput = (dir == 0);
        const int n = processor->getBusCount(isInput);

        for (int i = 0; i < n; ++i)
            if (AudioProcessor::Bus* bus = processor->getBus(isInput, i))
                retval.addBus(isInput, bus->getName(), bus->getDefaultLayout(), bus->isEnabledByDefault());
    }

    return retval;
}

AudioProcessorEditor* WrapperProcessor::createEditor()
{
    return new PapenUnifierEditor(*this);
}

void WrapperProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    plugin->releaseResources();
    plugin->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);

    // sync number of buses
    for (int dir = 0; dir < 2; ++dir)
    {
        const bool isInput = (dir == 0);
        int expectedNumBuses = getBusCount(isInput);
        int requiredNumBuses = plugin->getBusCount(isInput);

        for (; expectedNumBuses < requiredNumBuses; expectedNumBuses++)
            plugin->addBus(isInput);

        for (; requiredNumBuses < expectedNumBuses; requiredNumBuses++)
            plugin->removeBus(isInput);
    }

    plugin->setBusesLayout(getBusesLayout());
    plugin->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void WrapperProcessor::releaseResources()
{
    plugin->releaseResources();
}

bool WrapperProcessor::canApplyBusCountChange(bool isInput, bool isAddingBuses,
    BusProperties& outNewBusProperties)
{
    if (isAddingBuses)
    {
        int busIdx = plugin->getBusCount(isInput);

        if (!plugin->addBus(isInput))
            return false;

        if (Bus* bus = plugin->getBus(isInput, busIdx))
        {
            outNewBusProperties.busName = bus->getName();
            outNewBusProperties.defaultLayout = bus->getDefaultLayout();
            outNewBusProperties.isActivatedByDefault = bus->isEnabledByDefault();

            return true;
        }
        else
        {
            jassertfalse;
            return false;
        }
    }
    else
        return plugin->removeBus(isInput);
}

void WrapperProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    bool sendProgChange = isPositiveAndBelow(progNum, 128);
    if (sendProgChange)
    {
        midiMessages.addEvent(MidiMessage::programChange(1, progNum), midiMessages.getLastEventTime());
        progNum = -1;
    }

    plugin->processBlock(buffer, midiMessages);
}

void WrapperProcessor::sendProgramChange(int pnum)
{
    const ScopedLock lock(getCallbackLock());
    progNum = pnum;
}
