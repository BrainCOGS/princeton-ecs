%% preparatory stuff
clear all classes
try 
    MIJ.run('Conversions...','[scale when converting]=0');
catch
    error('type Miji before starting')
end
%%
movie=uipickfiles; % select movie to be processed
%%
frameRate=.033; % movie frame rate
make_copy=1;
mm=MovieIJ(movie,frameRate,make_copy); % create movie object
[min_,max_,mean_,median_,std_]=mm.getStatistics(); % get some basic info about the movie
 mm=mm-min_;
% if (min_>=-250)
%     mm=mm-(-250); % to make sure movie is larger than 0. Use a large value that will be ok for evey movie
% else 
%     error('The selected value is not large enough to make each pixel positive')
% end

%% if you do not perform this operation matlab will not be able to properly read the values of the tiff file
mm.run('32-bit','') 
mm.run('16-bit','')

%% if you wasnt to increase the SNR you can downsample in the z direction
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
%% save as tiff

%% get the movie in matlab format (slow)
data=mm.getMovie();
%% save as tiff with same name
mm.saveToTiff()
%% save as tiff with different name
mm.saveToTiff('andrea_playing.tif')