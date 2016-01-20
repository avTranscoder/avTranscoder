#include <AvTranscoder/common.hpp>
#include <AvTranscoder/reader/VideoReader.hpp>

#include "Window.hpp"

#include <cstdlib>

int main(int argc, char** argv)
{
    std::string filename;
    size_t streamIndex = 0;
    size_t width = 0;
    size_t height = 0;

    std::string help;
    help += "Usage\n";
    help += "\tavplay filename [streamIndex] [--width width] [--height height] [--help]\n";
    help += "Command line options\n";
    help += "\tstreamIndex: specify the index of the stream to read (by default 0)\n";
    help += "\t--width: specify the output width (by default the same as input)\n";
    help += "\t--height: specify the output height (by default the same as input)\n";
    help += "\t--help: display this help\n";

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
        else if(arguments.at(argument) == "--width")
        {
            try
            {
                width = atoi(arguments.at(++argument).c_str());
            }
            catch(...)
            {
                std::cout << help << std::endl;
                return 0;
            }
        }
        else if(arguments.at(argument) == "--height")
        {
            try
            {
                height = atoi(arguments.at(++argument).c_str());
            }
            catch(...)
            {
                std::cout << help << std::endl;
                return 0;
            }
        }
        // positional arguments
        if(argument == 0)
        {
            filename = arguments.at(argument);
        }
        else if(argument == 1)
        {
            streamIndex = atoi(arguments.at(argument).c_str());
        }
    }

    // Check required arguments
    if(argc < 2)
    {
        std::cout << "avplay can play the given video media file." << std::endl;
        std::cout << "Use option --help to display help" << std::endl;
        return (-1);
    }

    // Setup avtranscoder
    avtranscoder::preloadCodecsAndFormats();
    avtranscoder::Logger::setLogLevel(AV_LOG_QUIET);

    avtranscoder::VideoReader reader(filename, streamIndex);
    if(width == 0)
        width = reader.getOutputWidth();
    if(height == 0)
        height = reader.getOutputHeight();
    reader.updateOutput(width, height, "rgb24");
    Window window(reader);
    window.launch();
}
