%{
#include <AvTranscoder/util.hpp>
%}

namespace std {
%template(StrVector) vector< string >;
}

%include <AvTranscoder/util.hpp>
