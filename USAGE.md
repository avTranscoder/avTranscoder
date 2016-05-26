# How to use avTranscoder

## When to use avTranscoder
* when you need to access the API of ffmpeg, but you want to avoid the head-hurting part of AVPacket, AV_TIME_BASE and all the deep ffmpeg components.
* when you would like to access the ffmpeg features in your language (currently Java and Python, tomorrow what you want!).


## API
The original code is in C++, but almost all the methods are translated into Java/Python without any changes.  
So you should start having a look at the [C++ code](src/AvTranscoder) (since the generated Java/Python code is ugly).  

In the sake of brevity, the example code of the following section is written in python.

#### At the beginning
Before starting anything, you need to register all the codecs, formats, filters which are enabled at configuration time.  
So your first avTranscoder instruction should be:
```python
avtranscoder.preloadCodecsAndFormats()
```

#### Analyse media
To analyse a media, you can create an [__InputFile__](src/AvTranscoder/file/InputFile.hpp) and have access to all header [__properties__](src/AvTranscoder/properties).
```python
# create InputFile
inputFile = avtranscoder.InputFile("/path/to/media/file.mov")

# get access to properties
properties = inputFile.getProperties()

# print all format properties
for prop, value in properties.asVector():
   print(prop + ": " + str(value))

# print all properties of each stream
for stream in properties.getStreamProperties():
    print("------ Properties of stream at index " + str(stream.getStreamIndex()) + " -----")
    for prop, value in stream.asVector():
        print(prop + ": " + str(value))

# write all media properties as JSON in a file
import json
with open('data.txt', 'w') as outfile:
    json.dump(properties.allPropertiesAsJson(), outfile)
```

Some properties could have a value of _null_ because:
* the property is not indicated in the media.
* the property is not accessible in the header.

For the second point, you can launch a deeper analysis.  
For instance, you might like to access to the GOP structure:
```python
# create InputFile
inputFile = avtranscoder.InputFile("/path/to/media/file.mov")

# analyse first GOP (show progression in console)
progress = avtranscoder.NoDisplayProgress()
inputFile.analyse(progress, avtranscoder.eAnalyseLevelFirstGop)

# get access to properties
properties = inputFile.getProperties()
videoProperties = properties.getVideoProperties()[0]
print videoProperties.getGopSize()
print videoProperties.getGopStructure()
```

Check out [__avmeta__](app/avMeta/avMeta.cpp) application to see a C++ example of this use case.

