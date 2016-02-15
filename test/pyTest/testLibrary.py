from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testListLibraries():
    """
    Check name and license of libraries.
    """
    librariesName = set(["avtranscoder", "avutil", "avformat", "avcodec", "avresample", "swresample", "swscale", "avfilter"])

    for library in av.getLibraries():
        libraryName = library.getName()
        if libraryName == "avtranscoder":
            assert_equals(library.getLicense(), "GPL v2 or LGPL v2.1")
        assert_in(libraryName, librariesName)
        librariesName.remove(libraryName)

    # avresample or swresample, if avtranscoder depends on libav or ffmpeg
    assert_equals(len(librariesName), 1)


def testSupportedExtensions():
    """
    Check if we can access the supported extensions.
    These numbers of input/output extensions correspond to the features implemented in common libraries (not in third parties).
    @note Tested with ffmpeg-2.4.2
    """
    inputExtensions = av.getInputExtensions()
    outputExtensions = av.getOutputExtensions()
    
    assert_greater_equal(len(inputExtensions), 162)
    assert_greater_equal(len(outputExtensions), 184)
