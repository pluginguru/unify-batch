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
	updateGuruSamplerLibraryToggle.setButtonText("Update Guru Sampler [2] library references");
	updateGuruSamplerLibraryToggle.setToggleState(true, dontSendNotification);
	addAndMakeVisible(updateGuruSamplerLibraryToggle);
	updateGuruSamplerLibraryToggle.onClick = [this] {
		patchConverter.updateGuruSamplerLibraryName = updateGuruSamplerLibraryToggle.getToggleState();
		};

	updateGuruSamplerFolderToggle.setButtonText("Change samples folder name to:");
	updateGuruSamplerFolderToggle.setToggleState(false, dontSendNotification);
	addAndMakeVisible(updateGuruSamplerFolderToggle);
	updateGuruSamplerFolderToggle.onClick = [this] {
		patchConverter.updateGuruSamplerFolderName = updateGuruSamplerFolderToggle.getToggleState();
		};

	sampleFolder.setTextToShowWhenEmpty("Enter new sample folder name here (if needed)", darkText);
	addAndMakeVisible(sampleFolder);

	// Toggle whether or not to update MIDI Box Library reference name.
	updateMIDIBoxToggle.setButtonText("Update MIDIBox library references");
	updateMIDIBoxToggle.setToggleState(true, dontSendNotification);
	addAndMakeVisible(updateMIDIBoxToggle);
	updateMIDIBoxToggle.onClick = [this] {
		patchConverter.updateMIDIBoxLibraryName = updateMIDIBoxToggle.getToggleState();
		};

    setSize(800, 300);
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
	bounds.removeFromTop(24);

	bounds.removeFromLeft(-80);
	bounds.removeFromRight(20);

	auto row = bounds.removeFromTop(20);
	updateGuruSamplerLibraryToggle.setBounds(row.removeFromLeft(320));
	updateMIDIBoxToggle.setBounds(row.removeFromRight(240));
	bounds.removeFromTop(20);

	row = bounds.removeFromTop(24);
	updateGuruSamplerFolderToggle.setBounds(row.removeFromLeft(230));
	row.removeFromLeft(4);
	sampleFolder.setBounds(row);
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
	patchConverter.guruSamplerFolderName = sampleFolder.getText();
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
