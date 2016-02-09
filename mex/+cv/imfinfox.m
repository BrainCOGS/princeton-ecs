% IMFINFOX    Gets information about an image stack with minimal amount of frame reading.
%
%  The data structure returned by this function can be passed to subsequent
%  functions like cv.imreadx() to actually load the frames, obtain motion
%  correction shifts, and so forth.
%
%  Usage syntax:
%    info = imfinfox( inputPath );
%  where inputPath can be either a string (single file), or a cellstring for 
%  multi-file stacks.
%
%  Note that this function checks that all images in the stack have the same
%  width and height. An exception will be thrown if discrepancies are 
%  encountered.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
