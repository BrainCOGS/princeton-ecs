%% Load a TIFF movie from disk and apply nonlinear motion correction on-the-fly
function movie = imreadnonlin( inputPath, mcorr, gridUpsample )
  
  %% Default arguments
  if nargin < 3
%     gridUpsample        = [4 4];
%     gridUpsample        = [2 2];
    gridUpsample        = [1 1];
  elseif numel(gridUpsample) < 2
    gridUpsample        = [gridUpsample gridUpsample];
  end
  
  %%
  nRows                 = mcorr.inputSize(1);
  nCols                 = mcorr.inputSize(2);
  nFrames               = mcorr.inputSize(end);
  gridSize              = [numel(mcorr.yCenter), numel(mcorr.xCenter)] .* gridUpsample;
  
%   xShifts               = mcorr.xShifts;
%   yShifts               = mcorr.yShifts;
  xShifts               = imgaussfilt(mcorr.xShifts, 0.5);
  yShifts               = imgaussfilt(mcorr.yShifts, 0.5);
  patchXShifts          = upsamplePatchShifts(xShifts, gridSize);
  patchYShifts          = upsamplePatchShifts(yShifts, gridSize);
  
  xCenter               = [1, round(linspace(mcorr.xCenter(1), mcorr.xCenter(end), gridSize(2))), nCols];
  yCenter               = [1, round(linspace(mcorr.yCenter(1), mcorr.yCenter(end), gridSize(1))), nRows]';
  patchNx               = diff(xCenter);
  patchNy               = diff(yCenter);
  
  
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
  truncator             = {@floor, @ceil};
  for iFrame = 1:nFrames
    %%
    xLoc                = double(bsxfun(@plus, patchXShifts(:,:,iFrame), xCenter));
    yLoc                = double(bsxfun(@plus, patchYShifts(:,:,iFrame), yCenter));
    frameOffset         = nPixels * (iFrame-1);
%     figure; hold on; plot(xLoc(:),yLoc(:),'ko'); axis image ij;
%     figure; imagesc(patchXShifts(:,:,iFrame)); axis image ij; colorbar
%     figure; imagesc(patchYShifts(:,:,iFrame)); axis image ij; colorbar

    %% 
    for iX = 1:numel(xCenter)-1
      for iY = 1:numel(yCenter)-1
        %%
        iTri            = 0;
        nSources        = [patchNx(iX); patchNy(iY)];
        invSources      = 1./nSources;
        
        %%
        for iDir = [1 -1]
          fTrunc        = truncator{1 + (iDir > 0)};
          
          %% Triangle
          oWarped       = [ xLoc(iY+iTri,iX+iTri) ; yLoc(iY+iTri,iX+iTri) ];
          shape         = [ xLoc(iY+iTri,iX+iTri+iDir) - oWarped(1) , xLoc(iY+iTri+iDir,iX+iTri) - oWarped(1)   ...
                          ; yLoc(iY+iTri,iX+iTri+iDir) - oWarped(2) , yLoc(iY+iTri+iDir,iX+iTri) - oWarped(2)   ...
                          ];
          invShape      = [ shape(2,2), -shape(1,2) ; -shape(2,1), shape(1,1) ]   ...
                        / ( shape(1,1)*shape(2,2) - shape(1,2)*shape(2,1) )       ...
                        ;
          oGrid         = fTrunc(oWarped);
          
          %% Query grid in barycentric coordinates
          bcGridOrig    = invShape * (oGrid - oWarped);
          nMax          = fTrunc(shape * (1 - bcGridOrig));
          %{
          jScale        = sum(invShape(:,2));
          jiSlope       = sum(invShape(:,1))      / jScale;
          jiOffset      = ( 1 - sum(bcGridOrig) ) / jScale;
          iGrid         = 0:iDir:nMax(1);
          jMin          = fHiInt( jiOffset - iGrid * jiSlope - 1/jScale );
          jMax          = fHiInt( jiOffset - iGrid * jiSlope );
          nPoints       = iDir * sum(jMax);

          %%
          for iG = iGrid
            index       = abs(iG) + 1;
%             jGrid       = jMin(index):jMax(index);
            jGrid       = 0:jMax(index);
            bcGrid      = bcGridOrig + iG*invShape(:,1);
            bcGrid      = [ bcGrid(1) + jGrid*invShape(1,2)     ...
                          ; bcGrid(2) + jGrid*invShape(2,2)     ...
                          ];
            rGrid       = bsxfun(@plus, oWarped, shape * bcGrid);
            plot(rGrid(1,:), rGrid(2,:), 'r.', 'markersize', 2);
          end
          %}
          
          
          %%
