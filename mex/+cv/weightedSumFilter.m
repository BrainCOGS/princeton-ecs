% WEIGHTEDSUMFILTER    Computes a median filtered image using a selection mask to select pixels to include. 
%
%  Usage syntax:
%    filtered  = cv.weightedSumFilter(image, weight, [masked = []], [isSelected = []], [minWeight = 0], [emptyValue = nan]);
%
%  NaN-valued pixels are ignored.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%
