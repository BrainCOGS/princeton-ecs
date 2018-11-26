% /USR/PEOPLE/KOAY/CODE/PRINCETON-ECS/MEX/+CV/MOTIONCORRECT.MEXA64    Computes a motion corrected version of the given movie using cv::matchTemplate().
%
%  Usage syntax:
%    mc  = cv.motionCorrect( inputPath, maxShift, maxIter                                  ...
%                          , [displayProgress = false], [stopBelowShift = 0]               ...
%                          , [blackTolerance = nan], [medianRebin = 1]                     ...
%                          , [frameSkip = [0 0]], [centerShifts = ~isnan(blackTolerance)]  ...
%                          , [preferSmallestShifts = false]                                ...
%                          , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]          ...
%                          , [methodCorr = cve.TemplateMatchModes.TM_CCOEFF_NORMED]        ...
%                          , [emptyValue = mean]                                           ...
%                          );
%    mc  = cv.motionCorrect( {input, template}, ... );
%
%  The median image is used as the template to which frames are aligned, except for 
%  a border of maxShift pixels in size which is omitted since it is possible for 
%  motion correction to crop up to that much of the frame.
%
%  The medianRebin parameter can be used to specify that the median should be computed 
%  using this number of frames per data point, instead of all frames. This can help
%  reduce the amount of time required to motion correct, and also to obtain a sensible 
%  template for data that is very noisy or close to zero per frame.
%
%  The frameSkip parameter allows one to subsample the input movie in terms of frames.
%  It should be provided as a pair [offset, skip] where offset is the first frames to
%  skip, and skip is the number of frames to skip between reads. For example, 
%  frameSkip = [1 1] will start reading from the *second* frame and skip every other 
%  frame, i.e. read all even frames for motion correction. The produced shifts will
%  thus be fewer than the full movie and equal to the number of subsampled frames.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
