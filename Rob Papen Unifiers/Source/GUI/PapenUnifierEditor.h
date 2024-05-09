#pragma once
#include <JuceHeader.h>
#include "WrapperProcessor.h"
#include "PluginWindow.h"

class PapenUnifierEditor    : public AudioProcessorEditor
                            , protected ChangeListener
                            , public FileDragAndDropTarget
{
public:
    PapenUnifierEditor (WrapperProcessor&);
    ~PapenUnifierEditor();

    void paint (Graphics&) override;
    void resized() override;

    Point<int> getWindowTopRight() { return getScreenPosition() + Point<int>(getWidth(), 0); }

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const StringArray&) override;
    void filesDropped(const StringArray&, int, int) override;

protected:
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    WrapperProcessor& processor;

    std::unique_ptr<PluginWindow> pluginWindow;

    Label outputPathLabel;
    TextButton outputFolderPath; Label outputFolderPathLabel;

    Label libraryNameLabel; TextEditor libraryName;
    Label authorLabel; TextEditor author;
    Label prefixLabel; TextEditor prefix;
    Label categoryLabel; TextEditor category;
    Label tagsLabel; TextEditor tags;
    Label commentLabel; TextEditor comment;

    String message;

    File outputFolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PapenUnifierEditor)
};
