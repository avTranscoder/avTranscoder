%{
 #include <AvTranscoder/progress.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <AvTranscoder/progress.hpp>
