#include "MainComponent.h"
#include <stdlib.h>
#include <time.h>

MainComponent::MainComponent()
{
    PropertiesFile::Options options;
    options.applicationName = "Unify";
    options.filenameSuffix = ".settings";
    options.folderName = "PlugInGuru";
    options.osxLibrarySubFolder = "Application Support";
    settings.reset(new PropertiesFile(options));
    patchConverter.unifyRootFolderPath = settings->getValue("dataDirectoryPath");

    srand(unsigned(time(0)));
    Colour darkText = getLookAndFeel().findColour(ComboBox::textColourId).withMultipliedAlpha(0.5f);

    message = "Edit settings below, then drag .unify files or folders here";

    libraryLabel.setText("Library Name:", dontSendNotification);
	libraryLabel.setJustificationType(Justification::right);
	libraryLabel.attachToComponent(&library, true);
    library.setTextToShowWhenEmpty("Enter new library name here", darkText);
    addAndMakeVisible(library);

	outputFolderPathLabel.setText("Output Folder:", dontSendNotification);
	outputFolderPathLabel.setJustificationType(Justification::right);
	outputFolderPathLabel.attachToComponent(&outputFolderPath, true);
	addAndMakeVisible(outputFolderPath);
	outputFolderPath.onClick = [this]()
	{
		FileChooser chooser("Select output folder");
		if (chooser.browseForDirectory())
		{
			String path = chooser.getResult().getFullPathName();
			outputFolderPath.setButtonText(path);
			patchConverter.outputFolderPath = path;
		}
	};

	// Toggle whether or not to update Guru Sampler Library Name.
	updateGuruSamplerLibraryToggle.setButtonText("Update Guru Sampler Library reference?  "
		"Checked will update library name to new name.  Unchecked will leave reference as-is.");
	updateGuruSamplerLibraryToggle.setToggleState(true, dontSendNotification);
	addAndMakeVisible(updateGuruSamplerLibraryToggle);
	updateGuruSamplerLibraryToggle.onClick = [this] {
		patchConverter.updateGuruSamplerLibraryName = updateGuruSamplerLibraryToggle.getToggleState();
		};

	// Toggle whether or not to update MIDI Box Library reference name.
	updateMIDIBoxToggle.setButtonText("Update MIDI Box reference name?  "
		"Checked will update library name to new library name.  Unchecked will leave reference as-is.");
	updateMIDIBoxToggle.setToggleState(true, dontSendNotification);
	addAndMakeVisible(updateMIDIBoxToggle);
	updateMIDIBoxToggle.onClick = [this] {
		patchConverter.updateMIDIBoxName = updateMIDIBoxToggle.getToggleState();
		};

    setSize(800, 310);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(40, 20);
	bounds.removeFromTop(80);		// space for message (and a gap below)

	bounds.removeFromLeft(120);		// space for labels
	bounds.removeFromRight(20);		// balance space on right

    library.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(20);
	outputFolderPath.setBounds(bounds.removeFromTop(24));
	bounds.removeFromTop(20);
	updateGuruSamplerLibraryToggle.setBounds(bounds.removeFromTop(35));
	bounds.removeFromTop(25);
	updateMIDIBoxToggle.setBounds(bounds.removeFromTop(55));
}

void MainComponent::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	auto messageBox = getLocalBounds().reduced(40, 20).removeFromTop(60);

	g.setFont(Font(16.0f));
	g.setColour(Colours::white);
	g.drawText(message, messageBox, Justification::centred);
}

bool MainComponent::isInterestedInFileDrag(const StringArray&)
{
    return true;
}

void MainComponent::filesDropped(const StringArray& filePaths, int, int)
{
	patchConverter.libraryName	= library.getText();
	if (patchConverter.libraryName.isEmpty())
	{
		message = "You must specify the new library name";
	}
	else
	{
		int fileCount = patchConverter.processFiles(filePaths);
		message = String(fileCount) + " files processed.";
	}
	repaint();
}
