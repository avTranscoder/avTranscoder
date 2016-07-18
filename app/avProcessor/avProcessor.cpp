#include <AvTranscoder/transcoder/Transcoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
#include <AvTranscoder/progress/ConsoleProgress.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>

void parseConfigFile(const std::string& configFilename, avtranscoder::Transcoder& transcoder)
{
    std::ifstream configFile(configFilename.c_str(), std::ifstream::in);

    std::string line;
    while(std::getline(configFile, line))
    {
        std::istringstream is_line(line);
        std::string filename;
        if(std::getline(is_line, filename, '='))
        {
            std::string streamId;
            if(std::getline(is_line, streamId, ':'))
            {
                std::string transcodeProfile;
                std::getline(is_line, transcodeProfile);

                std::stringstream ss(streamId);
                size_t streamIndex = 0;
                char separator;
                std::vector<size_t> channelIndexArray;
                ss >> streamIndex;
                ss >> separator;
                if(separator == '.')
                {
                    int subStreamIndex = -1;
                    ss >> subStreamIndex;
                    channelIndexArray.push_back(subStreamIndex);
                }

                // generated stream
                if(!filename.length())
                    transcoder.addGeneratedStream(transcodeProfile);
                else
                {
                    avtranscoder::InputStreamDesc inputDesc(filename, streamIndex, channelIndexArray);
                    transcoder.addStream(inputDesc, transcodeProfile);
                }
            }
        }
    }

    configFile.close();
}

int main(int argc, char** argv)
{
    std::string help;
    help += "Usage\n";
    help += "\tavprocessor CONFIG.TXT OUTPUT_FILE_NAME [--generate-black] [--verbose] [--logFile] [--help]\n";
    help += "CONFIG.TXT\n";
    help += "\tEach line will be one stream in the output.\n";
    help += "\tPattern of each line is:\n";
    help += "\t[inputFile]=STREAM_ID.[subStreamId]:[profileName]\n";
    help += "\tNo inputFile: will generate black image / audio silence (audio by default)\n";
    help += "\tNo subStreamId: will process of channels of the stream\n";
    help += "\tNo profileName: will rewrap the stream\n";
    help += "Command line options\n";
    help += "\t--verbose: set log level to AV_LOG_DEBUG\n";
    help += "\t--logFile: put log in 'avtranscoder.log' file\n";
    help += "\t--help: display this help\n";

    // Preload FFmpeg context
    avtranscoder::preloadCodecsAndFormats();
    avtranscoder::Logger::setLogLevel(AV_LOG_QUIET);

    // List command line arguments
    std::vector<std::string> arguments;
    for(int argument = 1; argument < argc; ++argument)
    {
        arguments.push_back(argv[argument]);
    }
    for(size_t argument = 0; argument < arguments.size(); ++argument)
    {
        if(arguments.at(argument) == "--help")
        {
            std::cout << help << std::endl;
            return 0;
        }
        else if(arguments.at(argument) == "--verbose")
        {
            avtranscoder::Logger::setLogLevel(AV_LOG_DEBUG);
        }
        else if(arguments.at(argument) == "--logFile")
        {
            avtranscoder::Logger::logInFile();
        }
    }

    // Check required arguments
    if(argc < 3)
    {
        std::cout << "avprocessor can rewrap or transcode inputs to create an output media file." << std::endl;
        std::cout << "Use option --help to display help" << std::endl;
        return (-1);
    }

    try
    {
        std::string inputConfigFile(argv[1]);
        avtranscoder::OutputFile outputFile(argv[2]);

        avtranscoder::Transcoder transcoder(outputFile);
        transcoder.setProcessMethod(avtranscoder::eProcessMethodBasedOnStream, 0);

        parseConfigFile(inputConfigFile, transcoder);

        avtranscoder::ConsoleProgress progress;
        transcoder.process(progress);
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR: during process, an error occured: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "ERROR: during process, an unknown error occured" << std::endl;
    }
}
