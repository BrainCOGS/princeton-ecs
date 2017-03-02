% GETSYNCINFO    Parses the TIFF file header to extract ScanImage specific synchronization information.
%
%  Usage syntax:
%      [acquisition, epoch, frameTime, dataTime, data] = getSyncInfo(inputFile, dataType, frameSkip = [0 0])
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
function varargout = getSyncInfo(varargin)
  error(sprintf('%s:migrated', mfilename()), 'Oops sorry, this function has been moved to cv.%s', mfilename());
end
