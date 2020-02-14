from nose.tools import *

from pyAvTranscoder import avtranscoder as av

def testCodedDataConstructors():
    """
    Try to create a CodedData instances from different constructors.
    """
    dataSize = 1024
    codedData = av.CodedData(dataSize)
    assert_equals(dataSize, codedData.getSize())

    codedDataCopy = av.CodedData(codedData)
    assert_equals(dataSize, codedDataCopy.getSize())


def testCodedDataManagement():
    """
    Try to resize and assign CodedData data.
    """
    dataSize = 1024
    codedData = av.CodedData()
    codedData.resize(dataSize)
    assert_equals(dataSize, codedData.getSize())

    newDataSize = 128
    codedData.assign(newDataSize, 1)
    assert_equals(newDataSize, codedData.getSize())
    data = codedData.getData()
    for i in range(0, newDataSize):
        assert_equals('\x01', data[i])

    newDataSize = 256
    codedData.resize(newDataSize)
    assert_equals(newDataSize, codedData.getSize())
