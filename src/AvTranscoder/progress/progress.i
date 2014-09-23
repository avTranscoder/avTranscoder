%{
 #include <AvTranscoder/progress/IProgress.hpp>
 #include <AvTranscoder/progress/ConsoleProgress.hpp>
 #include <AvTranscoder/progress/NoDisplayProgress.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <AvTranscoder/progress/IProgress.hpp>
%include <AvTranscoder/progress/ConsoleProgress.hpp>
%include <AvTranscoder/progress/NoDisplayProgress.hpp>
