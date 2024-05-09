#include "WrapperProcessor.h"
#include "PapenUnifierEditor.h"

PapenUnifierEditor::PapenUnifierEditor (WrapperProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
{
    auto instance = processor.getPluginInstance();
    if (instance && instance->hasEditor())
    {
        // open a separate window with the plugin's own existing GUI
        pluginWindow.reset(new PluginWindow(instance, "Plugin GUI", 20, 20, false, false, false));
        pluginWindow->addChangeListener(this);
    }

    message = "Edit settings below, then drag __CatIndex.ind file here";

    // Output path
    outputPathLabel.setJustificationType(Justification::topLeft);
    outputPathLabel.setText("Click to set. If not set, files cannot be processed.", dontSendNotification);
    addAndMakeVisible(outputPathLabel);

    outputFolderPathLabel.setText("Output Folder:", dontSendNotification);
    outputFolderPathLabel.setJustificationType(Justification::right);
    outputFolderPathLabel.attachToComponent(&outputFolderPath, true);
    addAndMakeVisible(outputFolderPath);
    outputFolderPath.onClick = [this]()
        {
            FileChooser chooser("Select output folder");
            if (chooser.browseForDirectory())
            {
                outputFolder = chooser.getResult();
                String path = chooser.getResult().getFullPathName();
                outputFolderPath.setButtonText(path);

                libraryName.setText(outputFolder.getParentDirectory().getFileName());
            }
        };

    libraryNameLabel.setText("Library Name", dontSendNotification);
    libraryNameLabel.attachToComponent(&libraryName, true);
    addAndMakeVisible(libraryName);

    authorLabel.setText("Author", dontSendNotification);
    authorLabel.attachToComponent(&author, true);
    addAndMakeVisible(author);

    tagsLabel.setText("Tags", dontSendNotification);
    tagsLabel.attachToComponent(&tags, true);
    addAndMakeVisible(tags);

    commentLabel.setText("Comment", dontSendNotification);
    commentLabel.attachToComponent(&comment, true);
    comment.setMultiLine(true);
    addAndMakeVisible(comment);

    setSize(800, 600);
}

PapenUnifierEditor::~PapenUnifierEditor()
{
}

void PapenUnifierEditor::resized()
{
    auto bounds = getLocalBounds().reduced(40, 20);
    bounds.removeFromTop(80);		// space for message (and a gap below)

    bounds.removeFromLeft(120);		// space for labels
    bounds.removeFromRight(20);		// balance space on right

    bounds.removeFromTop(30);
    outputPathLabel.setBounds(bounds.removeFromTop(26));
    auto row = bounds.removeFromTop(24);
    outputFolderPath.setBounds(row);

    bounds.removeFromTop(30);
    libraryName.setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(10);
    author.setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(10);
    tags.setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(10);
    comment.setBounds(bounds.removeFromTop(10 * 24));
}

void PapenUnifierEditor::paint (Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    auto messageBox = getLocalBounds().reduced(40, 20).removeFromTop(60);

    g.setFont(Font(16.0f));
    g.setColour(Colours::white);
    g.drawText(message, messageBox, Justification::centred);
}

void PapenUnifierEditor::changeListenerCallback(ChangeBroadcaster* cb)
{
    PluginWindow* senderPluginWindow = dynamic_cast<PluginWindow*>(cb);
    if (senderPluginWindow)
    {
        int index;
        float value;
        if (pluginWindow->checkLastChangedParameter(index))
        {
            // user changed a plugin parameter
        }
        else
        {
            // window closed by user
            pluginWindow = nullptr;
        }
    }
}

bool PapenUnifierEditor::isInterestedInFileDrag(const StringArray& paths)
{
    return (paths.size() == 1 && paths[0].endsWith("__CatIndex.ind"));
}

void PapenUnifierEditor::filesDropped(const StringArray& paths, int, int)
{
    if (outputFolder.getFullPathName().isEmpty())
    {
        message = "Must set output file path first";
    }
    else
    {
        message = "0 files processed";

        processor.libraryName = libraryName.getText();
        processor.author = author.getText();
        processor.tags = tags.getText();
        processor.comment = comment.getText();

        processor.bankIndexFile = File(paths[0]);
        processor.outputFolder = outputFolder;
        processor.startThread();
        startTimer(250);
    }
    repaint();
}

void PapenUnifierEditor::timerCallback()
{
    message = String(processor.getCount()) + " files processed.";

    if (!processor.isThreadRunning())
    {
        message = "DONE: " + message;
        stopTimer();
    }

    repaint();
}
