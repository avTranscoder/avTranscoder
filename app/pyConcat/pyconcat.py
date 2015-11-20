#!/usr/bin/env python

from pyAvTranscoder import avtranscoder as av

from sets import Set


# Get command line arguments
args = []
try:
    # python2.7+
    import argparse
    
    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='pyconcat',
        description='''Concatenate first stream of each given file to create an output file.''',
    )

    # requirements
    parser.add_argument('inputs', nargs='+', action='store', help='list of files to concatenate')
    # options
    parser.add_argument("-o", "--outputFile", dest="outputFileName", type=str, default="output.mov", help="Set the output filename (output.mov by default).")
    # Parse command-line
    args = parser.parse_args()

except ImportError:
    print("pyconcat currently expects python2.7+")
    exit(1)

# setup avtranscoder
logger = av.Logger().setLogLevel(av.AV_LOG_QUIET)
av.preloadCodecsAndFormats()

streamTypeToConcat = Set()
# get all input files
inputFiles = []
for input in args.inputs:
    inputFile = av.InputFile(input)
    streamTypeToConcat.add( inputFile.getStream(0).getProperties().getStreamType() )
    inputFiles.append(inputFile)

# Check type of streams to rewrap
if len(streamTypeToConcat) > 1:
    raise RuntimeError("Cannot concatenate streams of different type.")

# Create the output
outputFile = av.OutputFile( args.outputFileName );
if av.AVMEDIA_TYPE_VIDEO in streamTypeToConcat:
    outputFile.addVideoStream( inputFiles[-1].getStream(0).getVideoCodec() )
elif av.AVMEDIA_TYPE_AUDIO in streamTypeToConcat:
    outputFile.addVideoStream( inputFiles[-1].getStream(0).getAudioCodec() )

### process
outputFile.beginWrap()

data = av.Frame()
# for each input
for inputFile in inputFiles:
    packetRead = True
    # read all packets of first stream
    while packetRead:
        # read
        packetRead = inputFile.readNextPacket( data, 0 )
        # wrap
        outputFile.wrap( data, 0 )

outputFile.endWrap()
