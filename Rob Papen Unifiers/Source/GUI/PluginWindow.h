#pragma once
#include <JuceHeader.h>

class PluginWindow  : public DocumentWindow
                    , public ChangeBroadcaster
                    , protected AudioProcessorListener
                    , protected Timer
{
public:
    PluginWindow(AudioProcessor* thePlugin, StringRef title, int leftX, int topY,
                 bool useNativeTitleBar, bool alwaysOnTop, bool allowScaling);
    ~PluginWindow();

    // DocumentWindow
    bool checkLastChangedParameter(int& index);

    // PluginWindow
    AudioProcessorEditor* getEditor() { return editor.get(); }

protected:
    void closeButtonPressed() override;

    float getDesktopScaleFactor() const override
    {
        return overrideScaling ? 1.0f : DocumentWindow::getDesktopScaleFactor();
    }

    void audioProcessorParameterChanged(AudioProcessor*, int index, float) override
    {
        lastChangedParameterIndex = index;
    }

#if JUCE_PROJUCER_VERSION > 0x60007
    void audioProcessorChanged(AudioProcessor* processor, const ChangeDetails& details) override {}
#else
    void audioProcessorChanged(AudioProcessor* processor) override {}
#endif

    void timerCallback() override
    {
        int lcpi = lastChangedParameterIndex.load();
        if (lcpi >= 0)
        {
            lastChangedParameterIndex = -1;
            lastChangedParameterIndexForDisplay = lcpi;
            sendChangeMessage();
        }
    }

private:
    AudioProcessor* plugin;
    std::unique_ptr<AudioProcessorEditor> editor;
    bool overrideScaling;

    String pluginName;
    std::atomic<int> lastChangedParameterIndex;
    int lastChangedParameterIndexForDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWindow)
};
