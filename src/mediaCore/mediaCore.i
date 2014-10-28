%module(directors="1") mediaCore

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"

%include "swig/mediaException.i"
%include "swig/mediaExport.i"

%include "progress/progress.i"

%{
#include <mediaCore/frame/Pixel.hpp>
#include <mediaCore/frame/Frame.hpp>
#include <mediaCore/frame/VideoFrame.hpp>
#include <mediaCore/frame/AudioFrame.hpp>

#include <mediaCore/profile/Profile.hpp>
%}

namespace std {
%template(ProfileMap)      map< string, string >;
%template(ProfilesVector)  vector< map< string, string > >;
}


%include <mediaCore/profile/Profile.hpp>

%include <mediaCore/frame/Pixel.hpp>
%include <mediaCore/frame/Frame.hpp>
%include <mediaCore/frame/VideoFrame.hpp>
%include <mediaCore/frame/AudioFrame.hpp>
