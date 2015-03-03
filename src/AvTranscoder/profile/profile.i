%{
#include <AvTranscoder/profile/ProfileLoader.hpp>
%}

namespace std {
%template(ProfileMap)      map< string, string >;
%template(ProfilesVector)  vector< map< string, string > >;
}

%include <AvTranscoder/profile/ProfileLoader.hpp>
