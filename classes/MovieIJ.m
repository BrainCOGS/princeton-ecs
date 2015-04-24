classdef MovieIJ
%	This class implements a general movie class providing a wrapper for
%     ImageJ. In order to use this class you must have Fiji installed,
%     as well as the Miji wrapper properly installed and started. For some methods
%     like IJmotionCorrection and for neuronal extractions other packages
%     might be needed.
%
%
%     EXAMPLE OF USE
% 
%     movie=uipickfiles; % select movie to be processed
%     frameRate=.033; % movie frame rate
% 
%     mm=MovieIJ(movie,frameRate); % create movie object
%     [min_,max_,mean_,median_,std_]=mm.getStatistics(); % get some basic info about the movie
%     mm=mm-min_; % to make sure movie is larger than 0.
%     
%     %make to make sure the format is consistent for different cases.
%     mm.run('32-bit','')
%     mm.run('16-bit','')
%     
%     % if you wasnt to increase the SNR you can downsample in the z direction
%
%     newNumberOfFrames=round(mm.numFrames/3);
%     mm=mm.resize(0,0,newNumberOfFrames);
%     %motion correct
%     xtot=[];
%     ytot=[];
%     maxShift=5;
%     numIterations=10;
%     for kk=1:numIterations
%         [mm,xshifts,yshifts]=mm.IJmotionCorrect(maxShift);
%         xtot=[xtot; xshifts];
%         ytot=[ytot; yshifts];
%         plot([xtot ytot])
%         drawnow
%         axis tight
%     end
% 
% author:Andrea Giovannucci

    properties (SetAccess = public)
        frameRate % if movies are collected in chunks and then joined this variable contain the name of each          
        timeVector % time points corresponding to frames
        movieId % retrieved from scanimage
        numFrames % total number of framess
        linesPerFrame %lines scanned in scan image per frame
        pixelsPerLine % number of pixels per line
        moviePath % path to the tif scanimage file
    end
    
    methods (Static)
        function setTiffParameter(filename,param,val)
            % set metadata of tiff file (only the default fields)
            % setTiffParameter(filename,param,val)
            % Example
            % MovieIJ.setTiffParameter(filename,'Software','Matlab v1')
            % See also
            % MovieIJ\getTiffParameter,MovieIJ\getTiffParameterList,MovieIJ\getImageDescrParameters
             t = Tiff(filename,'r+');
             setTag(t,param,val);
             try
                rewriteDirectory(t)
             catch e
                 e.message()
             end
             t.close()
        end
        
        function parval=getTiffParameter(filename,param)
            % get metadata of tiff file (only the default fields)
            % parval=getTiffParameter(filename,param)
            % Example
            % MovieIJ.getTiffParameter(filename,'Software')
            % See also
            % MovieIJ\setTiffParameter,MovieIJ\getTiffParameterList,MovieIJ\getImageDescrParameters
             t = Tiff(filename,'r+');
             parval=getTag(t,param);
             t.close()
        end
        
        function pars=getTiffParameterList(filename)
            % get the list of default tiff parameters
            % pars=getTiffParameterList(filename)
            t = Tiff(filename,'r+');
             pars=t.TagID;
             t.close()
        end
        
        function propStruct=parseImageDescription(imgInfo)
               % parse the image description field as collected by scanimage 
               % propStruct=parseImageDescription(imgInfo)
               % imgInfo is the ImageDescription field of the 
               % tif file. It works if it is stored in the format
               % field1=value1
               % field2=value2
               % ....
               values = textscan(imgInfo, '%s', 'delimiter', '\n');
               pairs=cellfun(@(X) strsplit(X,'='), values{1},'UniformOUtput',0);
               keys=cellfun(@(X) X{1}, pairs,'UniformOUtput',0);
               vals=cellfun(@(X) X{2}, pairs,'UniformOUtput',0);               
               propStruct=cell2struct(vals,keys);                                      
        end
        
        function propStruct=getImageDescrParameters(filename)
            % retrieve use specific metadata key value pairs stored in tiff
            % files
            % propStruct=getImageDescrParameters(filename)
            % propStruct is a struct type
            % filename is the tif file filename
            % It works if it is stored in the format
            % field1=value1
            % field2=value2
            % ....
            % Example
            % MovieIJ.setImageDescrParameter(moviePath,'timeVector',obj.timeVector);
            % See also MovieIJ\getImageDescrParameter, MovieIJ\setImageDescrParameter
            
            imgInfo=MovieIJ.getTiffParameter(filename,'ImageDescription');
            propStruct=MovieIJ.parseImageDescription(imgInfo);
            
        end
        
        function setImageDescrParameter(filename,par,val)
            % store user specific metadata in the tiff file
            % using the ImageDescription field 
            % setImageDescrParameter(filename,par,val)
            % par is a string and val can be string, array or cell
            % Examples
            % MovieIJ.setImageDescrParameter(moviePath,'frameRate',frameRate);
            % See also MovieIJ\getImageDescrParameter, MovieIJ\getImageDescrParameters
             propStruct=MovieIJ.getImageDescrParameters(filename);
             propStruct.(par)=val;   
             fields=fieldnames(propStruct);
             % here we store the files in the format specified above.
             % FieldX=valueX
             str=[];
             for field=1:numel(fields)
                 if iscell(propStruct.(fields{field}))
                    str=[str fields{field} '={''', strjoin(propStruct.(fields{field}),''',''') '''}']; 
                 else
                     if isnumeric(propStruct.(fields{field}))
                        str=[str sprintf('%s=%s',fields{field},mat2str(propStruct.(fields{field})))]; 
                     else
                        str=[str sprintf('%s=%s',fields{field},propStruct.(fields{field}))];
                     end
                 end
                 str=[str sprintf('\n')];
             end          
             MovieIJ.setTiffParameter(filename,'ImageDescription',str);             
        end
        
        function val=getImageDescrParameter(filename,par)
            % complementary to setImageDescrParameter
            % val=getImageDescrParameter(filename,par)
            % See also MovieIJ\setImageDescrParameter, MovieIJ\getImageDescrParameters
            propStruct=MovieIJ.getImageDescrParameters(filename);
            val=propStruct.(par);                   
        end                   
        
    end
    
    methods (Access = protected)
    end % methods
    
    
    methods (Access = public)
        %% constructor
        function obj = MovieIJ(moviePath,frameRate,copytmp)
            % constructor for the MovieIJ class
            % obj = MovieIJ(moviePath,frameRate,copytmp)
            % moviePath: cell containing path to tif file. Cell since in the future might contain multiple files to be chained together
            % frameRate: frame rate of the collected movie (at the moment only equally spaced frames are supported)
            % copytmp: flag to decide whether to operate directly on the
            % tiff file or create a security copy (with a '_tmp" extension
            % appended)
            % 
            % EXAMPLES
            % movie=uipickfiles; % select movie to be processed
            % frameRate=.033; % movie frame rate
            % 
            % mm=MovieIJ(movie,frameRate); % create movie object
            
            if nargin<3 % by default create a security copy
                copytmp=1;
            end            
            
            obj.movieId=ij.IJ.openImage(moviePath);  
            if isempty(obj.movieId)
                error('File not opened (possibly not found)')
            end
                                    
            if copytmp %makes security copy in case writing damages the file
                moviePath={[moviePath{1}(1:end-4) '_tmp.tif']};
                ij.IJ.saveAsTiff(obj.movieId,moviePath);
                obj.movieId.close();
                obj.movieId=ij.IJ.openImage(moviePath); 
            end
            try % in case we are using a bare tiff file with no metainformation
                obj.numFrames=str2num(MovieIJ.getImageDescrParameter(moviePath{1},'frames'));            
            catch
                warning('Metainformation extraced by movie');
                [~,~,obj.numFrames]=getDimensions(obj);
            end
            obj.moviePath=moviePath;
%             obj.timeVector=frameRate:frameRate:frameRate*(obj.numFrames);    
            [pixelsperline,linesperframe,~]=getDimensions(obj);
            obj.linesPerFrame=linesperframe;
            obj.pixelsPerLine=pixelsperline; 
            obj.frameRate=frameRate;
            
            MovieIJ.setImageDescrParameter(moviePath{1},'timeVector',obj.timeVector);
            MovieIJ.setImageDescrParameter(moviePath{1},'frameRate',obj.frameRate);
            MovieIJ.setImageDescrParameter(moviePath{1},'moviePath',obj.moviePath);   
            obj.movieId.show();
        end
        %% get movie dimensions
        function [pixelsperline,linesperframe,frames]=getDimensions(obj)
            % get movie dimensions from tiff file
            dims=obj.movieId.getDimensions';
            pixelsperline=dims(1);
            linesperframe=dims(2);
            frames=dims(4);
            if frames<2
                frames=dims(5);
            end
            if frames==0
                error('problem with dimensions assignment');
            end
        end
        %% show movie
        function show(obj)
            % show the movie corresponding to the object in ImageJ. 
            % Ny default called by the constructor
            % show(obj)
            % See also MovieIJ\hide
            
            obj.movieId.show(); 
            
        end
        
        %% hide movie
        function hide(obj)
            % hide the movie corresponding to the  object. 
            %When hidden no operation can be performed on it
            % show(obj)
            % See also MovieIJ\show
            
            obj.movieId.hide();            
        end
        
        %% minus function
        function obj1=minus(obj1,obj2)
           % operation of difference  (for now only defined on scalars)
           % obj1=minus(obj1,obj2)
           % newobj=obj-5;
           
           obj1.selectWin();
           if isnumeric(obj2)
               MIJ.run('Subtract...', ['value=' num2str(obj2) ' stack']);
           else
               error('operation only defined on scalars')
           end
        end
        
        %% get basic statistics
        function [min_,max_,mean_,median_,std_,statist]=getStatistics(obj)
             % get basic statistics about the movie
             %[min_,max_,mean_,median_,std_,statist]=getStatistics(obj)
             % statist is a java object that can be interrogated
             
             selectWin(obj)
             statist=obj.movieId.getStatistics(); 
             min_=statist.min;
             max_=statist.min;
             mean_=statist.mean;
             median_=statist.median;
             std_=statist.stdDev;
             
        end
        
        %% shift focus movie
        function selectWin(obj)
            % move the imageJ focus on the movie corresponding to the object
            % selectWin(obj)
            % See also MovieIJ\hide,  MovieIJ\show
             ij.IJ.selectWindow(obj.movieId.getTitle)
        end
        
        %% Perform motion correction in the movie
        function  [obj,xshifts,yshifts]=IJmotionCorrect(obj,maxShift,referenceImage,methodCorr,methodInterp)
           % motion correct using imageJ and the align slices in stack plugin,
           % based on openCV and downladable from 
            % https://sites.google.com/site/qingzongtseng/template-matching-ij-plugin            
            % [obj,xshifts,yshifts]=IJmotionCorrect(obj,maxShift,referenceImage,methodCorr,methodInterp)
            % maxShift: maximum number of pixel shift expected from movement
            % referenceImage: it can be an integer n (will set the n=th frame of the
            % movie as the reference frame) or a matrix (will use the
            % matrix as reference frame)
            % methodCorr, methodInterp: parameters from the plugin, Normally  methodCorr=5;
            % and methodInterp=1 work well           
            % xshifts,yshifts: shifts extimated by the algorithm                           
            ig=obj.movieId;
            switch nargin
                case 2
                    referenceImage=0;
                    methodCorr=5;
                    methodInterp=1;
                case 3
                    methodCorr=5;
                    methodInterp=1;
            end
            
            if nargin>1
                width=obj.pixelsPerLine;
                height=obj.linesPerFrame;
                winSizeX=width-2*maxShift;
                winSizeY=height-2*maxShift;
                xo=maxShift;
                yo=maxShift;
                xymotion=agMotionCorrectionImageJ(ig,winSizeX,winSizeY,xo,yo,methodCorr,methodInterp,maxShift,referenceImage);
            else
                xymotion=agMotionCorrectionImageJ(ig);
            end
            xshifts=xymotion(:,2);
            yshifts=xymotion(:,3);
            
            obj.movieId=ij.IJ.getImage();
%             ig.changes=true;
        end
        
        %% run a macro or plugin
        function run(obj,func,args)
            % run ImageJ macro or plugin
            % run(obj,func,args)
            % func: macro/plugin name 
            % args: macro/plugin arguments 
            % Example 
            % obj.run('Size...', 'width=5 height=10 depth=100');
           obj.selectWin()
           MIJ.run(func,args); 
        end
        
        %% resize the movie
        function obj=resize(obj,x,y,z)
            % resize the movies assigning the new dimensions z,y, and z.
            % A value of 0 leaves the dimension untouched
            % Examples
            % obj=resize(obj,0,0,100)
            % resample the movie in such a way that now there are 100
            % frames. it will interpolate in case of newz>oldz             
            if x==0
                x=obj.linesPerFrame;
            else
                obj.linesPerFrame=x;
            end
            if y==0
                y=obj.pixelsPerLine;
            else
                obj.pixelsPerLine=y;
            end
            if z==0
               z=obj.numFrames; 
            else
               obj.numFrames=z; 
            end
            obj.run('Size...', ['width=' num2str(y) ' height=' num2str(x) ' depth=' num2str(z) ' constrain average interpolation=Bilinear']);
        end
        
        %%
        function data=getMovie(obj)
           % get the movie in matlab matrix format
           % data=getMovie(obj)
           obj.selectWin();
           data=MIJ.getCurrentImage();
        end
        %%
        function saveToTiff(obj,filename)
            % save the object in a tiff file
            % saveToTiff(obj,filename)
            % if filename is not specified it will save with the original
            % filename
            if nargin<2
                ij.IJ.saveAsTiff(obj.movieId,obj.moviePath);
            else
                ij.IJ.saveAsTiff(obj.movieId,filename)
            end
        end
        
        %% project across the z-axis
        function varargout=zProject(obj,method)
            % projects across z dimension
            % img=zProject(obj,method)
            % method can be 'avg', 'median', 'std', 'min', 'max'
            % img:array
            selectWin(obj);
            switch method
                case 'median'
                    projector='Median';
                case 'mean'
                    projector='Average Intensity';
                case 'std'
                    projector='Stamdard Deviation';
                otherwise 
                    error('method not implemented');
            end
            MIJ.run('Z Project...', ['projection=[' projector ']']);            
            igproj=ij.IJ.getImage();
            varargout{1}=MIJ.getCurrentImage(); 
            if nargout>0
                igproj.close();
                selectWin(obj);
            end
            
        end
    end
    
end
%%

