%% IMPLAY   A better configuration for movie player.
function player = implay(img, fps, saturation)

  % Default arguments
  if nargin < 2
    fps               = 100;
  end
  if nargin < 3
    saturation        = 0.001;
  end

  % Set up player
  player              = implay(img, fps);
  
  % Set display region
  sizeTool            = findall(player.Parent, 'Tag', 'uimgr.uitoggletool_Maintain');
  sizeTool.ClickedCallback();

  % Linear interpolation cannot handle non-increasing values
  if numel(saturation) == 2
    pixRange          = saturation;
    
  else
    % Compute optimal range of colormap using mean image
    imgPix            = mean(img, 3, 'omitnan');
    imgPix            = imgPix(isfinite(imgPix));

    % Compute empirical estimate of CDF via linear interpolation
    [pixCDF,pixValue] = ecdf(imgPix);
    pixValue          = double(pixValue(2:end));
    pixCDF            = double( pixCDF(1:end-1) + pixCDF(2:end) ) / 2;
    pixRange          = interp1(pixCDF, pixValue, [saturation, 1-saturation], 'linear', 'extrap');
  end
  
  % Set colormap range
  player.Visual.ColorMap.UserRangeMin   = pixRange(1);
  player.Visual.ColorMap.UserRangeMax   = pixRange(2);
  player.Visual.ColorMap.UserRange      = 1;
  
end
