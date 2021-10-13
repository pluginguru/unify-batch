# FXP Unifier

See the discussion for [TAL Unifier](../TAL&20Unifier)--all of the concepts discussed there apply here also.

Many older VST plug-ins use preset files with the extension *.fxp*, whose contents are exactly the same as the binary state-data blob that plug-in requires to re-load the patch. This simplest of cases practically begs for automatic unification using the same basic approach as I developed for [TAL Software](https://tal-software.com/) plug-ins.

## FXB vs FXP

When Unify saves a patch, it requests the state of all plug-in instances, and in the case of VST2 plug-ins, it does so by requesting the entire bank (returned as an "FXB chunk") rather than only the "current program" (returned as a "FXP chunk"). Hence, Unify was built with the assumption that the state-data blob for every VST2 plug-in is in the form of a FXB chunk. I had to make a small modification to Unify (at v.1.4.1 Beta 130, 21 April 2021) to allow it to work with the FXP chunks created by this unifier program.

Unify beta 130 (and probably later versions) inspect the state-data blob just before passing it to newly-instantiated VST2 *instrument* plug-ins (note this only done for instrument plug-ins at this time), to check the so-called "magic number" which is either 'FBCh' (bank) or 'FPCh' (preset), and if it is the latter, uploads it to the plug-in using *juce::AudioProcessor::setCurrentProgramStateInformation()* rather than *juce::AudioProcessor::setStateInformation()* which had been used for all VST2's previously.

This remains an experimental change at the time of writing.

