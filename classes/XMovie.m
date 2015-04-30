classdef XMovie
%	This class implements a general movie class providing a wrapper for
%     ImageJ. In order to use this class you must have Fiji installed,
%     as well as the Miji wrapper properly installed and started. For some methods
%     like IJmotionCorrection and for neuronal extractions other packages
%     might be needed.
%
%
%     EXAMPLE OF USE
% 
%     see file agExampleScriptMovieIJ.m
% 
% author:Andrea Giovannucci

    properties (SetAccess = public)
        frameRate % if movies are collected in chunks and then joined this variable contain the name of each          
        movieId % retrieved from scanimage
        moviePath % path to the tif scanimage file
        Data % in matlab 3D matrix form
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
        function obj = XMovie(movie,frameRate,varargin)
            % constructor for the MovieIJ class
            % obj = MovieIJ(moviePath,frameRate)
            % moviePath: cell containing path to tif file. 
            % frameRate: frame rate of the collected movie (at the moment only equally spaced frames are supported)
            % 
            % EXAMPLES
            % movie=uipickfiles; % select movie to be processed
            % frameRate=.033; % movie frame rate
            % 
            % mm=MovieIJ(movie,frameRate); % create movie object
            % author:Andrea Giovannucci
            
            if nargin<2 % 
                frameRate=[];
            end            
                                    
            if iscell(movie) || ischar(movie)                
                obj.movieId=ij.IJ.openImage(movie);             
                if isempty(obj.movieId)
                    error('File not opened (possibly not found)')
                end
                obj.moviePath=movie;               
            elseif size(movie,1)>1 && size(movie,2)>1
                if ~isempty(find(movie<0))
                    movie=double(movie);
                    warning('converted to 32-bits')
                end
                MIJ.createImage(movie);                          
                obj.movieId=ij.IJ.getImage();
                windowname=datestr(now,'mmmm-dd-yyyy_HH:MM:SS.FFF'); % must be unique name for window
                obj.movieId.setTitle(windowname);
                obj.moviePath=[];                
            else
               error('Input must be a string, a cell or a matrix') 
            end
            [pixelsperline,linesperframe,nFrames]=getDimensions(obj);
            obj.movieId.show(); %the movie must be visible to perform operations on it
            obj.frameRate=frameRate;
            obj=obj.run('Select None','');
%             obj.numFrames=str2num(MovieIJ.getImageDescrParameter(moviePath{1},'frames'));
%             MovieIJ.setImageDescrParameter(moviePath{1},'timeVector',obj.timeVector);
%             MovieIJ.setImageDescrParameter(moviePath{1},'frameRate',obj.frameRate);
%             MovieIJ.setImageDescrParameter(moviePath{1},'moviePath',obj.moviePath);
            
        end
        %%
        function ppl=pixelsPerLine(obj)
             [ppl,~,~]=getDimensions(obj);
        end
        function lpp=linesPerFrame(obj)
             [~,lpp,~]=getDimensions(obj);
        end
        function nFrames=numFrames(obj)
             [~,~,nFrames]=getDimensions(obj);
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
        
        %% get basic statistics
        function [min_,max_,mean_,median_,std_,statist]=getStatistics(obj,singleFrame)
             % get basic statistics about the movie
             %[min_,max_,mean_,median_,std_,statist]=getStatistics(obj,singleFrame)
             % statist is a java object that can be interrogated
             % singleFrame=1 -> the method is called on a single image. in
             % this case median_ and statist  are valid, otherwise they
             % will be empty
             
             if nargin<2
                 singleFrame=0;
             end
             if singleFrame
                 selectWin(obj)
                 statist=obj.movieId.getStatistics(); 
                 min_=statist.min;
                 max_=statist.max;
                 mean_=statist.mean;
                 median_=statist.median;
                 std_=statist.stdDev;
             else
                MIJ.run('Clear Results') 
                obj.run('Statistics',''); 
                res=MIJ.getResultsTable(); 
                mean_=res(3);
                std_=res(4);
                min_=res(5);
                max_=res(6);                
                median_=[]; 
                statist=[];
             end
             
        end
        
        %% shift focus movie
        function selectWin(obj)
            % move the imageJ focus on the movie corresponding to the object
            % selectWin(obj)
            % See also MovieIJ\hide,  MovieIJ\show
             openedImages=MIJ.getListImages();
             count=0;             
             for img=1:numel(openedImages)
                 count=count+(strcmp(openedImages(img),obj.movieId.getTitle()));                                       
             end
             if count>1
                 error('Multiple windows with the same name open, the names should be unique')
             end
             ij.IJ.selectWindow(obj.movieId.getTitle)
             MIJ.run('Enhance Contrast', ['saturated=0.001'])             
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
            obj.selectWin();
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
            obj=obj.run('Select None','');
