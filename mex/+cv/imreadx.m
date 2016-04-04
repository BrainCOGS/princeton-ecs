% IMREADX    Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
%
%  Usage syntax:
%    [image, stats, median] = imreadx( inputPath, [xShift = []], [yShift = []]               ...
%                                    , [xScale = 1], [yScale = 1], [maxNumFrames = inf]      ...
%                                    , [blackTolerance = nan], [subtractZero = false]        ...
%                                    , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]  ...
%                                    , [methodResize = cve.InterpolationFlags.INTER_AREA]    ...
%                                    , [nanMask = []]                                        ...
%                                    );
%
%  maxNumFrames = nan can be used to return only the statistics structure, which saves on memory load in 
%  case the image stack is very large. Note that the median image cannot be computed in this case because
%  it requires the full stack to be in memory.
% 
%  If sub-pixel registration is requested, cv::warpAffine() is used.
%
%  Todo:     Binned median.
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
