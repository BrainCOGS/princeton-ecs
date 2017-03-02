% ADAPTIVEMEDIANFILTER    Computes a median filtered image using a mask to select pixels to include. NaN-valued pixels are ignored.
%
%  Usage syntax:
%    filtered  = ecs.adaptiveMedianFilter(image, category, numCategories, targetFracPixels, [isSelected = []], [emptyValue = nan]);
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
function varargout = adaptiveMedianFilter(varargin)
  error(sprintf('%s:migrated', mfilename()), 'Oops sorry, this function has been moved to cv.%s', mfilename());
end
