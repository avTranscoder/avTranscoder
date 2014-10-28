%module(directors="1") mediaTransform

%include "std_vector.i"

%include "mediaCore/swig/mediaExport.i"

%{
#include <mediaTransform/ITransform.hpp>
#include <mediaTransform/AudioTransform.hpp>
#include <mediaTransform/VideoTransform.hpp>
%}

%include <mediaCore/common.hpp>

%include <mediaTransform/ITransform.hpp>
%include <mediaTransform/AudioTransform.hpp>
%include <mediaTransform/VideoTransform.hpp>
