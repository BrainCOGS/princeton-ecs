%% IMPLAY   A better configuration for movie player.
function player = implay(img, fps, saturation)

  % Default arguments
  if nargin < 2
    fps             = 60;
  end
  if nargin < 3
    saturation      = 0.01;
  end

  % Set up player
  player            = implay(img, fps);
  
  % Set display region
  sizeTool          = findall(player.Parent, 'Tag', 'uimgr.uitoggletool_Maintain');
  sizeTool.ClickedCallback();

  % Compute optimal range of colormap
  [freq, edges]     = histcounts(img);
  pixValue          = double(edges(2:end));
  pixCDF            = cumsum(double(freq));
  pixCDF            = pixCDF / pixCDF(end);
  pixRange          = interp1(pixCDF, pixValue, [saturation, 1-saturation]);
  
  player.Visual.ColorMap.UserRangeMin   = pixRange(1);
  player.Visual.ColorMap.UserRangeMax   = pixRange(2);
  player.Visual.ColorMap.UserRange      = 1;
  
end