% how to install
This class implements a general movie class providing a wrapper for
ImageJ. In order to use this class you must have Fiji installed,
as well as the Miji wrapper properly installed and started. For some methods
like IJmotionCorrection and for neuronal extractions other packages
might be needed.


Requirements and installation

1) Matlab 2013b
2) Fiji installed (http://fiji.sc/Fiji)
3) Miji installed  (http://fiji.sc/Miji)
4) IMageJ align slices in stack installed (https://sites.google.com/site/qingzongtseng/template-matching-ij-plugin)
5) Add the Fiji scripts folfer to matlab yourpath and you should be good to go. 
6) Go to Matlab Preferences and increase the java heap size (Preferences->General->Memory and Heap Size)
9) Restart Matlab
7) Type Miji + enter 
8) Fiji should open. Go to the menu entry Edit->options->Memory and Threads and set the max memory for Fiji to the 
   same value you used for the heap size memory
9) Before starting anything type Miji and enter. This is required for the class to work. 


EXAMPLE OF USE

movie=uipickfiles; % select movie to be processed
frameRate=.033; % movie frame rate

mm=MovieIJ(movie,frameRate); % create movie object
[min_,max_,mean_,median_,std_]=mm.getStatistics(); % get some basic info about the movie
mm=mm-min_; % to make sure movie is larger than 0.

%make to make sure the format is consistent for different cases.
mm.run('32-bit','')
mm.run('16-bit','')

% if you wasnt to increase the SNR you can downsample in the z direction

newNumberOfFrames=round(mm.numFrames/3);
mm=mm.resize(0,0,newNumberOfFrames);
%motion correct
xtot=[];
ytot=[];
maxShift=5;
numIterations=10;
for kk=1:numIterations
  [mm,xshifts,yshifts]=mm.IJmotionCorrect(maxShift);
  xtot=[xtot; xshifts];
  ytot=[ytot; yshifts];
  plot([xtot ytot])
  drawnow
  axis tight
end

author:Andrea Giovannucci