%           %{
          [iGrid,jGrid] = meshgrid( -iDir:iDir:nMax(1), -iDir:iDir:nMax(2) );
          bcGrid        = bsxfun( @plus                                         ...
                                , bcGridOrig                                    ...
                                , bsxfun(@times, iGrid(:)', invShape(:,1))      ...
                                + bsxfun(@times, jGrid(:)', invShape(:,2))      ...
                                );
          sel           = bcGrid(1,:)               >= 0                        ...
                        & bcGrid(2,:)               >= 0                        ...
                        & bcGrid(1,:) + bcGrid(2,:) <= 1                        ...
                        ;
          bcGrid        = bcGrid(:,sel);
          drGrid        = shape * bcGrid;
          rGrid         = bsxfun(@plus, oWarped, drGrid);
          
          %{
          if iDir > 0
            plot(rGrid(1,:), rGrid(2,:), 'r.', 'markersize', 2);
          else
            plot(rGrid(1,:), rGrid(2,:), 'b.', 'markersize', 2);
          end
          %}
          
          %% Barycentric interpolation using nearest source pixel
%           drGrid
          srcGrid       = bsxfun(@times, bcGrid, nSources);           % rows are barycentric
          srcIndex      = floor(srcGrid);
          relOrig       = bsxfun(@times, srcIndex(1,:)*invSources(1), shape(:,1))     ...
                        + bsxfun(@times, srcIndex(2,:)*invSources(2), shape(:,2))     ...
                        ;
          scaledInv     = bsxfun(@times, invShape, nSources);
          bcCoords      = scaledInv * ( drGrid - relOrig );
          
          %% Upper right triangle
          jDir          = ones(1,size(bcCoords,2));
          up            = sum(bcCoords,1) > 1;
          jDir(up)      = -1;

          srcIndex(:,up)= ceil(srcGrid(:,up));
          relOrig(:,up) = bsxfun(@times, srcIndex(1,up)*invSources(1), shape(:,1))    ...
                        + bsxfun(@times, srcIndex(2,up)*invSources(2), shape(:,2))    ...
                        ;
          bcCoords(:,up)= -scaledInv * ( drGrid(:,up) - relOrig(:,up) );
          
          bcCoords(end+1,:) = 1 - sum(bcCoords,1);
          
          %% 
          rGrid         = round(rGrid);
          rowOrig       = yCenter(iY+iTri) + iDir*srcIndex(2,:);
          colOrig       = xCenter(iX+iTri) + iDir*srcIndex(1,:);
          rowSide       = rowOrig + jDir;
          colSide       = colOrig + jDir;
          
          valid         = rowSide > 0 & rowSide <= nRows & colSide > 0 & colSide <= nCols;
          rowOrig       = rowOrig(valid);
          rowSide       = rowSide(valid);
          colOrig       = colOrig(valid);
          colSide       = colSide(valid);
          
          srcValue      = [ movie(rowOrig + nRows*(colSide - 1) + frameOffset)      ...
                          ; movie(rowSide + nRows*(colOrig - 1) + frameOffset)      ...
                          ; movie(rowOrig + nRows*(colOrig - 1) + frameOffset)      ...
                          ];
          corrected( rGrid(2,valid) + nRows*(rGrid(1,valid) - 1) + frameOffset )    ...
                        = sum(bcCoords(:,valid) .* srcValue, 1);

          %%
          iTri          = iTri + 1;
        end
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

%---------------------------------------------------------------------------------------------------
function value = safeRead(tensor, index)
  
  value       = nan(size(index));
  sel         = index > 0 & inde
  
end
