% MOTIONCORRECT    Computes a motion corrected version of the given movie using cv::matchTemplate().
%
%  Usage syntax:
%    mc  = cv.motionCorrect( inputPath, maxShift, maxIter                            ...
%                          , [displayProgress = false], [stopBelowShift = 0]         ...
%                          , [blackTolerance = nan], [medianRebin = 1]               ...
%                          , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]    ...
%                          , [methodCorr = cve.TemplateMatchModes.TM_CCOEFF_NORMED]  ...
%                          , [emptyValue = mean]                                     ...
%                          );
%    mc  = cv.motionCorrect( {input, template}, ... );
%
%  The median image is used as the template to which frames are aligned, except for 
%  a border of maxShift pixels in size which is omitted since it is possible for 
%  motion correction to crop up to that much of the frame.
%
%  The medianRebin parameter can be used to specify that the median should be computed 
%  using this number of frames per data point, instead of all frames. This can help
%  reduce the amount of time required to motion correct.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
