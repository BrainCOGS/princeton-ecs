% IMREADX    Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
%
%  If sub-pixel registration is requested, cv::warpAffine() is used.
%  Usage syntax:
%    [image, stats, median] = imreadx( inputPath, xShift, yShift, [xScale = 1], [yScale = 1] ...
%                                    , [blackTolerance = nan], [subtractZero = false]        ...
%                                    , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]  ...
%                                    , [methodResize = cve.InterpolationFlags.INTER_AREA]    ...
%                                    );
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
