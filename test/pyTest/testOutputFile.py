from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testCreateOutputFileWithExtension():
    """
    Create an OutputFile with a filename with extension.
    """
    formatName = "mov"
    outputFileName = "testCreateOutputFileWithExtension." + formatName
    ouputFile = av.OutputFile( outputFileName )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), formatName )


@raises(IOError)
def testCreateOutputFileWithoutExtension():
    """
    Create an OutputFile with a filename without extension.
    """
    outputFileName = "testCreateOutputFileWithoutExtension"
    ouputFile = av.OutputFile( outputFileName )

    assert_equals( ouputFile.getFilename(), outputFileName )


def testCreateOutputFileWithoutExtensionWithFormat():
    """
    Create an OutputFile with a filename without extension.
    Indicate the format.
    """
    formatName = "mov"
    outputFileName = "testCreateOutputFileWithoutExtensionWithFormat"
    ouputFile = av.OutputFile( outputFileName, formatName )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), formatName )


def testCreateOutputFileWithoutExtensionWithMimeType():
    """
    Create an OutputFile with a filename without extension.
    Indicate the Mime Type.
    """
    mimeType = "application/mp4"
    outputFileName = "testCreateOutputFileWithoutExtensionWithMimeType"
    ouputFile = av.OutputFile( outputFileName, "", mimeType )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatMimeType(), mimeType )


def testCreateOutputFileWithoutExtensionWithInconsistentFormatAndMimeType():
    """
    Create an OutputFile with a filename without extension.
    Indicate inconsistent format and Mime Type.
    The OutputFile should by-pass the Mime Type.
    """
    formatName = "mov"
    mimeType = "application/mp4"
    outputFileName = "testCreateOutputFileWithoutExtensionWithInconsistentFormatAndMimeType"
    ouputFile = av.OutputFile( outputFileName, formatName, mimeType )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), formatName )

