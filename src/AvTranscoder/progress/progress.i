%{
 #include <AvTranscoder/progress/progress.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <AvTranscoder/progress/progress.hpp>