%             ig.changes=true;
        end
        
        %% run a macro or plugin
        function obj=run(obj,func,args)
            % run ImageJ macro or plugin
            % run(obj,func,args)
            % func: macro/plugin name 
            % args: macro/plugin arguments 
            % Example 
            % obj.run('Size...', 'width=5 height=10 depth=100');
           obj.selectWin()
           MIJ.run(func,args); 
           obj.movieId=ij.IJ.getImage();
        end
        
        %% resize the movie
        function obj=resize(obj,x,y,z)
            % resize the movies assigning the new dimensions z,y, and z.
            % A value of 0 leaves the dimension untouched
            % Examples
            % obj=resize(obj,0,0,100)
            % resample the movie in such a way that now there are 100
            % frames. it will interpolate in case of newz>oldz             
            obj.selectWin();
            if x==0
                x=obj.linesPerFrame;            
            end
            
            if y==0
                y=obj.pixelsPerLine;            
            end
            
            if z==0
               z=obj.numFrames;                
            end
            obj.frameRate=obj.frameRate*(double(obj.numFrames)/double(z));
            
            obj=obj.run('Size...', ['width=' num2str(y) ' height=' num2str(x) ' depth=' num2str(z) ' constrain average interpolation=Bilinear']);            
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
            obj.selectWin();
            if nargin<2
                ij.IJ.saveAsTiff(obj.movieId,obj.moviePath);
            else
                ij.IJ.saveAsTiff(obj.movieId,filename)
                obj.moviePath={filename};                
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
        
        %% translate movie frames
        function obj=translateFrame(obj,xtransl,ytransl,issubpixel)
            % translate frames by interpolating bicubically
            %  translateFrame(obj,xtransl,ytransl,issubpixel)
            % xtransl,ytransl translation values in pixel (can be fractional)
            % issubpixel=1 perform subpixel registration. DISCLAIMER:it
            % will change the fluorescence of pixels because of the
            % interpolation
            obj.selectWin();
            nframes=obj.numFrames;            
            if numel(xtransl)==nframes && numel(ytransl)==nframes
                for fr=1:nframes      
                    ij.IJ.setSlice(fr)
                    if issubpixel
                        MIJ.run('Translate...', ['x=' num2str(xtransl(fr)) ' y=' num2str(ytransl(fr))  ' interpolation=Bicubic slice']);
                    else
                        MIJ.run('Translate...', ['x=' num2str(round(xtransl(fr))) ' y=' num2str(round(ytransl(fr)))  ' interpolation=None slice']);
                    end
                end
            else
                error('function not implemented for whole stack')
            end
            obj.movieId=ij.IJ.getImage();
        end
        %% running minimum
        function obj=rollingMinimum(obj,x,y,z)
            % compute rolling Minimum in 3 D
            % obj=rollingMedian(obj,x,y,z)
            obj.selectWin();
            obj=obj.run('Minimum 3D...', ['x=' num2str(x) ' y=' num2str(y) ' z=' num2str(z)]);            
        end
        %% rolling median
        function obj=rollingMedian(obj,x,y,z)
            % compute rolling Median in 3 D
            % obj=rollingMedian(obj,x,y,z)
            obj.selectWin();
            obj=obj.run('Median 3D...', ['x=' num2str(x) ' y=' num2str(y) ' z=' num2str(z)]);            
        end
        %% function rolling percentile
        function obj=rollingPercentile(obj,quantileMin, windowLength)     
            % compute rolling percentile over window
            % obj=rollingPercentile(obj,percentile, window)     
            obj.selectWin();
            warning('*** For Andrea:remember to update adding the new function agRunning Quantile that works on half window per size!!')
            disp('...retrieving movie...')
            signal=single(obj.getMovie());
            disp('...computing rolling percentile...')
            numSamples=size(signal,3);
            funsl=@(is) quantile(signal(:,:,is-windowLength+1:is),quantileMin,3);
            quantSignal=arrayfun(funsl,windowLength:numSamples,'UniformOutput',false);
            quantSignal=cat(3,quantSignal{:});
            quantSignal=cat(3,repmat(median(quantSignal(:,:,1:windowLength-1),3),[1 1 windowLength-1]),quantSignal);
            obj=setMovie(obj,quantSignal);                                               
        end
        %%
        function obj=setMovie(obj,dat)
             % transfer movie to imageJ
             % obj=setMovie(obj,dat)
             % dat is a 2D or 3D matrix
             disp('...transferring movie to ImageJ...')
             title=obj.movieId.getTitle();
             obj.movieId.close();
             if ~isempty(find(dat<0))
                 dat=double(dat);
                 warning('converted to 32-bits')
             end             
             MIJ.createImage(dat);
             clear dat;
             obj.movieId=ij.IJ.getImage();
             obj.movieId.setTitle(title);
        end        
        %%
        function obj=calculatorPlus(i1,i2,operation,k1,k2,isdestructive)
            % implements operation in ImageCalculatorPlus of ImageJ
            % obj=calculatorPlus(obj,obj2,operation,k1,k2,isdestructive)
            % possible operations are
            % 'Subtract': i2 = (i1-i2) x k1 + k2
            % 'Add' : i2 = (i1+i2) x k1 + k2 
            % 'Divide' : i2 = (i1/i2) x k1 + k2
            % 'Multiply' : i2 = (i1*i2) x k1 + k2
            if nargin<3
                k1=1;
                k2=0;
                isdestructive=0;
            end
            if ~exist('isdestructive')
                isdestructive=0;
            end
            
            if (i1.movieId.getBitDepth+i2.movieId.getBitDepth)<64 
                ch=i1.movieId.changes;
                i1.setBitDepth(32);
                i1.movieId.changes=ch;
                ch=i2.movieId.changes;
                i2.setBitDepth(32);
                i2.movieId.changes=ch;
                warning('automatically converting to 32 bits')
            end
            
            switch operation
                case 'Subtract'
                     operStr=' operation=[Subtract: i2 = (i1-i2) x k1 + k2] ';                   
                case 'Divide'
                     operStr=' operation=[Divide: i2 = (i1/i2) x k1 + k2] ';                     
                case 'Add'
                     operStr=' operation=[Add: i2 = (i1+i2) x k1 + k2] ';
                case 'Multiply'
                     operStr=' operation=[Multiply: i2 = (i1*i2) x k1 + k2] ';                    
                case 'Scale'
                     operStr=' operation=[Scale: i2 = i1 x k1 + k2] ';                     
                otherwise
                    error('operation not implemented')
            end
            if isdestructive
                obj=i2.run('Calculator Plus',['i1=' char(i1.movieId.getTitle) ' i2=' char(i2.movieId.getTitle) ...
                   operStr ' k1=' num2str(k1) ' k2=' num2str(k2)]);
                   obj.movieId.setTitle(char(i2.movieId.getTitle()))
            else
                 obj=i2.run('Calculator Plus',['i1=' char(i1.movieId.getTitle) ' i2=' char(i2.movieId.getTitle) ...
                 operStr ' k1=' num2str(k1) ' k2=' num2str(k2) ' create 32-bit']);
                 obj.movieId.setTitle(['Result_' char(i2.movieId.getTitle()) '_' operation])
            end
            
            
        end
        %% macro math functions
        function obj=mathMacroFunction(obj,operation)
            % implements math functions on matrices
            % obj=mathFunctions(obj,val)
            % the function apply the operation pixel by pixel, less
            % effcient than the macth functions, but more flexible
            % EXAMPLEs
            % obj=obj.mathMacroFunction('abs(v)');
            % obj=obj.mathMacroFunction('sqrt(v)');
            % obj=obj.mathMacroFunction('square(v)');
            % obj=obj.mathMacroFunction('v^2');
            % See Also XMovie/mathFunctions     
            if obj.movieId.getBitDepth<32
                obj.setBitDepth(32);
                warning('bit depth set to 32 bits')
            end
            obj=obj.run('Macro...', ['code=v=' operation ' stack']);           
        end
        %% math Functions 
        function obj=mathFunctions(obj,function_name, value)
            % run fast math operations on matrices 
            % obj=mathFunctions(obj,function_name, args)
            % EXAMPLE 
            % obj=obj.mathFunctions('Add',20)
            % possible values are
            % 'Subtract','Multiply','Divide','Min','Max','Abs','Reciprocal',...
            % in practice evrything in the PImageJ Process->Math submenu
            if obj.movieId.getBitDepth<32
                obj.setBitDepth(32);
                warning('bit depth set to 32 bits')
            end
            if ~isempty(value)
                obj=obj.run([function_name '...'],['value=' num2str(value) ' stack']);
            else
                obj=obj.run(function_name,'stack');
            end
        end
         %% minus function
        function obj1=minus(obj1,obj2,isdestructive)
           % operation of difference 
           % obj1=minus(obj1,obj2,isdestructive)
           % obj1=obj1-obj2;
           % isdestructive=1 will overwrite obj1 movie
           % obj1=obj-5;
           % obj1=obj1-obj2;
           % 
           if nargin<3
               isdestructive=0;
           end          

           if isnumeric(obj2)
               obj1=mathFunctions(obj1,'Subtract', obj2);
           elseif isobject(obj2)
               obj1=calculatorPlus(obj2,obj1,'Subtract',-1,0,isdestructive);     
           else
               error('operation only defined on scalars and Xmovies')
           end
           
        end
        %% negative sign
        function obj=uminus(obj)
            if obj.movieId.getBitDepth<32  
                warning('automatically converting to 32 bits')
            end
            obj=mathFunctions(obj,'Multiply', -1);            
        end       
        %% plus
        function obj1=plus(obj1,obj2,isdestructive)
           % operation of sum 
           % obj1=minus(obj1,obj2,isdestructive)
           % isdestructive=1 will overwrite obj1 movie
           % obj1=obj+5;
           % obj1=obj1+obj2;
           if nargin<3
               isdestructive=0;
           end          
