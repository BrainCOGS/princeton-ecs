function [xymotion]=agMotionCorrectionImageJ(imageplusinstance,winSizeX,winSizeY,xo,yo,methodCorr,methodInterp,maxShift,referenceImage)
% methodCorr  {"Square difference", "Normalized square difference", "Cross correlation", "Normalized cross correlation", "Correlation coefficient", "Normalized correlation coefficient"};
% 0 1 2 3 4 5
% methodInterp; //{"Bilinear", "Bicubic"}; 0 1
%% get the average image
ig=imageplusinstance;
igtitle=ig.getTitle();
if ~exist('referenceImage')
    referenceImage=0;
end

if ~isscalar(referenceImage) || referenceImage==0 
    if referenceImage==0
        MIJ.run('Z Project...', 'projection=[Median]');
    else
        MIJ.createImage(referenceImage);        
    end
    MIJ.run('16-bit')
    igavg=ij.IJ.getImage();
    avgtitle=igavg.getTitle();      
	argmacro=['title=' char(igtitle) ' image1=[' string(avgtitle)  '] image2=[' string(igtitle) ']'];
    MIJ.run('Concatenate...', argmacro);
    igavg.close()
else
    ij.IJ.setSlice(referenceImage);
end
   
% concatimagetitle='concatimage2';

%%
if nargin>1
    MIJ.run('Align slices in stack...',['method = ' num2str(methodCorr) ...
        ' windowsizex=' num2str(winSizeX) ...
        ' windowsizey=' num2str(winSizeY) ...
        ' x0=' num2str(xo) ...
        ' y0=' num2str(yo) ...
        ' swindow=' num2str(maxShift) ...
        ' subpixel=TRUE '...
        ' itpmethod=' num2str(methodInterp) ...
        ' ref.slice=1 show=TRUE']);
else
    MIJ.run('Align slices in stack...');    
end
%%
MIJ.run('Slice Remover', 'first=1 last=1 increment=1');
xymotion=MIJ.getResultsTable();
MIJ.run('Clear Results');
