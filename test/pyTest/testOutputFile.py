from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testCreateOutputFileWithExtension():
    """
    Create an OutputFile with a filename with extension.
    """
    ext = "mov"
    outputFileName = "testCreateOutputFileWithExtension." + ext
    ouputFile = av.OutputFile( outputFileName )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), ext )


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
    format = "mov"
    outputFileName = "testCreateOutputFileWithoutExtensionWithFormat"
    ouputFile = av.OutputFile( outputFileName, format )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), format )


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
    format = "mov"
    mimeType = "application/mp4"
    outputFileName = "testCreateOutputFileWithoutExtensionWithInconsistentFormatAndMimeType"
    ouputFile = av.OutputFile( outputFileName, format, mimeType )

    assert_equals( ouputFile.getFilename(), outputFileName )
    assert_equals( ouputFile.getFormatName(), format )

