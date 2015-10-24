% IMFINFOX    Gets information about an image stack without reading in frames.
%
%  Usage syntax:
%    info = imfinfox(inputPath, [maxNumFrames = inf]);
%  where inputPath can be either a string (single file), or a cellstring for 
%  multi-file stacks.
%
%  Note that this function checks that all images in the stack have the same
%  width and height. An exception will be thrown if discrepancies are 
%  encountered.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
