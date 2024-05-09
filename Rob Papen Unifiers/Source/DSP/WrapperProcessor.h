#pragma once
#include <JuceHeader.h>
#include "PapenBankConverter.h"

class WrapperProcessor  : public AudioProcessor
                        , public PapenBankConverter
{
public:
    WrapperProcessor(AudioPluginInstance*);
    virtual ~WrapperProcessor();

    const String getName() const override { return plugin->getName(); }
    bool canAddBus(bool inputBus) const override { return plugin->canAddBus(inputBus); }
    bool canRemoveBus(bool inputBus) const override { return plugin->canRemoveBus(inputBus); }
    bool supportsDoublePrecisionProcessing() const override { return plugin->supportsDoublePrecisionProcessing(); }
    double getTailLengthSeconds() const override { return plugin->getTailLengthSeconds(); }

    bool acceptsMidi() const override { return plugin->acceptsMidi(); }
    bool producesMidi() const override { return plugin->producesMidi(); }
    bool supportsMPE() const override { return plugin->supportsMPE(); }
    bool isMidiEffect() const override { return plugin->isMidiEffect(); }

    void reset() override { plugin->reset(); }

    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override;

    int getNumPrograms() override { return plugin->getNumPrograms(); }
    int getCurrentProgram() override { return plugin->getCurrentProgram(); }
    void setCurrentProgram(int index) override { plugin->setCurrentProgram(index); }
    const String getProgramName(int index) override { return plugin->getProgramName(index); }
    void changeProgramName(int index, const String& newName) override { plugin->changeProgramName(index, newName); }

    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    void getStateInformation(juce::MemoryBlock& destData) override { plugin->getStateInformation(destData); }
    void setStateInformation(const void* data, int sizeInBytes) override { plugin->setStateInformation(data, sizeInBytes); }
    void setCurrentProgramStateInformation(const void* data, int bytes) override { plugin->setCurrentProgramStateInformation(data, bytes); }
    void getCurrentProgramStateInformation(juce::MemoryBlock& destData) override { plugin->getCurrentProgramStateInformation(destData); }

    // Editor interface
    AudioPluginInstance* getPluginInstance() { return plugin.get(); }
    void sendProgramChange(int);

protected:
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return plugin->checkBusesLayoutSupported(layouts); }
    bool canApplyBusesLayout(const BusesLayout& layouts) const override {
        plugin->releaseResources(); return plugin->setBusesLayout(layouts);
    }
    bool canApplyBusCountChange(bool isInput, bool isAddingBuses, BusProperties& outNewBusProperties) override;

    static BusesProperties getBusesPropertiesFromProcessor(AudioProcessor* processor);

private:
    Array<AudioProcessorParameter*> pluginParams;
    MemoryBlock pluginState;

    int progNum;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrapperProcessor)
};
