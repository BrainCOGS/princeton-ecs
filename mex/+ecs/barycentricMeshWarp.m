% BARYCENTRICMESHWARP    Very fast warping with locally linear assumptions.
%
%  Usage syntax:
%    interpolated = ecs.barycentricMeshWarp(source, xSample, ySample, xTarget, yTarget);
%
%  *Sample should be locations in pixel units of the original measurements.
%  *Target are the locations in pixel units to which the original image should be warped,
%  specified as a function of time.
%
function varargout = barycentricMeshWarp(varargin)
  error(sprintf('%s:migrated', mfilename()), 'Oops sorry, this function has been moved to cv.%s', mfilename());
end
