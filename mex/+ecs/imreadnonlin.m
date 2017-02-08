%% Load a TIFF movie from disk and apply nonlinear motion correction on-the-fly
%   Copied and modified from normcorre() in https://github.com/simonsfoundation/NoRMCorre
%   HACK HACK HACK Right now ths is only correct for the mot_uf > 1 case
function movie = imreadnonlin( inputPath, shifts, reference, options )
  
  %% Read input movie
  if ischar(inputPath)
    input               = cv.imreadx(inputPath);
  else
    input               = inputPath;
  end
  
  %% Supporting computations
  sizY                  = size(input);
  nd                    = length(sizY) - 1;
  sizY                  = sizY(1:nd);
  [d1,d2,d3,T]          = size(input);
  if nd == 2;
    T                   = d3;
    d3                  = 1; 
  end

  [xx_s,xx_f,yy_s,yy_f,zz_s,zz_f,xx_us,xx_uf,yy_us,yy_uf,zz_us,zz_uf]   ...
                        = construct_grid(options.grid_size, options.mot_uf, options.d1, options.d2, options.d3, options.min_patch_size);
  temp_cell             = mat2cell_ov(reference, xx_us, xx_uf, yy_us, yy_uf, zz_us, zz_uf, options.overlap_post, sizY);

  Nr                    = cell(size(temp_cell));
  Nc                    = cell(size(temp_cell));
  Np                    = cell(size(temp_cell));
  Bs                    = cell(size(temp_cell));
  for i = 1:length(xx_us)
    for j = 1:length(yy_us)
      for k = 1:length(zz_us)
        [nr,nc,np]      = size(temp_cell{i,j,k});
        nr              = ifftshift(-fix(nr/2):ceil(nr/2)-1);
        nc              = ifftshift(-fix(nc/2):ceil(nc/2)-1);
        np              = ifftshift(-fix(np/2):ceil(np/2)-1);
        [Nc{i,j,k},Nr{i,j,k},Np{i,j,k}]                                   ...
                        = meshgrid(nc,nr,np);
        extended_grid   = [ max(xx_us(i)-options.overlap_post(1),1)       ...
                          , min(xx_uf(i)+options.overlap_post(1),d1)      ...
                          , max(yy_us(j)-options.overlap_post(2),1)       ...
                          , min(yy_uf(j)+options.overlap_post(2),d2)      ...
                          , max(zz_us(k)-options.overlap_post(3),1)       ...
                          , min(zz_uf(k)+options.overlap_post(3),d3)      ...
                          ];
        Bs{i,j,k}       = permute(construct_weights([xx_us(i),xx_uf(i),yy_us(j),yy_uf(j),zz_us(k),zz_uf(k)],extended_grid),[2,1,3]); 
      end
    end
  end
  
  if nd == 2
    Np                  = cellfun(@(x) 0,Nr,'un',0); 
  end
  input                 = num2cell(input, 1:nd);

  %%
  output                = cell(size(input));
  parfor iFrame = 1:T
    %% Mean-zeroed frame
    Yt                  = input{iFrame};
    minY                = min(Yt(:));
    maxY                = max(Yt(:));
    Yt                  = Yt + options.add_value;

    %% 
    M_fin               = cell(length(xx_us),length(yy_us),length(zz_us));
    for i = 1:length(xx_uf)
      for j = 1:length(yy_uf)
        for k = 1:length(zz_uf)
          extended_grid = [ max(xx_us(i)-options.overlap_post(1),1)           ...
                          , min(xx_uf(i)+options.overlap_post(1),d1)          ...
                          , max(yy_us(j)-options.overlap_post(2),1)           ...
                          , min(yy_uf(j)+options.overlap_post(2),d2)          ...
                          , max(zz_us(k)-options.overlap_post(3),1)           ...
                          , min(zz_uf(k)+options.overlap_post(3),d3)          ...
                          ];
          I_temp        = Yt( extended_grid(1):extended_grid(2)               ...
                            , extended_grid(3):extended_grid(4)               ...
                            , extended_grid(5):extended_grid(6)               ...
                            );
          M_fin{i,j,k}  = shift_reconstruct ( I_temp, shifts(iFrame).shifts_up(i,j,k,:), shifts(iFrame).diff(i,j,k)     ...
                                            , options.us_fac, Nr{i,j,k},Nc{i,j,k},Np{i,j,k}, 'NaN', options.add_value   ...
                                            );
        end
      end
    end

    %%
    gx                  = max(abs(reshape(diff(shifts(iFrame).shifts_up,[],1),[],1)));
    gy                  = max(abs(reshape(diff(shifts(iFrame).shifts_up,[],2),[],1)));
    gz                  = max(abs(reshape(diff(shifts(iFrame).shifts_up,[],3),[],1)));
    flag_interp         = max([gx;gy;gz;0])<0.5;      % detect possible smearing
    
    if flag_interp
      Mf                = cell2mat_ov_sum(M_fin,xx_us,xx_uf,yy_us,yy_uf,zz_us,zz_uf,options.overlap_post,sizY,Bs) - options.add_value;
    else
      Mf                = cell2mat_ov(M_fin,xx_us,xx_uf,yy_us,yy_uf,zz_us,zz_uf,options.overlap_post,sizY) - options.add_value;
    end
    
    Mf(Mf<minY)         = minY;
    Mf(Mf>maxY)         = maxY;
    output{iFrame}      = Mf;
  end
  

  %%
  movie                 = cat(nd+1, output{:});

end

