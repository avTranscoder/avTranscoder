%module avTranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"

%{
#include <avTranscoder/OutputFile.hpp>

#include <avTranscoder/OutputStream.hpp>
#include <avTranscoder/OutputStreamAudio.hpp>
#include <avTranscoder/OutputStreamVideo.hpp>

#include <avTranscoder/InputStream.hpp>
#include <avTranscoder/InputStreamAudio.hpp>
#include <avTranscoder/InputStreamVideo.hpp>
%}

%include <avTranscoder/OutputFile.hpp>

%include <avTranscoder/OutputStream.hpp>
%include <avTranscoder/OutputStreamAudio.hpp>
%include <avTranscoder/OutputStreamVideo.hpp>

%include <avTranscoder/InputStream.hpp>
%include <avTranscoder/InputStreamAudio.hpp>
%include <avTranscoder/InputStreamVideo.hpp>