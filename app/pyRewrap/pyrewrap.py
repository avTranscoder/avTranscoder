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
        description='''Rewrap a video file and can move the index (moov atom) to the beginning of the file.''',
    )

    # requirements
    parser.add_argument('inputFileName', type=str, help='It could be any video file. Support file without extension.')
    # options
    parser.add_argument("-o", "--outputFile", dest="outputFileName", type=str, default="output.mov", help="Set the output filename (output.mov by default).")
    parser.add_argument("-c", "--format", dest="format", type=str, default="mov", help="Specify the output format.")
    parser.add_argument("-f", "--faststart", dest="faststart", action="store_true", default=False, help="Specify if the faststart option must be apply during rewrapping process (warning: 'mov' specific option).")
    # Parse command-line
    args = parser.parse_args()

except ImportError:
    import optparse

    # Create command-line interface
    parser = optparse.OptionParser(
        usage='usage: %prog -o <outputfile> -c <format> [-f] -i <inputfile>',
        prog='pyrewrap',
        description='''Rewrap a video file and can move the index (moov atom) to the beginning of the file.''',
    )

    # requirements
    parser.add_option("-i", "--inputFile", dest='inputFileName', type="string", help='It could be any video file. Support file without extension.')
    # options
    parser.add_option("-o", "--outputFile", dest="outputFileName", type="string", default="output.mov", help="Set the output filename (output.mov by default).")
    parser.add_option("-c", "--format", dest="format", type="string", default="mov", help="Specify the output format.")
    parser.add_option("-f", "--faststart", dest="faststart", action="store_true", default=False, help="Specify if the faststart option must be apply during rewrapping process (warning: 'mov' specific option).")
    # Parse command-line
    args, other = parser.parse_args()

    if args.inputFileName is None:
        parser.print_help()
        exit(1)

# setup avtranscoder
av.Logger().setLogLevel(av.AV_LOG_QUIET)
av.preloadCodecsAndFormats()

# create input file
inputFile = av.InputFile(args.inputFileName)
if len(inputFile.getProperties().getVideoProperties()) == 0:
    print("No video stream found in file ", args.inputFileName)
    exit(1)

# create output file (need to set format profile of encoding to force output format to mp4)
formatProfile = av.ProfileMap()
formatProfile[ av.avProfileIdentificator ] = "mp4WrapFormatPreset"
formatProfile[ av.avProfileIdentificatorHuman ] = "MP4 rewraping format preset"
formatProfile[ av.avProfileType ] = av.avProfileTypeFormat
formatProfile[ av.avProfileFormat ] = args.format
if args.faststart:
    formatProfile[ "movflags" ] = "+faststart"
outputFile = av.OutputFile( args.outputFileName )
outputFile.setupWrapping( formatProfile )

# create transcoder
transcoder = av.Transcoder( outputFile )
for streamIndex in range(0, inputFile.getProperties().getNbStreams()):
    transcoder.add(args.inputFileName, streamIndex) 

# launch process
progress = av.ConsoleProgress()
transcoder.process(progress)

