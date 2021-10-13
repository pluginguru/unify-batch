# Synth1 Unifier

This is a unifier for the popular free [*Synth1* VST synthesizer by Daichi Laboratory](https://daichilab.com/synth1/), which can be downloaded from [Plugins4Free.com](https://plugins4free.com/plugin/245/) (links on the Daichi web page are invalid.)

*Synth1* uses its own (undocumented) preset format consisting of *.sy1* files, whose names are numbers 001-128, corresponding to program numbers 0-127 in MIDI program-change messages, and *Synth1* does indeed respond to such MIDI messages to change patches within a bank. It can respond to bank-change messages as well, but the process of setting up banks is a bit complicated, because it must be done using files and folders (one folder per bank), and more importantly, only 100 banks can be used. This is a serious limitation, because over 250 *Synth1* patch banks are available on the Internet.

*Synth1* is more than a little quirky. Its binary state-data blob does not actually contain the patch name; the plug-in recovers patch names from the *.sy1* file corresponding to the current MIDI program number, so *displayed patch names in the Synth1 GUI may be wrong* if the files in its bank-folders don't correspond exactly to the computer on which the state was captured. The patch will still sound correctly, though.

## First failed attempt

*Synth1*'s *.sy1* preset files consist mainly of number-pairs *(i, v)* that essentially mean "set the *i*th parameter to value *v*". I started with a pure reverse-engineering approach, where I figured out enough about the *Synth1* binary state-data format to determine what bytes corresponded to the 98 parameters. After trying this, I discovered that "essentially" doesn't mean "precisely, and for all parameters", so I gave up on this approach.

## Hybrid technique

The *Synth1 Unifier* program uses a hybrid approach, scanning the *.sy1* preset files in a designated folder to obtain patch names and MIDI program-numbers, sending MIDI program-change messages to an actual instance of the *Synth1* VST to force it to load the patch, then capturing its state-data to save into a new Unify patch (based on a supplied reference patch).

To make the program-change messages work, the "designated folder" for *.sy1* preset files must be the folder which *Synth1* is configured to look for the first bank ("bank 00"). When using *Synth1 Unifier*, you follow the same steps for each source bank:

1. Remove all files from the bank-00 folder
2. Copy the source *.sy1* files into the folder
3. Edit lines 8-11 in *Synth1_Unifier.cpp* to define the library name, author name, and comment text
4. Run the program

The program will create a new library folder in a target *Libraries* folder, which at this point is defined explicitly on line 13 of *Synth1_Unifier.cpp*. I set the path to correspond exactly to Unify's main *Libraries* folder, so after creating a few libraries I could simply refresh Unify's patch database and see them immediately.

## Limitations of this code

This approach creates one Unify library per bank of *Synth1* presets, which is a lot of tiny libraries, but other tools in this GitHub repo can be used to coalesce them if desired.

I have not attempted to apply any heuristics to guess the appropriate patch-name prefix for *Synth1* presets. Instead, I use the dummy prefix "SY1" for everything. Suitable renaming of the many thousands of presets is "left as an exercise for the reader".

