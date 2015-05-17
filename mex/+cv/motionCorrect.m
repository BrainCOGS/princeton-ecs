% MOTIONCORRECT    Computes a motion corrected version of the given movie using cv::matchTemplate().
%
%  Usage syntax:
%    [xShifts, yShifts]  = cvMotionCorrect( inputPath, outputPath, maxShift, maxIter                 ...
%                                         , [minShiftChange = 0], [restrictInterp = false]           ...
%                                         , [methodInterp = -1], [methodCorr = 5], [emptyValue = 0]  ...
%                                         );
%
%  The output is stored in the given outputPath, **overwriting** if the file already exists.
%  The history of x and y shifts can be returned to Matlab,
%
%  The memory usage of this function is numRows * numCols * (numZ + 1), i.e. one
%  frame more than the input image stack. The median image is used as the template
%  to which frames are aligned, except for a border of maxShift pixels in size which
%  is omitted since it is possible for motion correction to crop up to that much of
%  the frame.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
