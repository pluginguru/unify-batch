# Notes on automatic "unifier" programs

This repo contains a few experimental programs for creating "unified" libraries, i.e., collections of Unify patches, each of which loads just one instance of some third-party plug-in, pre-loaded with one of that plug-in's factory presets.

While I encourage capable programmers to study these programs and try to extend them, or create their own, I must emphasize three points:

1. **Everything here is unsupported**. No one at PlugInGuru has time or resources to support these programs, or teach you anything more than you can already find here.
2. **These programs are all highly experimental, and largely undocumented**. Don't expect any of them to be turnkey solutions or ideal starting-points.
3. **There is no standard way to do this**. As will be explained further below, it's very much a trial-and-error, reverse-engineering process, and in general every third-party plug-in will require different methods.

## Why automatic generation of unified patches is hard

The world of plug-ins is deeply balkanized. The “standard” formats like VST2, VST3, Audio Units and AAX are all proprietary and subject to licensing restrictions. There’s an emerging open-source format called [CLAP](https://cleveraudio.org/) which is promising, but likely to take years to become widely supported. The need to support multiple formats as well as multiple platforms and CPU types has led to the development of big C++ frameworks like [JUCE](https://juce.com/) that have murderous learning-curves, but provide true “write once, run anywhere” ability. Unify is built with JUCE, and would be impractical without it.

There are essentially no standard formats for plug-in presets. Well, there are some proprietary standards, but very few commercial plug-ins actually use them. Everyone rolls their own. Think "wild-west".

Each plug-in has a way of reporting its current state to the host (DAW, Unify, etc.) as a binary blob, which, if stuffed back into a later instantiation of the same plug-in, will return it to that state. Unify patches are basically XML, which include these state-blobs, serialized into gibberish strings with a base64 algorithm. No one publishes their state-blob formats, and it’s another wild west there.

The first “unified” libraries were built entirely manually, by loading up a plug-in in Unify, then laboriously loading each of its presets (within the plug-in’s GUI) one at a time, then saving the resulting patch in Unify. A few people have been able to at least partially automate this process using tools like [AutoHotKey](https://www.autohotkey.com/) on Windows, but it's not a lot of fun. This remains the most widely used approach.

In this GitHub repository, you’ll find a number of JUCE/C++ projects I’ve done for “unifying” specific plug-ins, i.e., programmatically converting that plug-in’s own preset files into Unify patches that include an instance of that plug-in, loaded with a state-data blob which effectively loads the original preset. There are NO standard ways to do this, but through careful reverse engineering I’ve figured out some techniques that work for a few specific vendor’s plug-ins.

