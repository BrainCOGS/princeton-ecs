% ABSMINFILTER    Computes a median filtered image using a selection mask to select pixels to include. 
%
%  Usage syntax:
%    filtered  = ecs.absMinFilter(image, weight, [refValue = 0], [masked = []], [isSelected = []], [minWeight = 0], [emptyValue = nan]);
%
%  NaN-valued pixels are ignored.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
function varargout = absMinFilter(varargin)
  error(sprintf('%s:migrated', mfilename()), 'Oops sorry, this function has been moved to cv.%s', mfilename());
end
