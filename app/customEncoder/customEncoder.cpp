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
                char separator = 'x';
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
                    transcoder.addGenerateStream(transcodeProfile);
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



class AvExport CustomCodec
    : public avtranscoder::ICodec
{
public:
    CustomCodec()
        : avtranscoder::ICodec(avtranscoder::eCodecTypeEncoder, AV_CODEC_ID_PCM_S24LE)
    {
    }

    void openCodec(){}
    void closeCodec(){}

    std::string getCodecName() const { return "Custom Encoder"; };
    AVCodecID getCodecId() const { return AV_CODEC_ID_PCM_S24LE; }
    avtranscoder::ECodecType getCodecType() const { return avtranscoder::eCodecTypeEncoder; }
    int getLatency() const { return 0; }

    avtranscoder::OptionArray getOptions() {
        std::vector<avtranscoder::Option> options;
        return options;
    }
};


class AvExport CustomEncoder
    : public avtranscoder::IEncoder
{
public:
    CustomEncoder()
        : _codec(CustomCodec())
    {}
    /**
     * @brief Setup the encoder
     * @param profile: set encoder parameters from the given profile
     * @note Open the encoder.
     */
    void setupEncoder(const avtranscoder::ProfileLoader::Profile& profile = avtranscoder::ProfileLoader::Profile()) {
        return;
    };

    /**
     * @brief Encode a new frame, and get coded frame
     * @param sourceFrame: frame that needs to be encoded
     * @param codedFrame: output encoded coded data (first frames can be delayed)
     * @return status of encoding
     * @throw runtime_error if the encoded process failed.
     */
    bool encodeFrame(const avtranscoder::IFrame& sourceFrame, avtranscoder::CodedData& codedFrame) {
        codedFrame.assign(5760, 0);
        return true;
    };

    /**
     * @brief Get the frames remaining into the encoder
     * @param codedFrame: output encoded data
     * @return status of encoding
     * @throw runtime_error if the encoded process failed.
     */
    bool encodeFrame(avtranscoder::CodedData& codedFrame) {
        return false;
    };

    /**
     * @brief Get codec used for encoding.
     * @return a reference to the codec
     */
    avtranscoder::ICodec& getCodec() {
        return _codec;
    };

private:
    CustomCodec _codec;
};


int main(int argc, char** argv)
{
    std::string help;
    help += "Usage\n";
    help += "\tavprocessor INPUT_FILE_NAME OUTPUT_FILE_NAME [--verbose] [--logFile] [--help]\n";
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
        std::string output_format = "s24le";
        avtranscoder::OutputFile outputFile(argv[2], output_format);
    
        avtranscoder::Transcoder transcoder(outputFile);
        transcoder.setProcessMethod(avtranscoder::eProcessMethodBasedOnStream);

        CustomEncoder* customEncoder = new CustomEncoder;
        avtranscoder::InputStreamDesc inputDescLeft(argv[1], 1, 0);
        avtranscoder::InputStreamDesc inputDescRight(argv[1], 2, 0);

        std::vector<avtranscoder::InputStreamDesc> inputDescriptors;
        inputDescriptors.push_back(avtranscoder::InputStreamDesc(argv[1], 1, 0));
        inputDescriptors.push_back(avtranscoder::InputStreamDesc(argv[1], 2, 0));

        transcoder.addStream(inputDescriptors, customEncoder);

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
