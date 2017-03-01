% MEDIANFILTER    Computes a median filtered image using a mask to select pixels to include. NaN-valued pixels are ignored.
%
%  Usage syntax:
%    filtered  = ecs.medianFilter(image, mask, isSelected);
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
function medianFilter(varargin)
  error(sprintf('%s:migrated', mfilename()), 'Oops sorry, this function has been moved to cv.%s', mfilename());
end
