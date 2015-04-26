%% preparatory stuff
clear all classes
try 
    MIJ
    disp('you are good to go')
catch
    error('type Miji before starting')
end
%%
movie=uipickfiles; % select movie to be processed
%% create a security copy
frameRate=.033; % movie frame rate
movOrig=XMovie(movie,frameRate); % create movie object (load movie)
movOrig.saveToTiff('temp.tif') % create security copy to not affect original
[min_,max_,mean_,median_,std_]=movOrig.getStatistics(); % get some basic info about the movie
%% this is needed since scanimage 5 collects data with negative values. BEWARE THAT if you have negative values
% this toobox will not work. If you want to use negative values, it is at your
% own risk
movOrig=movOrig-min_; 
movOrig=movOrig.setBitDepth(16);
%% if you do not perform this operation matlab will not be able to properly read the values of the tiff file
% fundamentally there is 
% % mm.run('32-bit','') 
% % mm.run('16-bit','')

%% if you want to increase the SNR you can downsample in the z direction
if 1
    newNumberOfFrames=round(movOrig.numFrames/3);
    movOrig=movOrig.resize(0,0,newNumberOfFrames);
end
movOrig.saveToTiff('temp.tif') % save movie to compare

%% motion correct
xtot=[];
ytot=[];
maxShift=5;
numIterations=10;
for kk=1:numIterations 
    imgref=movOrig.zProject('median');
%     imgref=0;
    [movOrig,xshifts,yshifts]=movOrig.IJmotionCorrect(maxShift,imgref); 
    xtot=[xtot; xshifts];
    ytot=[ytot; yshifts];
    plot([xtot ytot])
    drawnow
    axis tight
end

%%
movOrig.saveToTiff('temp_mc.tif')
movOrig.movieId.close();
%%
save('tmp_shifts','xtot','ytot')
%% get the total motion for each frame
movMC=XMovie('./temp.tif');
xx=sum(reshape(xtot,[movMC.numFrames numel(xtot)/movMC.numFrames]),2);
yy=sum(reshape(ytot,[movMC.numFrames numel(ytot)/movMC.numFrames]),2);
%% motion correct the original movie by applying the shifts computed by the algorithm

subpixelreg=0; % =1 if you want subpixel registration.DISCLAIMER:it
            % will change the fluorescence of pixels because of the
            % interpolation
movMC.translateFrame(xx,yy,subpixelreg);
%%
movMC.saveToTiff('temp_mc_nosubpix.tif')
movMC.movieId.close();
%% compute DFF
movOrig=XMovie('./temp_mc_nosubpix.tif');
movOrig.frameRate=.1;
minValueF=15;
quantileMin=.08;
rollingWindowTime=10;
downsampleFactor=4;
movDFF=computeDFFMovie(movOrig,minValueF,downsampleFactor,rollingWindowTime,quantileMin);
movDFF.saveToTiff('./movDFF.tif')

%% retrieve the movie
movDFF=movDFF.setConstrast(0);
mat=movDFF.getMovie();
%% to retrieve the movie in single precision
%% to get the  movie back from matlab you have to play a trick because ImageJ cannot handle 
movDFF=XMovie('./movDFF.tif');
[min_,max_,mean_,median_,std_,statist]=movDFF.getStatistics;
movDFF=movDFF-min_;
movDFF=movDFF*1000;
movDFF=movDFF.setBitDepth(16,0);
mat=movDFF.getMovie();
mat=single(mat)/1000;
mat=(mat+min_);

%% get the movie in matlab format (slow)
data=mm.getMovie();
%% save as tiff with same name
mm.saveToTiff()
%% save as tiff with different name
mm.saveToTiff('andrea_playing.tif')