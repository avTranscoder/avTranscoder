from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testCreateOutputFileWithExtension():
    """
    Create an OutputFile with a filename with extension.
    """
    formatName = "mov"
    formatLongName = "QuickTime / MOV"
    outputFileName = "testCreateOutputFileWithExtension." + formatName
    ouputFile = av.OutputFile( outputFileName )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), formatName )
    assert_equals( ouputFile.getFormatLongName(), formatLongName )


@raises(IOError)
def testCreateOutputFileWithoutExtension():
    """
    Create an OutputFile with a filename without extension.
    """
    outputFileName = "testCreateOutputFileWithoutExtension"
    av.OutputFile( outputFileName )


def testCreateOutputFileWithoutExtensionWithFormat():
    """
    Create an OutputFile with a filename without extension.
    Indicate the format.
    """
    formatName = "mov"
    formatLongName = "QuickTime / MOV"
    outputFileName = "testCreateOutputFileWithoutExtensionWithFormat"
    ouputFile = av.OutputFile( outputFileName, formatName )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), formatName )
    assert_equals( ouputFile.getFormatLongName(), formatLongName )


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
    formatLongName = "QuickTime / MOV"
    mimeType = "application/mp4"
    outputFileName = "testCreateOutputFileWithoutExtensionWithInconsistentFormatAndMimeType"
    ouputFile = av.OutputFile( outputFileName, formatName, mimeType )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), formatName )
    assert_equals( ouputFile.getFormatLongName(), formatLongName )
    assert_not_equals( ouputFile.getFormatMimeType(), mimeType )
