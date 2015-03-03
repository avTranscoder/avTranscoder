%{
#include <AvTranscoder/profile/ProfileLoader.hpp>
%}

namespace std {
%template(IntPair)         pair< size_t, size_t >;
%template(ProfileMap)      map< string, string >;
%template(ProfilesVector)  vector< map< string, string > >;
}

%include <AvTranscoder/profile/ProfileLoader.hpp>