#### Process media
An important part of the API concerns media processing. The general overview of the workflow could be separated into two main cases:  
* _transcoding_: demuxing, decoding, transform, encoding and muxing. Here we can update all parts of the media, from the container to the codec used to encode data.  
See the [transcoding process](https://ffmpeg.org/ffmpeg.html#toc-Detailed-description) in ffmpeg documentation.
* _rewrap_: omit the decoding, transform and encoding steps. It is useful for changing the container format or modifying container-level metadata.  
See the [copy](https://ffmpeg.org/ffmpeg.html#Stream-copy) paramer in ffmpeg documentation.

The main class to process media is the [__Transcoder__](src/AvTranscoder/transcoder/Transcoder.hpp).  

Here a rewrapping example to update container (from AVI to MOV):
```python
ouputFile = avtranscoder.OutputFile("/path/to/output/media.mov")
transcoder = avtranscoder.Transcoder(ouputFile)
transcoder.add("/path/to/input/media.avi")
transcoder.process()
```
Check out [__pyrewrap__](app/pyRewrap/pyrewrap.py) application to see a python example of this use case.

Here a transcoding example to encode video and audio streams:
```python
ouputFile = avtranscoder.OutputFile("/path/to/output/media.mov")
transcoder = avtranscoder.Transcoder(ouputFile)

# transcode a video stream at index 0 to H264 High Quality
transcoder.add("/path/to/input/media.avi", 0, "h264-hq")
# transcode an audio stream at index 1 to PCM 24bits 48kHz mono
transcoder.add("/path/to/input/media.wav", 1, "wave24b48kmono")

# show the progression in console
progress = avtranscoder.ConsoleProgress()
transcoder.process(progress)
```
Check out [__avprocessor__](app/avProcessor/avProcessor.cpp) application to see a C++ example of this use case.

An other important parameter is the transcoding policy, which defines how we manage the process in case of several streams. By default the process ends at the end of the longest stream (so the shorter streams are filled with silence or black images).
To change the transcoding policy:
```python
# stop the process when the output stream at index 0 is finished
transcoder.setProcessMethod(avtranscoder.eProcessMethodBasedOnStream, 0)

# stop the process when the output file has a duration of 50s
transcoder.setProcessMethod(avtranscoder.eProcessMethodBasedOnDuration, 0, 50)
```

#### More on process: use profiles
To wrap/unwrap/encode/decode, avTranscoder manipulates profiles.
A profile is a set of key-value given as parameters to the InputFile(unwrap), the OutputFile(wrap), the Video/AudioDecoder(decode) or the Video/AudioEncoder(encode).
There are two ways to manipulate profiles:
* create profiles inside your code, by instanciate ```Map``` structures.
* create profiles outside your code, by create text files. You have examples [here](avprofiles).

To indicate the path to the text files, export environment variable:
```bash
export AVPROFILES=/path/to/profiles
```

The minimum format profile (wrap/unwrap) is:
```
avProfileName=profileName
avProfileLongName=profileLongName
avProfileType=avProfileTypeFormat
format=formatName 
```
The minimum video profile (encode/decode) is:
```
avProfileName=profileName
avProfileLongName=profileLongName
avProfileType=avProfileTypeVideo
codec=codecName
```
The minimum audio profile (encode/decode) is:
```
avProfileName=profileName
avProfileLongName=profileLongName
avProfileType=avProfileTypeAudio
codec=codecName
```

To encode a video stream using your custom profile:
```python
ouputFile = avtranscoder.OutputFile("/path/to/output/media.mov")
transcoder = avtranscoder.Transcoder(ouputFile)
transcoder.add("/path/to/input/media.avi", 0, "profileName")
transcoder.process()
```

To go into more details about the avTranscoder API, take a look at the [__Doxygen documentation__](http://avtranscoder.github.io/avTranscoder-doxygen/).

## Environment
#### In C++
Set environment:
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/ffmpeg/lib:/path/to/avtranscoder/lib
export PATH=$PATH:/path/to/avtranscoder/bin
```

#### In Java
Add argument to the JVM:
```bash
-Djava.library.path=/path/to/avtranscoder/lib/java
```
Set environment:
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/ffmpeg/lib
```

#### In Python
Set environment:
```bash
export PYTHONPATH=$PYTHONPATH:/path/to/avtranscoder/lib/python<version>/site-packages/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/ffmpeg/lib
```

## Projects which uses avTranscoder
#### TuttleOFX
TuttleOFX is a library to connect and batch operations with OpenFx plugins. It comes with a set of plugins that allows you to batch process on movies and file sequences.  
TuttOFX uses avTranscoder for the [AudioVideo plugin](https://github.com/tuttleofx/TuttleOFX/tree/develop/plugins/image/io/AudioVideo), one of its io plugins.

#### PADdef / Slingshot
PADdef is a platform developed by [Mikros Image](http://www.mikrosimage.eu/) for France Télévisions's new digital distribution centers. Its purpose is to handle the delivery of ready-to-broadcast feature material.  
Slingshot is its mirror project for Belgium broadcasters: Medialaan, RTBF and RTL.
PADdef / Slingshot uses avTranscoder in one of its application to transcode media in post-production labs.

#### Ploud
A set of applications developed by [Mikros Image](http://www.mikrosimage.eu/) to analyse, visualize and correct the loudness.  
Ploud uses avTranscoder in [one of its application](https://github.com/mikrosimage/loudness_validator/tree/develop/app/mediaAnalyser) to give decoded data to the loudness analyser.
