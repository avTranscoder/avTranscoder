# How to use avTranscoder

Check out applications contained in the project to see examples of how to use the library in C++, Java or Python.

#### In C++
Set environment:
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/ffmpeg/lib:/path/to/avtranscoder/lib
export PATH=$PATH:/path/to/avtranscoder/bin
```

#### In Java
Add argument to the JVM:
```
-Djava.library.path=/path/to/avtranscoder/lib/java
```
Set environment:
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/ffmpeg/lib
```

#### In Python
Set environment:
```
export PYTHONPATH=$PYTHONPATH:/path/to/avtranscoder/lib/python<version>/site-packages/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/ffmpeg/lib
```

#### Use profiles
To wrap/unwrap/encode/decode, avTranscoder manipulates profiles.
A profile is a set of key-value given as parameters to the InputFile(unwrap), the OutputFile(wrap), the Video/AudioDecoder(decode) or the Video/AudioEncoder(encode).
There are two ways to manipulate profiles:
* create profiles inside your code, by instanciate ```Map``` structures.
* create profiles outside your code, by create text files.
To indicate the path to the text files, export environment variable:
```
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
