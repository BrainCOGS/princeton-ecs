%% Load a TIFF movie from disk and apply nonlinear motion correction on-the-fly
function movie = imreadnonlin( inputPath, mcorr, gridUpsample )
  
  %% Default arguments
  if nargin < 3
%     gridUpsample        = [4 4];
    gridUpsample        = [2 2];
  elseif numel(gridUpsample) < 2
    gridUpsample        = [gridUpsample gridUpsample];
  end
  
  %%
  nRows                 = mcorr.inputSize(1);
  nCols                 = mcorr.inputSize(2);
  nFrames               = mcorr.inputSize(end);
  gridSize              = [numel(mcorr.yCenter), numel(mcorr.xCenter)] .* gridUpsample;
  patchXShifts          = upsamplePatchShifts(mcorr.xShifts, gridSize);
  patchYShifts          = upsamplePatchShifts(mcorr.xShifts, gridSize);
  
  xCenter               = [1, round(linspace(mcorr.xCenter(1), mcorr.xCenter(end), gridSize(2))), nCols];
  yCenter               = [1, round(linspace(mcorr.yCenter(1), mcorr.yCenter(end), gridSize(1))), nRows]';
  patchNx               = diff(xCenter);
  patchNy               = diff(yCenter);
  patchXidx             = arrayfun(@(x,y) x + (0:y), xCenter(1:end-1), patchNx, 'UniformOutput', false);
  patchYidx             = arrayfun(@(x,y) x + (0:y), yCenter(1:end-1), patchNy, 'UniformOutput', false);
  
  
  %% Read input movie
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath, mcorr.rigid.xShifts, mcorr.rigid.yShifts);
  else
    movie               = inputPath;
    movie               = cv.imtranslatex(movie, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  end
  
  
  %%
%   profile on
  tic
  corrected             = nan(size(movie), 'like', movie);
  nPixels               = nRows * nCols;
  for iFrame = 1:nFrames
    %%
    xLoc                = double(bsxfun(@plus, patchXShifts(:,:,iFrame), xCenter));
    yLoc                = double(bsxfun(@plus, patchYShifts(:,:,iFrame), yCenter));
%     idx                 = knnsearch([yLoc(:),xLoc(:)], queryGrid);

    %% 
    for iX = 1:numel(xCenter)-1
      for iY = 1:numel(yCenter)-1
        %% Bounding coordinate rectangle
        patchX          = xLoc(iY:iY+1,iX:iX+1);
        patchY          = yLoc(iY:iY+1,iX:iX+1);
        boundX          = [ceil(min(patchX(:))), floor(max(patchX(:)))];
        boundY          = [ceil(min(patchY(:))), floor(max(patchY(:)))];
        
        %%
        x21             = patchX(:,2) - patchX(:,1);
        y21             = patchY(:,2) - patchY(:,1);
        x31             = patchX(2,:) - patchX(1,:);
        y31             = patchY(2,:) - patchY(1,:);
%         x21             = (x21(1) + x21(2)) / 2;
%         y21             = (y21(1) + y21(2)) / 2;
%         x31             = (x31(1) + x31(2)) / 2;
%         y31             = (y31(1) + y31(2)) / 2;
        x21             = max(x21(1), x21(2));
        y21             = max(y21(1), y21(2));
        x31             = max(x31(1), x31(2));
        y31             = max(y31(1), y31(2));
        
        norm21          = x21.^2 + y21.^2;
        norm31          = x31.^2 + y31.^2;
        
        %% All points inside rectangle
        [outX,outY]     = meshgrid(boundX(1):boundX(2), boundY(1):boundY(2));
        Px              = outX - patchX(1);
        Py              = outY - patchY(1);
        proj21          = ( Px*x21 + Py*y21 ) / norm21;
        proj31          = ( Px*x31 + Py*y31 ) / norm31;
        inside          = proj21 >= 0 & proj21 <= 1 & proj31 >= 0 & proj31 <= 1;
        proj21          = proj21(inside) * patchNx(iX);
        proj31          = proj31(inside) * patchNy(iY);
        i21             = floor(proj21);
        i31             = floor(proj31);
        w21             = proj21 - i21;
        w31             = proj31 - i31;
        
        %%
        rows            = yCenter(iY) + i31;
        cols            = xCenter(iX) + i21;
        oFrame          = nPixels*(iFrame-1);
        interpolated    = (1 - w21).*(1 - w31).*movie(rows   + nRows*(cols-1) + oFrame)   ...
                        +      w21 .*(1 - w31).*movie(rows   + nRows*(cols  ) + oFrame)   ...
                        + (1 - w21).*     w31 .*movie(rows+1 + nRows*(cols-1) + oFrame)   ...
                        +      w21 .*     w31 .*movie(rows+1 + nRows*(cols  ) + oFrame)   ...
                        ;
                      
        %%
        index           = outY(inside) + nRows*(outX(inside) - 1) + oFrame;
        corrected(index)= interpolated;
      end
    end
  end
  toc
%   profile viewer
  
end

%---------------------------------------------------------------------------------------------------
function shifts = upsamplePatchShifts(origShifts, gridSize)
  
  numFrames                 = size(origShifts,3);
  shifts                    = nan([gridSize + 2, numFrames], 'like', origShifts);
  shifts(2:end-1,2:end-1,:) = imresize( origShifts, gridSize, 'bicubic' );
  shifts(2:end-1,1,:)       = shifts(2:end-1,2,:);
  shifts(2:end-1,end,:)     = shifts(2:end-1,end-1,:);
  shifts(1,2:end-1,:)       = shifts(2,2:end-1,:);
  shifts(end,2:end-1,:)     = shifts(end-1,2:end-1,:);
  shifts(1,[1 end],:)       = shifts(2,[1 end],:);
  shifts(end,[1 end],:)     = shifts(end-1,[1 end],:);
  
end
