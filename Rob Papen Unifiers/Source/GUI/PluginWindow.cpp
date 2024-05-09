#include "PluginWindow.h"

PluginWindow::PluginWindow(AudioProcessor* thePlugin, StringRef title, int leftX, int topY,
                           bool useNativeTitleBar, bool alwaysOnTop, bool allowScaling)
    : DocumentWindow(title, Colours::lightgrey, TitleBarButtons::closeButton)
    , plugin(thePlugin)
    , overrideScaling(!allowScaling)
    , lastChangedParameterIndex(-1)
    , lastChangedParameterIndexForDisplay(-1)
{
    // Needed for Mac: JUCE default is 128, 128, 32768, 32768
    setResizeLimits(64, 64, 32768, 32768);

    setName(title);
    setUsingNativeTitleBar(useNativeTitleBar);

    if (plugin->hasEditor())
    {
        editor.reset(plugin->createEditorIfNeeded());

        // Apparently not all plug-ins report that they are resizable, even though they are.
        // Here we make a special exception for the Reason Rack plug-ins.
        // TODO: expand this into a centralized system of exception managers, so we can
        // add many different kinds of exceptions in future.
        bool resizable = editor->isResizable();
        if (plugin->getName().startsWith("Reason Rack"))
            resizable = true;
        setResizable(resizable, false);
    }
    else return;

    editor->setOpaque(true);

    setContentNonOwned(editor.get(), true);
    editor->setTopLeftPosition(1, 1 + getTitleBarHeight());

    setTopLeftPosition(leftX, topY);
    setAlwaysOnTop(alwaysOnTop);
    setVisible(true);

    pluginName = plugin->getName();
    plugin->addListener(this);

    startTimerHz(10);
}

void PluginWindow::closeButtonPressed()
{
    plugin->editorBeingDeleted(editor.get());
    editor = nullptr;
    sendChangeMessage();
}

PluginWindow::~PluginWindow()
{
    plugin->removeListener(this);
    if (editor) closeButtonPressed();
}

bool PluginWindow::checkLastChangedParameter(int& index)
{
    if (editor)
    {
        index = lastChangedParameterIndexForDisplay;
        return true;
    }
    return false;
}
