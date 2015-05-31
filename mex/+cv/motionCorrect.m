% MOTIONCORRECT    Computes a motion corrected version of the given movie using cv::matchTemplate().
%
%  Usage syntax:
%    mc  = cv.motionCorrect( inputPath, maxShift, maxIter                            ...
%                          , [displayProgress = false], [stopBelowShift = 0]         ...
%                          , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]    ...
%                          , [methodCorr = cve.TemplateMatchModes.TM_CCOEFF_NORMED]  ...
%                          , [emptyValue = mean]                                     ...
%                          );
%
%  The memory usage of this function is numRows * numCols * (numZ + 1), i.e. one
%  frame more than the input image stack. The median image is used as the template
%  to which frames are aligned, except for a border of maxShift pixels in size which
%  is omitted since it is possible for motion correction to crop up to that much of
%  the frame.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
