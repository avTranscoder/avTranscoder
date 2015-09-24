#!/usr/bin/env python

from pyAvTranscoder import avtranscoder as av


# Get command line arguments
args = []
try:
    # python2.7+
    import argparse
    
    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='pyrewrap',
        description='''Rewrap a video file in the same codec and can mov header at the begining of file''',
    )

    # requirements
    parser.add_argument('inputFileName', help='It could be any video file. Support file without extension.')
    # options
    parser.add_argument("-o", "--outputFile", dest="outputFileName", help="Set the output filename (thumbnail.jpg by default). Must be with jpg extension!")
    parser.add_argument("-c", "--codec", dest="codec", type=str, help="Specify the video input/output codec.")
    parser.add_argument("-f", "--faststart", dest="faststart", action="store_true", help="Specify if the faststart option must be apply during rewrapping process.")
    # Parse command-line
    args = parser.parse_args()
    
    if args.inputFileName is None:
        parser.print_help()
        exit(1)

except ImportError:
    import optparse

    # Create command-line interface
    parser = optparse.OptionParser(
        usage='usage: %prog -o <outputfile> -c <codec> [-f] -i <inputfile>',
        prog='pythumbnail',
        description='''Generate jpeg thumbnail from video/image.''',
    )

    # requirements
    parser.add_option("-i", "--inputFile", dest='inputFileName', help='It could be any video file. Support file without extension.')
    # options
    parser.add_option("-o", "--outputFile", dest="outputFileName", help="Set the output filename (thumbnail.jpg by default). Must be with jpg extension!")
    parser.add_option("-c", "--codec", dest="codec", type=str, help="Specify the video input/output codec.")
    parser.add_option("-f", "--faststart", dest="faststart", help="Specify if the faststart option must be apply during rewrapping process.")
    # Parse command-line
    args, other = parser.parse_args()

    if args.inputFileName is None or args.outputFileName is None or args.codec is None:
        parser.print_help()
        exit(1)

# setup avtranscoder
logger = av.Logger().setLogLevel(av.AV_LOG_QUIET)
av.preloadCodecsAndFormats()

# create input file
inputFile = av.InputFile(args.inputFileName)
if len(inputFile.getProperties().getVideoProperties()) == 0:
    print "No video stream found in file ", args.inputFileName
    exit(1)


# create output file (need to set format profile of encoding to force output format to mp4)
formatProfile = av.ProfileMap()
formatProfile[ av.avProfileIdentificator ] = "mp4WrapFormatPreset"
formatProfile[ av.avProfileIdentificatorHuman ] = "MP4 rewraping format preset"
formatProfile[ av.avProfileType ] = av.avProfileTypeFormat
formatProfile[ av.avProfileFormat ] = args.codec
if args.faststart is not None:
    # formatProfile[ "movflags" ] = "faststart"
    pass
outputFile = av.OutputFile( args.outputFileName )
outputFile.setProfile( formatProfile )

# create transcoder
transcoder = av.Transcoder( outputFile )

def addStreamsToTranscoder(transcoder, streams):
    for st in streams:
        stIndex = st.getStreamIndex()
        transcoder.add( args.inputFileName, stIndex )

addStreamsToTranscoder(transcoder, inputFile.getProperties().getVideoProperties())
# addStreamsToTranscoder(transcoder, inputFile.getProperties().getAudioProperties())
# addStreamsToTranscoder(transcoder, inputFile.getProperties().getDataProperties())

# launch process
transcoder.process()