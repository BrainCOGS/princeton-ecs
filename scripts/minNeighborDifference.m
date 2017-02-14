function minDiffNN = minNeighborDifference(values)
  
  %%
  minDiffNN             = inf(size(values));
  diffNN                = nan(size(values));
  sel                   = false(size(values));
  
  %% N
  sel(:)                = false;
  src                   = 2:size(values,1);
  diffNN(src,:,:)       = abs(values(src,:,:) - values(src-1,:,:));
  sel(src,:,:)          = diffNN(src,:,:) < minDiffNN(src,:,:);
  minDiffNN(sel)        = diffNN(sel);
  
  %% S
  sel(:)                = false;
  src                   = 1:size(values,1)-1;
  diffNN(src,:,:)       = abs(values(src,:,:) - values(src+1,:,:));
  sel(src,:,:)          = diffNN(src,:,:) < minDiffNN(src,:,:);
  minDiffNN(sel)        = diffNN(sel);
  
  %% E
  sel(:)                = false;
  src                   = 1:size(values,2)-1;
  diffNN(:,src,:)       = abs(values(:,src,:) - values(:,src+1,:));
  sel(:,src,:)          = diffNN(:,src,:) < minDiffNN(:,src,:);
  minDiffNN(sel)        = diffNN(sel);
  
  %% W
  sel(:)                = false;
  src                   = 2:size(values,2);
  diffNN(:,src,:)       = abs(values(:,src,:) - values(:,src-1,:));
  sel(:,src,:)          = diffNN(:,src,:) < minDiffNN(:,src,:);
  minDiffNN(sel)        = diffNN(sel);
  
end
