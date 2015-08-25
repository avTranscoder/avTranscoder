#ifndef _AV_TRANSCODER_PRINT_HPP_
#define _AV_TRANSCODER_PRINT_HPP_

#include <AvTranscoder/file/InputFile.hpp>

#include <iostream>

namespace avtranscoder
{

std::ostream& operator<<( std::ostream& flux, const FileProperties& fileProperties );

std::ostream& operator<<( std::ostream& flux, const VideoProperties& videoProperties );

std::ostream& operator<<( std::ostream& flux, const AudioProperties& audioProperties );

std::ostream& operator<<( std::ostream& flux, const DataProperties& dataProperties );

std::ostream& operator<<( std::ostream& flux, const SubtitleProperties& subtitleProperties );

std::ostream& operator<<( std::ostream& flux, const AttachementProperties& attachementProperties );

std::ostream& operator<<( std::ostream& flux, const UnknownProperties& unknownProperties );

std::ostream& operator<<( std::ostream& flux, const InputFile& input );

}

#endif
