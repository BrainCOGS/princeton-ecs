% IMREADTRANS    Loads the given image stack into memory, applying row/column shifts
  (rigid translation) to each frame. If sub-pixel registration is
  requested, cv::warpAffine() is used.
%
%  Usage syntax:
%    image = imreadtrans( inputPath, dx, dy, [methodInterp = -1] );
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