%            obj1.selectWin();
           if isnumeric(obj2)
               obj1=mathFunctions(obj1,'Add', obj2);              
           elseif isobject(obj2)
               obj1=calculatorPlus(obj2,obj1,'Add',1,0,isdestructive);     
           else
               error('operation only defined on scalars and Xmovies')
           end           
        end
        %% mtimes
         function obj1=mtimes(obj1,obj2,isdestructive)
           % operation of sum 
           % obj1=minus(obj1,obj2,isdestructive)
           % isdestructive=1 will overwrite obj1 movie
           % obj1=obj+5;
           % obj1=obj1+obj2;
           if nargin<3
               isdestructive=0;
           end          
           obj1.selectWin();
           if isnumeric(obj2)
               obj1=mathFunctions(obj1,'Multiply', obj2);              
           elseif isobject(obj2)
               obj1=calculatorPlus(obj2,obj1,'Multiply',1,0,isdestructive);     
           else
               error('operation only defined on scalars and Xmovies')
           end           
        end
        %% rdivide ./
        function obj1=rdivide(obj1,obj2,isdestructive)
            % operation of elementwise division
            % obj1=rdivide(obj1,obj2,isdestructive) or
            % obj1=obj1./obj2
            % isdestructive=1 will overwrite obj1 movie
            % obj1=obj+5;
            % obj1=obj1+obj2;
            if nargin<3
                isdestructive=0;
            end
