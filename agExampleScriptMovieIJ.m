%% preparatory stuff
clear all classes
try 
    MIJ
    disp('you are good to go')
catch
    error('type Miji before starting')
end
%% this prevents any rescaling to happen when switching between 16 and 32 bits
MIJ.run('Conversions...','[scale when converting]=0'); % 
%%
movie=uipickfiles; % select movie to be processed
%% create a security copy
frameRate=.033; % movie frame rate
mm=MovieIJ(movie,frameRate); % create movie object (load movie)
mm.saveToTiff('temp.tif') % create security copy to not affect original
[min_,max_,mean_,median_,std_]=mm.getStatistics(); % get some basic info about the movie
%% this is needed since scanimage 5 collects data with negative values. BEWARE THAT if you have negative values
% this toobox will not work. If you want to use negative values, it is at your
% own risk
mm=mm-min_; 
%% if you do not perform this operation matlab will not be able to properly read the values of the tiff file
% fundamentally there is 
mm.run('32-bit','') 
mm.run('16-bit','')
%%
mm.saveToTiff('temp.tif') % save movie to compare
%% if you want to increase the SNR you can downsample in the z direction
if 1
    newNumberOfFrames=round(mm.numFrames/3);
    mm=mm.resize(0,0,newNumberOfFrames);
end
%% motion correct
xtot=[];
ytot=[];
maxShift=5;
numIterations=10;
for kk=1:numIterations 
    imgref=mm.zProject('median');
%     imgref=0;
    [mm,xshifts,yshifts]=mm.IJmotionCorrect(maxShift,imgref); 
    xtot=[xtot; xshifts];
    ytot=[ytot; yshifts];
    plot([xtot ytot])
    drawnow
    axis tight
end
%%
mm.saveToTiff('temp_mc.tif')
mm.movieId.close();
%%
save('tmp_shifts','xtot','ytot')
%% get the total motion for each frame
xx=sum(reshape(xtot,[mm.numFrames numel(xtot)/mm.numFrames]),2);
yy=sum(reshape(ytot,[mm.numFrames numel(ytot)/mm.numFrames]),2);
%% motion correct the original movie by applying the shifts computed by the algorithm
mc=MovieIJ('./temp.tif',frameRate);
subpixelreg=0; % =1 if you want subpixel registration.DISCLAIMER:it
            % will change the fluorescence of pixels because of the
            % interpolation
mc.translateFrame(xx,yy,subpixelreg);
%% get the movie in matlab format (slow)
data=mm.getMovie();
%% save as tiff with same name
mm.saveToTiff()
%% save as tiff with different name
mm.saveToTiff('andrea_playing.tif')