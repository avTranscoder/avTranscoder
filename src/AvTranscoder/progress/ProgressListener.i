%module(directors="1") AvTranscoder


%{
 #include <AvTranscoder/progress/ProgressListener.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <AvTranscoder/progress/ProgressListener.hpp>
