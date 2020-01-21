#include <AvTranscoder/transcoder/Transcoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
#include <AvTranscoder/progress/ConsoleProgress.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>

std::vector<avtranscoder::InputStreamDesc> parseConfigFile(const std::string& configFilename)
{
    std::vector<avtranscoder::InputStreamDesc> result;

    std::ifstream configFile(configFilename.c_str(), std::ifstream::in);

    std::string line;
    size_t countLines = 0;
    while(std::getline(configFile, line))
    {
        if(++countLines > 2)
            throw std::runtime_error("Audio phase analysis can only be done on a stereo pair, i.e. two channels (see usage).");

        std::istringstream is_line(line);
        std::string filename;
        if(std::getline(is_line, filename, '='))
        {
            std::string stream;
            std::getline(is_line, stream);

            std::stringstream ss(stream);
            size_t streamIndex = 0;
            char separator;
            int channelIndex = -1;
            ss >> streamIndex;
            ss >> separator;
            if(separator == '.')
                ss >> channelIndex;

            bool newInputDescAdded = false;
            // if we already have an input description with the same filename/streamIndex, add only the new channelIndex
            for(std::vector<avtranscoder::InputStreamDesc>::iterator it = result.begin(); it != result.end(); ++it)
            {
                if(it->_filename == filename && it->_streamIndex == streamIndex)
                {
                    it->_channelIndexArray.push_back(channelIndex);
                    newInputDescAdded = true;
                    break;
                }
            }
            if(! newInputDescAdded)
                result.push_back(avtranscoder::InputStreamDesc(filename, streamIndex, channelIndex));
        }
    }

    configFile.close();

    return result;
}

void displayUsage(const std::string& program)
{
    std::cout << "Usage: " << program << " CONFIG OUTPUT [OPTIONS]" << std::endl << std::endl;
    std::cout << "\tCONFIG: input configuration file" << std::endl;
    std::cout << "\t\tEach line represents one audio stream analysed." << std::endl;
    std::cout << "\t\tPattern of each line is:" << std::endl;
    std::cout << "\t\t[inputFile]=STREAM_INDEX.CHANNEL_INDEX" << std::endl;
    std::cout << "\t\tWARNING: audio phase analyser only support stereo layout, i.e. two lines in this configuration." << std::endl << std::endl;
    std::cout << "\tOUTPUT: metadata output file" << std::endl;
    std::cout << "\t\tPattern for each frame is:" << std::endl;
    std::cout << "\t\t `frame:[FRAME_ID]    pts:[PTS]       pts_time:[PTS_TIME]" << std::endl;
    std::cout << "\t\t  lavfi.aphasemeter.phase=[PHASE_VALUE]`" << std::endl << std::endl;
    std::cout << "\tOPTIONS:" << std::endl;
    std::cout << "\t\t--info     set log level to AV_LOG_INFO" << std::endl;
    std::cout << "\t\t--debug    set log level to AV_LOG_DEBUG" << std::endl;
    std::cout << "\t\t--help     display this help" << std::endl << std::endl;
}

int main(int argc, char** argv)
{
    // Preload FFmpeg context
    avtranscoder::preloadCodecsAndFormats();
    avtranscoder::Logger::setLogLevel(AV_LOG_QUIET);

    if(argc < 3) {
        displayUsage(argv[0]);
    }

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
            displayUsage(argv[0]);
            return 0;
        }
        else if(arguments.at(argument) == "--debug")
        {
            avtranscoder::Logger::setLogLevel(AV_LOG_DEBUG);
        }
        else if(arguments.at(argument) == "--info")
        {
            avtranscoder::Logger::setLogLevel(AV_LOG_INFO);
        }
    }

    try
    {
        std::string configFilePath(arguments.at(0));
        std::string outputFilePath(arguments.at(1));
        std::vector<avtranscoder::InputStreamDesc> inputStreamsToAnalyse = parseConfigFile(configFilePath);

        avtranscoder::OutputFile outputFile(outputFilePath, "null"); // the output file will be overwritten by the extracted metadata

        avtranscoder::Transcoder transcoder(outputFile);
        transcoder.setProcessMethod(avtranscoder::eProcessMethodBasedOnStream, 0);
        transcoder.addStream(inputStreamsToAnalyse);

        avtranscoder::StreamTranscoder& streamTranscoder = transcoder.getStreamTranscoder(0);
        avtranscoder::FilterGraph* filterGraph = streamTranscoder.getFilterGraph();
        filterGraph->addFilter("aphasemeter", "video=0");
        filterGraph->addFilter("ametadata", "mode=print:file=" + outputFilePath);

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
