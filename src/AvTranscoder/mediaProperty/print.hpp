#ifndef _AV_TRANSCODER_PRINT_HPP_
#define _AV_TRANSCODER_PRINT_HPP_

#include <AvTranscoder/file/InputFile.hpp>

namespace avtranscoder
{

AvExport std::ostream& operator<<( std::ostream& flux, const FileProperties& fileProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const StreamProperties& streamProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const VideoProperties& videoProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const AudioProperties& audioProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const DataProperties& dataProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const SubtitleProperties& subtitleProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const AttachementProperties& attachementProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const UnknownProperties& unknownProperties );

AvExport std::ostream& operator<<( std::ostream& flux, const InputFile& input );

}

#endif