%             obj1.selectWin();
            if isnumeric(obj2)
                obj1=mathFunctions(obj1,'Divide', obj2);
            elseif isobject(obj2)
                obj1=calculatorPlus(obj2,obj1,'Divide',1,0,isdestructive);
                obj1=mathFunctions(obj1,'Reciprocal', []);  
            else
                error('operation only defined on scalars and Xmovies')
            end
        end
        %% mrdivide used as the elementwise operation
        function obj1=mrdivide(obj1,obj2)
            obj1=rdivide(obj1,obj2);
        end
        
        %% setBitDepth
        function obj=setBitDepth(obj,nbits,scale)
            % converts among pixel bit depth
            % obj=setBitDepth(obj,nbits,scale)
            % obj=setBitDepth(obj,nbits)
            % currently supported is 32 16 8 bits 
            % if nargin>2 and scale == 1 it will rescale the movie to use the whole 
            % dynamic range
            % 
            
            if nargin<3 || ~scale % set the option to scaling or no scaling
                MIJ.run('Conversions...','noscale'); %
            else
                error('this option is very dangerous, not suggested')
                disp('if you really want to use comment the line above, at you rown risk')
                MIJ.run('Conversions...','scale'); %
            end
                        
            switch nbits                
                case 32
                    obj=obj.run('32-bit','');
                case 16
                    obj=obj.run('16-bit','');
                case 8
                    obj=obj.run('8-bit','');
                otherwise
                    error('bit conversion not possible')
            end
            obj=setConstrast(obj,0.001);
            
        end
        %% function close
        function close(obj,force)
            % close IMageJ window
            % close(obj,force)
            % force=1 will close even if the file was modified and not
            % saved 
            if nargin>1 && force % forse to overwrite in case file was changed
              obj.movieId.changes=0; 
            end
            obj.movieId.close();
        end
        %% duplicate movie
        function obj=duplicate(obj,title)
            % create a copy of the movie in ImageJ
            % obj=duplicate(obj,title)
            % title: window title and reference used to point to the window
           obj=obj.run('Duplicate...','duplicate');
           obj.movieId.setTitle(title);
        end
        %% compute DFF
        function [obj]=computeDFFMovie(obj,minValueF,downsampleFactor,rollingWindowTime,quantileMin)
            % compute the DFF of an input movie
            % This algorithm will create a baseline movie by taking the lowest 8 percentile value 
            % over a rolling window on a downsampled movie. Then it will upsample it back
            % Before dividing by F we will clip values lower than a given
            % minimum 
            % obj=computeDFFMovie(obj,minValueF,downsampleFactor,rollingWindowTime,quantileMin)
            % minValueF: it is the minimum value of F when performing the
            % /F operation
            % downsampleFactor (default 4): how many times to downsample
            % the movie when computing the pixelwise rolliong percentile
            % rollingWindowTime(s) (default 10 sec) lenght of the rolling
            % window in seconds
            % quantileMin (default .08, that is 8 percentile): quantile used for the baseline
            % over the rolling window
            % 
            if nargin < 5
                quantileMin=.08;
            end                    
            if nargin < 4 
                rollingWindowTime=10;
            end            
            if nargin < 3
                 downsampleFactor=4;
            end            
            
            
            if isempty(obj.frameRate)
                error('please set the framerate field to the appropriate value')
            end
            isdestructive=1;
            rollingWindowSize=rollingWindowTime/obj.frameRate;
            rollingWindowSize=round(rollingWindowSize/downsampleFactor);
            nframes=obj.numFrames;
            
            movBL=obj.duplicate('movBL');
            movBL=movBL.resize(0,0,round(nframes/downsampleFactor));
            movBL=movBL.rollingPercentile(quantileMin,rollingWindowSize);
            movBL=movBL.resize(0,0,nframes);
            
            obj=minus(obj,movBL,isdestructive);
            movBL=movBL.mathFunctions('Min',minValueF);
            obj=rdivide(obj,movBL,isdestructive);            
            movBL.close(1);
        end
        %% enhance contrast
        function obj=setConstrast(obj,saturated)
        	obj=obj.run('Enhance Contrast', ['saturated=' num2str(saturated)]);
        end
    end
    
end
%%

