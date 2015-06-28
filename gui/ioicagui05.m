function varargout = ioicagui05(varargin)
% IOICAGUI05 M-file for ioicagui05.fig
%      IOICAGUI05, by itself, creates a new IOICAGUI05 or raises the existing
%      singleton*.
%
%      H = IOICAGUI05 returns the handle to a new IOICAGUI05 or the handle to
%      the existing singleton*.
%
%      IOICAGUI05('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in IOICAGUI05.M with the given input arguments.
%
%      IOICAGUI05('Property','Value',...) creates a new IOICAGUI05 or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before ioicagui05_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to ioicagui05_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help ioicagui05

% Last Modified by GUIDE v2.5 27-Jan-2010 10:31:18

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ioicagui05_OpeningFcn, ...
                   'gui_OutputFcn',  @ioicagui05_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before ioicagui05 is made visible.
function ioicagui05_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to ioicagui05 (see VARARGIN)

% Choose default command line output for ioicagui05
handles.output = hObject;

folder=pwd;
handles.folder=folder;
set(handles.brightness,'string',1);
set(handles.spatial,'value',1);
handles.icamode='sp';
set(handles.ica_tanh,'value',1);
icamethodcell=cell(5,1);
icamethod{1}='pow3';
icamethod{2}='skew';
icamethod{3}='gauss';
icamethod{4}='tanh';
handles.icamethodcell=icamethod;
handles.icamethodnum=4;
handles.loadfolder=folder;
set(handles.hororder,'value',1);
handles.mainmask=zeros(128,128,2);
handles.maskcnt=1;
handles.totaldendmask=zeros(128,128);
handles.dendfrnummat=0;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes ioicagui05 wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = ioicagui05_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on slider movement.
function slider1_Callback(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider


brightness=(str2num(get(handles.brightness,'string')))/10;
frinit=1;

showmask=get(handles.show_mask,'value');

if showmask==0;
    mov=handles.mov;
end
if showmask==1;
    mov=handles.m;
end

frame_num = round(get(handles.slider1,'Value'));
set(handles.slider1,'value',frame_num);
set(handles.fr_num,'string',num2str(frame_num+frinit-1));
frame1=brightness*mov(:,:,frame_num);

axes(handles.axes1);
image(frame1)
colormap(gray)
guidata(hObject, handles);



% --- Executes during object creation, after setting all properties.
function slider1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function slider2_Callback(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider

pixx=handles.pixels_x;
pixy=handles.pixels_y;
frames=handles.frames;
mainmask=handles.mainmask;
siz=size(mainmask);
frame_num = round(get(handles.slider2,'Value'));
set(handles.slider2,'value',frame_num);
set(handles.mask_num,'string',num2str(frame_num));

tempfr(:,:)=mainmask(:,:,siz(3));
tempfr2(:,:)=mainmask(:,:,frame_num);

if frame_num~=siz(3);
    for i=1:pixx;
        for j=1:pixy;
            if tempfr(i,j)>=2;
                tempfr(i,j)=1;
            end
            if tempfr2(i,j)>=2;
                tempfr2(i,j)=1;
            end
        end
    end
    %frame2=tempfr(:,:)+2*tempfr2(:,:);
    frame2=tempfr2(:,:);
elseif frame_num==siz(3);
    frame2=mainmask(:,:,frame_num);
end
axes(handles.axes2);
imagesc(frame2)
colormap(gray)

mov=handles.mov;
mov=double(reshape(mov,pixx*pixy,frames));
tr=reshape(frame2,1,pixx*pixy)*mov;

axes(handles.axes3);
plot(tr)

guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function slider2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function slider3_Callback(hObject, eventdata, handles)
% hObject    handle to slider3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider

spcomps=handles.spcomps;
tpcomps=handles.tpcomps;
compnum=get(handles.slider3,'value');
set(handles.ica_num,'string',num2str(compnum));


axes(handles.axes4);
imagesc(spcomps(:,:,compnum))
colormap(jet)

x=tpcomps(compnum,:);
if mean(x)-min(x)>max(x)-mean(x);
    x=-x;
end

handles.currentspcomp=spcomps(:,:,compnum);

cla(handles.axes3);
axes(handles.axes3);
plot(x)
zoom on

handles.masktrace=x;
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function slider3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in load_movie.
function load_movie_Callback(hObject, eventdata, handles)
% hObject    handle to load_movie (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

ratiomovie=get(handles.ratio_movie,'value');
framejump=str2num(get(handles.frame_jump,'string'));
brightness=str2num(get(handles.brightness,'string'));
set(handles.show_mask,'Value',0);
folder=handles.loadfolder;
eval(['cd ' char(39) folder char(39) ';']);
  
frinit=str2num(get(handles.first_frame,'string'));
frfinal=str2num(get(handles.last_frame,'string'));

filterspec = {'*.mat','MAT files (.mat)';'*.tif','TIF files (.tif)'};
title = 'load';
folder=handles.loadfolder; 
temp_file=folder;

[filename, path, index] = uigetfile (filterspec, title, temp_file);
eval(['addpath' ' ' char(39) path char(39) ';']);
[~,name,ext]=fileparts(filename);

handles.folder=path;
handles.loadfolder=path;
total_filename=[folder filename];
size_filename=size(filename);
fname=filename(1:size_filename(2)-4);
handles.fname=fname;


if(isequal(ext,'.mat'))
    index=1;
    load(filename);
    numFramesOrig=size(mov,3);
    mov=mov(:,:,frinit:framejump:min(frfinal,numFramesOrig));
    try
        frames_size=size(mov,3);
        if ratiomovie==0;
            frames=max(frames_size)/framejump;
        else
            frames=max(frames_size);
        end
    catch
        if ratiomovie==0;
            frames=round((frfinal-frinit+1)/framejump);
        else
            frames=frfinal-frinit+1;
        end
    end
else
    try 
        movie_info=imfinfo(total_filename);
        frames_size=size(movie_info);
        if ratiomovie==0;
            frames=max(frames_size)/framejump;
        else
            frames=max(frames_size);
        end
    catch
        if ratiomovie==0;
            frames=round((frfinal-frinit+1)/framejump);
        else
            frames=frfinal-frinit+1;
        end
    end
end

if frfinal<frames;
    if ratiomovie==0;
        frames=round((frfinal-frinit+1)/framejump);
    else
        frames=frfinal-frinit+1;
    end
end
    

switch index 
    case 0
    % No action taken...the user has cancelled the Save   
    case {1,2}
    set(handles.movie_filename, 'String', fname);
    if ratiomovie==0;
        frame=1;
        while frame<=frames;
            try
               if(index==2)
                mov(:,:,frame) = imread(fname, 'tiff', (frinit+(frame-1)*framejump));
               end
               frame=frame+1;
            catch
                frame=frames+1;
            end
        end
    else
        for frame=1:frames;
            try
               [mov(:,:,:,frame),map] = imread(fname, 'tiff', frinit+(frame-1));
            catch
                frame=frfinal;
            end
        end
    end
    mov=squeeze(mov);
    if ratiomovie==1;
        m1=mov(:,:,1:2:end);
        m2=mov(:,:,2:2:end);
        if framejump==1;
            mov=m2./m1;
        else
            mov=m1./m2;
        end
        clear m1 m2
        movmax=max(max(max(mov)));
        mov=floor(5000/movmax)*mov;
    end
    frames=size(mov,3);
    set(handles.slider1,'max',frames);
    set(handles.slider1,'min',0);
    set(handles.slider1,'SliderStep',[1/frames 1/frames]);
    
    frame1 = brightness*mov(:,:,1);
    set(handles.slider1,'value',1);
    set(handles.fr_num,'string','1');
    cla(handles.axes1);
    axes(handles.axes1);
    imagesc(frame1)
    colormap(gray)
end

movmax=double(max(max(max(mov))))
brightness=floor(250000/movmax)/100
set(handles.brightness,'string',num2str(brightness));

[pixels_x,pixels_y,frames]=size(mov);
handles.dendmask=zeros(pixels_x,pixels_y);
handles.fname=fname;
handles.mov=mov;
handles.pixels_x=pixels_x;
handles.pixels_y=pixels_y;
handles.frames=frames;
%handles.deltaFcnt=0;

guidata(hObject, handles);


function movie_filename_Callback(hObject, eventdata, handles)
% hObject    handle to movie_filename (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of movie_filename as text
%        str2double(get(hObject,'String')) returns contents of movie_filename as a double


% --- Executes during object creation, after setting all properties.
function movie_filename_CreateFcn(hObject, eventdata, handles)
% hObject    handle to movie_filename (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function brightness_Callback(hObject, eventdata, handles)
% hObject    handle to brightness (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of brightness as text
%        str2double(get(hObject,'String')) returns contents of brightness as a double


% --- Executes during object creation, after setting all properties.
function brightness_CreateFcn(hObject, eventdata, handles)
% hObject    handle to brightness (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function first_frame_Callback(hObject, eventdata, handles)
% hObject    handle to first_frame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of first_frame as text
%        str2double(get(hObject,'String')) returns contents of first_frame as a double


% --- Executes during object creation, after setting all properties.
function first_frame_CreateFcn(hObject, eventdata, handles)
% hObject    handle to first_frame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function last_frame_Callback(hObject, eventdata, handles)
% hObject    handle to last_frame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of last_frame as text
%        str2double(get(hObject,'String')) returns contents of last_frame as a double


% --- Executes during object creation, after setting all properties.
function last_frame_CreateFcn(hObject, eventdata, handles)
% hObject    handle to last_frame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function frame_jump_Callback(hObject, eventdata, handles)
% hObject    handle to frame_jump (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of frame_jump as text
%        str2double(get(hObject,'String')) returns contents of frame_jump as a double


% --- Executes during object creation, after setting all properties.
function frame_jump_CreateFcn(hObject, eventdata, handles)
% hObject    handle to frame_jump (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in ratio_movie.
function ratio_movie_Callback(hObject, eventdata, handles)
% hObject    handle to ratio_movie (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ratio_movie


% --- Executes on button press in filter_movie.
function filter_movie_Callback(hObject, eventdata, handles)
% hObject    handle to filter_movie (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

th1=str2num(get(handles.LF,'string'));
th2=str2num(get(handles.HF,'string'));
mov=double(handles.mov);

movie_size=size(mov);
frames=movie_size(3);
movie_mean1fr=mean(mov,3);

movie_mean=zeros(handles.pixels_x,handles.pixels_y,handles.frames);

for i=1:frames;
    movie_mean(:,:,i)=movie_mean1fr(:,:);
end
movie_ac=(mov-movie_mean);
    

for i=1:movie_size(1);
    for j=1:movie_size(2);
        tr=squeeze(movie_ac(i,j,:));
        slopebegin=mean(tr(10:90));
        slopeend=mean(tr((end-90):(end-10)));
        slope=(slopeend-slopebegin)/(length(tr)-100);         
        trslope=slope*(-49:(movie_size(3)-50))+slopebegin;
        tr=tr-trslope';
        movie_ac(i,j,:)=tr;
    end
end


movie_ac=reshape(movie_ac,movie_size(1)*movie_size(2),movie_size(3));
movfft=fft(movie_ac');
if th1~=0;
    movfft(1:(th1+1),:)=0;
    movfft((end-th1+1):end,:)=0;
end
if th2~=0;
    y=size(movfft,1);
    movfft((round(y/2)-th2):(round(y/2)+th2),:)=0;
end
movie_ac=real(ifft(movfft));
movie_ac=reshape(movie_ac',movie_size(1),movie_size(2),movie_size(3));

frm=get(handles.slider1,'value');
frame1 = movie_ac(:,:,frm);

cla(handles.axes1);
axes(handles.axes1);
imagesc(frame1)
colormap(gray)

handles.mov=movie_ac;

guidata(hObject, handles);

function LF_Callback(hObject, eventdata, handles)
% hObject    handle to LF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of LF as text
%        str2double(get(hObject,'String')) returns contents of LF as a double


% --- Executes during object creation, after setting all properties.
function LF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to LF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function HF_Callback(hObject, eventdata, handles)
% hObject    handle to HF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of HF as text
%        str2double(get(hObject,'String')) returns contents of HF as a double


% --- Executes during object creation, after setting all properties.
function HF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to HF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in choose_region.
function choose_region_Callback(hObject, eventdata, handles)
% hObject    handle to choose_region (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

brightness=str2num(get(handles.brightness,'string'))/10;
showmask=get(handles.show_mask,'Value')
frames=handles.frames;
pixels_x=handles.pixels_x;
pixels_y=handles.pixels_y;
dendmask=zeros(pixels_x,pixels_y);

if showmask==0;
    mov=handles.mov;
end
if showmask==1;
    try
        mov=handles.m;
    catch
        mov=handles.mov;
    end
end

frnum=str2num(get(handles.fr_num,'string'));
fr=mov(:,:,frnum);
%axes(handles.axes1);
maskfig=figure;
image(brightness*fr); colormap(gray);
reg=roipoly;
close(maskfig)

mov_fltrd=handles.mov;

[reg_x,reg_y]=find(reg==1);
pixel_number=max(size(reg_x));

handles.regpixelnumber=length(reg_x);

xmax=max(reg_x);
ymax=max(reg_y);
xmin=min(reg_x);
ymin=min(reg_y);

frames=handles.frames;
movie_cut=zeros(xmax-xmin+1,ymax-ymin+1,frames);
movie_reg=zeros(pixels_x,pixels_y,frames);

xsize=xmax-xmin+1;
ysize=ymax-ymin+1;

for i=1:pixel_number;
    movie_reg(reg_x(i),reg_y(i),:)=mov_fltrd(reg_x(i),reg_y(i),:);
    dendmask(reg_x(i),reg_y(i))=1;
end

for i=xmin:xmax;
    for j=ymin:ymax;  
        movie_cut(i+1-xmin,j+1-ymin,:)=movie_reg(i,j,:);
    end
end

frame2=movie_reg(:,:,1);

cla(handles.axes2);
axes(handles.axes2);
imagesc(frame2)
colormap(gray)

set(handles.slider2,'value',0);
set(handles.mask_num,'string',num2str(0));

trmat=reshape(movie_cut,(xmax-xmin+1)*(ymax-ymin+1),frames);
tr=sum(trmat);

cla(handles.axes3);
axes(handles.axes3);
plot(tr)
zoom on

handles.masktrace=tr;
handles.xsize=xsize;
handles.ysize=ysize;
handles.xmax=xmax;
handles.xmin=xmin;
handles.ymax=ymax;
handles.ymin=ymin;
% handles.moviecut=movie_cut;
% handles.moviereg=movie_reg;
handles.dendmask=dendmask;
    
guidata(hObject, handles);

% --- Executes on button press in choose_whole.
function choose_whole_Callback(hObject, eventdata, handles)
% hObject    handle to choose_whole (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


frames=handles.frames;
pixels_x=handles.pixels_x;
pixels_y=handles.pixels_y;
dendmask=ones(pixels_x,pixels_y);
mov=handles.mov;

xmax=pixels_x;
ymax=pixels_y;
xmin=1;
ymin=1;
xsize=xmax;
ysize=ymax;

frame2=mov(:,:,1);

cla(handles.axes2);
axes(handles.axes2);
imagesc(frame2)
colormap(gray)

set(handles.slider2,'value',0);
set(handles.mask_num,'string',num2str(0));

trmat=reshape(mov,pixels_x*pixels_y,frames);
tr=mean(trmat);

cla(handles.axes3);
axes(handles.axes3);
plot(tr)
zoom on

handles.masktrace=tr;
handles.xsize=xsize;
handles.ysize=ysize;
handles.xmax=xmax;
handles.xmin=xmin;
handles.ymax=ymax;
handles.ymin=ymin;
handles.dendmask=dendmask;
    
guidata(hObject, handles);


% --- Executes on button press in show_mask.
function show_mask_Callback(hObject, eventdata, handles)
% hObject    handle to show_mask (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of show_mask


frinit=str2num(get(handles.first_frame,'string'));
showall=get(hObject,'Value');
masksize=size(handles.mainmask);

masknum=str2num(get(handles.mask_num,'string'));

if masknum==0;
    dendmask=handles.dendmask;
else
    dendmask(:,:)=handles.mainmask(:,:,masknum);
end
mov=double(handles.mov);
m_size=size(mov);
px=m_size(1);
py=m_size(2);
frames=m_size(3);
m=zeros(m_size(1),m_size(2),1,m_size(3));

b=max(max(max(mov)));

if showall==1;
        for i=1:px;
            for j=1:py;
                if dendmask(i,j)==0;
                    m(i,j,:)=mov(i,j,:);
                end
                if dendmask(i,j)~=0;
                    m(i,j,:)=b;
                end
            end
        end
        handles.m=m;
end
b=(str2num(get(handles.brightness,'string')))/10;

if showall==0;
    mov=handles.mov;
end
if showall==1;
    mov=handles.m;
end


frame_num = round(get(handles.slider1,'Value'));
set(handles.slider1,'value',frame_num);
set(handles.fr_num,'string',num2str(frame_num+frinit-1));

frame1=b*mov(:,:,frame_num);
axes(handles.axes1);
image(frame1)
colormap(gray)
guidata(hObject, handles);

% --- Executes on button press in do_ica.
function do_ica_Callback(hObject, eventdata, handles)
% hObject    handle to do_ica (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


oldpcomps=get(handles.use_old_comps,'value');

mov=handles.mov;
icamode=handles.icamode;
icamethod=handles.icamethodcell{handles.icamethodnum}
handles.icamethod=icamethod;

masknum=str2num(get(handles.mask_num,'string'));
region=handles.mainmask(:,:,masknum);
xpix=handles.pixels_x;
ypix=handles.pixels_y;

[x,y]=find(region~=0);
numpix=length(x);   
frames=handles.frames;
regica=zeros(numpix,frames);
for i=1:numpix;
    regica(i,:)=mov(x(i),y(i),:);
end

numcomps=str2num(get(handles.num_ica_comps,'string'));
svdcomps=str2num(get(handles.svd_comps,'string'));
excludecomp=str2num(get(handles.exclude_comps,'string'));
excludecomp=max(1,excludecomp)

if icamode=='tp';
    if oldpcomps==0;
        [icasig, A, W, E, D] = iofastica (regica, 'firstEig', excludecomp, 'lastEig', svdcomps, 'numOfIC', numcomps, 'g', icamethod);
    else
        [icasig, A, W, E, D] = iofastica (regica, 'firstEig', excludecomp, 'lastEig', svdcomps, 'numOfIC', numcomps, 'g', icamethod, 'pcae', handles.E, 'pcad', handles.D);
    end
    %numcomps=numcomps-excludecomp+1;    
    tpcomps=icasig;
    handles.tpcomps=tpcomps;
    spcomps=zeros(xpix,ypix,numcomps);
    for j=1:numcomps;
        for i=1:numpix;
            spcomps(x(i),y(i),j)=W(j,i);
        end
    end
    handles.spcomps=spcomps;
end

if icamode=='sp';
    
    if oldpcomps==0;
        [icasig, A, W, E, D] = iofastica (regica', 'firstEig', excludecomp, 'lastEig', svdcomps, 'numOfIC', numcomps, 'g', icamethod);
    else
        [icasig, A, W, E, D] = iofastica (regica', 'firstEig', excludecomp, 'lastEig', svdcomps, 'numOfIC', numcomps, 'g', icamethod, 'pcae', handles.E, 'pcad', handles.D);
    end
    %numcomps=numcomps-excludecomp+1; 
    tpcomps=W;  
    
    spcomps=zeros(xpix,ypix,numcomps);   
    for j=1:numcomps;
        for i=1:numpix;
            spcomps(x(i),y(i),j)=icasig(j,i);
        end
    end
    handles.spcomps=spcomps;
    handles.tpcomps=tpcomps;
end

axes(handles.axes4);
imagesc(spcomps(:,:,numcomps))
colormap(jet)

x=tpcomps(numcomps,:);
if mean(x)-min(x)>max(x)-mean(x);
    x=-x;
end

handles.currentspcomp=spcomps(:,:,numcomps);

cla(handles.axes3);
axes(handles.axes3);
plot(x)
zoom on

handles.masktrace=x;
handles.E=E;
handles.D=D;
set(handles.ica_num,'string',num2str(numcomps));
set(handles.slider3,'max',numcomps);
set(handles.slider3,'min',0);
set(handles.slider3,'SliderStep',[1/numcomps 1/numcomps]);
set(handles.slider3,'value',numcomps);

guidata(hObject, handles);

% --- Executes on button press in spatial.
function spatial_Callback(hObject, eventdata, handles)
% hObject    handle to spatial (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of spatial
a=get(hObject,'Value');
if a==1;
    set(handles.temporal,'Value',0);
    handles.icamode='sp';
end
guidata(hObject, handles);

% --- Executes on button press in temporal.
function temporal_Callback(hObject, eventdata, handles)
% hObject    handle to temporal (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of temporal

a=get(hObject,'Value');
if a==1;
    set(handles.spatial,'Value',0);
    handles.icamode='tp';
end
guidata(hObject, handles);


% --- Executes on button press in ica_kurt.
function ica_kurt_Callback(hObject, eventdata, handles)
% hObject    handle to ica_kurt (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ica_kurt
a=get(hObject,'Value');
if a==1;
    set(handles.ica_gauss,'Value',0);
    set(handles.ica_skew,'Value',0);
    set(handles.ica_tanh,'Value',0);
    handles.icamethodnum=1;
end
guidata(hObject, handles);

% --- Executes on button press in ica_skew.
function ica_skew_Callback(hObject, eventdata, handles)
% hObject    handle to ica_skew (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ica_skew
a=get(hObject,'Value');
if a==1;
    set(handles.ica_kurt,'Value',0);
    set(handles.ica_gauss,'Value',0);
    set(handles.ica_tanh,'Value',0);
    handles.icamethodnum=2;
end
guidata(hObject, handles);

% --- Executes on button press in ica_gauss.
function ica_gauss_Callback(hObject, eventdata, handles)
% hObject    handle to ica_gauss (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ica_gauss
a=get(hObject,'Value');
if a==1;
    set(handles.ica_kurt,'Value',0);
    set(handles.ica_skew,'Value',0);
    set(handles.ica_tanh,'Value',0);
    handles.icamethodnum=3;
end
guidata(hObject, handles);

% --- Executes on button press in ica_tanh.
function ica_tanh_Callback(hObject, eventdata, handles)
% hObject    handle to ica_tanh (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ica_tanh

a=get(hObject,'Value');
if a==1;
    set(handles.ica_kurt,'Value',0);
    set(handles.ica_skew,'Value',0);
    set(handles.ica_gauss,'Value',0);
    handles.icamethodnum=4;
end
guidata(hObject, handles);

function num_ica_comps_Callback(hObject, eventdata, handles)
% hObject    handle to num_ica_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of num_ica_comps as text
%        str2double(get(hObject,'String')) returns contents of num_ica_comps as a double


% --- Executes during object creation, after setting all properties.
function num_ica_comps_CreateFcn(hObject, eventdata, handles)
% hObject    handle to num_ica_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in use_old_comps.
function use_old_comps_Callback(hObject, eventdata, handles)
% hObject    handle to use_old_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of use_old_comps


% --- Executes on button press in save_ica.
function save_ica_Callback(hObject, eventdata, handles)
% hObject    handle to save_ica (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

icamethod=handles.icamethod;
m=get(handles.num_ica_comps,'string');
svdcomps=get(handles.svd_comps,'string');
icamode=num2str(handles.icamode);
spcomps=handles.spcomps;
tpcomps=handles.tpcomps;
filename=handles.fname;

try
    folder=handles.savefolder;
    eval(['cd ' char(39) folder char(39)]);
catch
end
totalfname=[filename '_' icamode '_' m 'comps' icamethod '_' svdcomps 'svdcomps'];
filename=totalfname;
temp_file=filename;

filterspec = {'*.mat','mat files (.mat)'};
title = 'Save';


[filename, path, index] = uiputfile (filterspec, title, temp_file);
totalfname=filename;
totalfname
handles.savefolder=path;
%filename=['m' filename];
if index == 0
    % No action taken...the user has cancelled the Save
    
elseif index == 1
    % Save the file as a .mat file    
     icamask=struct('spcomps',spcomps, 'tpcomps', tpcomps);
     save([path totalfname], 'icamask');
     handles.folder=path;
end

guidata(hObject, handles);

function svd_comps_Callback(hObject, eventdata, handles)
% hObject    handle to svd_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of svd_comps as text
%        str2double(get(hObject,'String')) returns contents of svd_comps as a double


% --- Executes during object creation, after setting all properties.
function svd_comps_CreateFcn(hObject, eventdata, handles)
% hObject    handle to svd_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function exclude_comps_Callback(hObject, eventdata, handles)
% hObject    handle to exclude_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of exclude_comps as text
%        str2double(get(hObject,'String')) returns contents of exclude_comps as a double


% --- Executes during object creation, after setting all properties.
function exclude_comps_CreateFcn(hObject, eventdata, handles)
% hObject    handle to exclude_comps (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in hororder.
function hororder_Callback(hObject, eventdata, handles)
% hObject    handle to hororder (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of hororder



function fr_num_Callback(hObject, eventdata, handles)
% hObject    handle to fr_num (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of fr_num as text
%        str2double(get(hObject,'String')) returns contents of fr_num as a double


% --- Executes during object creation, after setting all properties.
function fr_num_CreateFcn(hObject, eventdata, handles)
% hObject    handle to fr_num (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function mask_num_Callback(hObject, eventdata, handles)
% hObject    handle to mask_num (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of mask_num as text
%        str2double(get(hObject,'String')) returns contents of mask_num as a double


% --- Executes during object creation, after setting all properties.
function mask_num_CreateFcn(hObject, eventdata, handles)
% hObject    handle to mask_num (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function ica_num_Callback(hObject, eventdata, handles)
% hObject    handle to ica_num (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of ica_num as text
%        str2double(get(hObject,'String')) returns contents of ica_num as a double


% --- Executes during object creation, after setting all properties.
function ica_num_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ica_num (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in add_dendrite.
function add_dendrite_Callback(hObject, eventdata, handles)
% hObject    handle to add_dendrite (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

pixels_x=handles.pixels_x;
maskcnt=handles.maskcnt;
handles.posx=zeros(maskcnt+1,1);
handles.posy=zeros(maskcnt+1,1);

hororder=get(handles.hororder,'value');
pixels_y=handles.pixels_y;

if maskcnt~=1;
    tempmask=handles.mainmask;
end

if maskcnt==1;
    tempmask=zeros(pixels_x,pixels_y,maskcnt);
end

mainmask=zeros(pixels_x,pixels_y,maskcnt);

if maskcnt~=1;
    for i=1:maskcnt-1;
        mainmask(:,:,i)=tempmask(:,:,i);
    end
end

mainmask(:,:,maskcnt)=handles.dendmask(:,:);
totaldendmask=zeros(pixels_x,pixels_y);

for i=1:maskcnt;
    totaldendmask(:,:)=totaldendmask(:,:)+mainmask(:,:,i);    
end

for i=1:pixels_x;
    for j=1:pixels_y;
        if totaldendmask(i,j)>2;
            totaldendmask(i,j)=2;
        end
    end
end

totalmasktemp(:,:,1)=totaldendmask(:,:);

mainmask=cat(3,mainmask,totalmasktemp);

set(handles.slider2,'max',(maskcnt+1));
set(handles.slider2,'min',0);
set(handles.slider2,'SliderStep',[1/(maskcnt+1) 1/(maskcnt+1)]);

set(handles.mask_num,'String',num2str(maskcnt+1));
set(handles.slider2,'Value',(maskcnt+1));

cla(handles.axes2);
axes(handles.axes2);
imagesc(totaldendmask)
colormap(gray)

handles.maskcnt=maskcnt+1;
masksize=size(mainmask);
numdendrites=masksize(3)-1;

for i=1:numdendrites;
    dend(:,:)=mainmask(:,:,i);
    [x,y]=find(dend);
    posx(i)=(max(x)+min(x))/2;
    posy(i)=round((max(y)+min(y))/2);   
end

tempmask=(zeros(masksize(1),masksize(2)));
qq=0;   

for i=1:numdendrites;
    if hororder==0;
        [posval,posmin]=min(posx(i:numdendrites));
        if posmin==numdendrites-i+1&qq==0;
            lastdendrite=i;
            qq=qq+1;
        end
        aa=posy(posmin+i-1);
        posy(posmin+i-1)=posy(i);
        posy(i)=aa;
        posx(posmin+i-1)=posx(i);
        posx(i)=posval;
        tempmask(:,:)=mainmask(:,:,posmin+i-1);
        mainmask(:,:,posmin+i-1)=mainmask(:,:,i);
        mainmask(:,:,i)=tempmask(:,:);
    else
        [posval,posmin]=min(posy(i:numdendrites));
        if posmin==numdendrites-i+1&qq==0;
            lastdendrite=i;
            qq=qq+1;
        end
        aa=posx(posmin+i-1);
        posx(posmin+i-1)=posy(i);
        posx(i)=aa;
        posy(posmin+i-1)=posy(i);
        posy(i)=posval;
        tempmask(:,:)=mainmask(:,:,posmin+i-1);
        mainmask(:,:,posmin+i-1)=mainmask(:,:,i);
        mainmask(:,:,i)=tempmask(:,:);
    end
end

handles.posy=posy;
handles.posx=posx;
handles.mainmask=mainmask;
handles.lastdendrite=lastdendrite;

guidata(hObject, handles);


% --- Executes on button press in remove_dendrite.
function remove_dendrite_Callback(hObject, eventdata, handles)
% hObject    handle to remove_dendrite (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

mainmask=handles.mainmask;
a=size(mainmask);

tempmainmask=zeros(a(1),a(2),a(3)-1);
tempdendfrnummat=zeros(a(3)-2,1);
dendritenum=str2num(get(handles.mask_num,'string'));

tempmainmask(:,:,1:(dendritenum-1))=mainmask(:,:,1:(dendritenum-1));
tempmainmask(:,:,((dendritenum):(a(3)-2)))=mainmask(:,:,((dendritenum+1):(a(3)-1)));

totaldendmask=zeros(a(1),a(2));

for i=1:(a(3)-2);
    totaldendmask(:,:)=totaldendmask(:,:)+tempmainmask(:,:,i);
end

tempmainmask(:,:,(a(3)-1))=totaldendmask(:,:);
handles.mainmask=tempmainmask;


set(handles.slider2,'max',((a(3)-1)));
set(handles.slider2,'min',0);
set(handles.slider2,'SliderStep',[1/((a(3)-1)) 1/((a(3)-1))]);

set(handles.mask_num,'String',num2str((a(3)-1)));
set(handles.slider2,'Value',((a(3)-1)));

cla(handles.axes2);
axes(handles.axes2);
imagesc(totaldendmask)
colormap(gray)

handles.maskcnt=handles.maskcnt-1; 

guidata(hObject, handles);


% --- Executes on button press in clear_dendrites.
function clear_dendrites_Callback(hObject, eventdata, handles)
% hObject    handle to clear_dendrites (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

pixels_x=handles.pixels_x;
pixels_y=handles.pixels_y;

handles.totaldendmask=zeros(pixels_x,pixels_y);
handles.mainmask=zeros(pixels_x,pixels_y,1);
handles.maskcnt=1;
cla(handles.axes2);
guidata(hObject, handles);




%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************

function [Out1, Out2, Out3, E, D] = iofastica(mixedsig, varargin)


%FASTICA - Fast Independent Component Analysis
%
% FastICA for Matlab 7.x and 6.x
% Version 2.5, October 19 2005
% Copyright (c) Hugo G???vert, Jarmo Hurri, Jaakko S???rel???, and Aapo Hyv???rinen.
%
% FASTICA(mixedsig) estimates the independent components from given
% multidimensional signals. Each row of matrix mixedsig is one
% observed signal.  FASTICA uses Hyvarinen's fixed-point algorithm,
% see http://www.cis.hut.fi/projects/ica/fastica/. Output from the
% function depends on the number output arguments:
%
% [icasig] = FASTICA (mixedsig); the rows of icasig contain the
% estimated independent components.
%
% [icasig, A, W] = FASTICA (mixedsig); outputs the estimated separating
% matrix W and the corresponding mixing matrix A.
%
% [A, W] = FASTICA (mixedsig); gives only the estimated mixing matrix
% A and the separating matrix W.
%
% Some optional arguments induce other output formats, see below.
%
% A graphical user interface for FASTICA can be launched by the
% command FASTICAG
%
% FASTICA can be called with numerous optional arguments. Optional
% arguments are given in parameter pairs, so that first argument is
% the name of the parameter and the next argument is the value for
% that parameter. Optional parameter pairs can be given in any order.
%
% OPTIONAL PARAMETERS:
%
% Parameter name        Values and description
%
%======================================================================
% --Basic parameters in fixed-point algorithm:
%
% 'approach'            (string) The decorrelation approach used. Can be
%                       symmetric ('symm'), i.e. estimate all the
%                       independent component in parallel, or
%                       deflation ('defl'), i.e. estimate independent
%                       component one-by-one like in projection pursuit.
%                       Default is 'defl'.
%
% 'numOfIC'             (integer) Number of independent components to
%                       be estimated. Default equals the dimension of data.
%
%======================================================================
% --Choosing the nonlinearity:
%
% 'g'                   (string) Chooses the nonlinearity g used in 
%                       the fixed-point algorithm. Possible values:
%
%                       Value of 'g':      Nonlinearity used:
%                       'pow3' (default)   g(u)=u^3
%                       'tanh'             g(u)=tanh(a1*u)
%                       'gauss             g(u)=u*exp(-a2*u^2/2)
%                       'skew'             g(u)=u^2
% 
% 'finetune'		(string) Chooses the nonlinearity g used when 
%                       fine-tuning. In addition to same values
%                       as for 'g', the possible value 'finetune' is:
%                       'off'              fine-tuning is disabled.
%
% 'a1'                  (number) Parameter a1 used when g='tanh'.
%                       Default is 1.
% 'a2'                  (number) Parameter a2 used when g='gaus'.
%                       Default is 1.
%
% 'mu'			(number) Step size. Default is 1.
%                       If the value of mu is other than 1, then the
%                       program will use the stabilized version of the
%                       algorithm (see also parameter 'stabilization').
%
%
% 'stabilization'       (string) Values 'on' or 'off'. Default 'off'. 
%                       This parameter controls wether the program uses
%                       the stabilized version of the algorithm or
%                       not. If the stabilization is on, then the value
%                       of mu can momentarily be halved if the program
%                       senses that the algorithm is stuck between two
%                       points (this is called a stroke). Also if there
%                       is no convergence before half of the maximum
%                       number of iterations has been reached then mu
%                       will be halved for the rest of the rounds.
% 
%======================================================================
% --Controlling convergence:
%
% 'epsilon'             (number) Stopping criterion. Default is 0.0001.
%
% 'maxNumIterations'    (integer) Maximum number of iterations.
%                       Default is 1000.
%
% 'maxFinetune'         (integer) Maximum number of iterations in 
%                       fine-tuning. Default 100.
%
% 'sampleSize'          (number) [0 - 1] Percentage of samples used in
%                       one iteration. Samples are chosen in random.
%                       Default is 1 (all samples).
%
% 'initGuess'           (matrix) Initial guess for A. Default is random.
%                       You can now do a "one more" like this: 
%                       [ica, A, W] = fastica(mix, 'numOfIC',3);
%                       [ica2, A2, W2] = fastica(mix, 'initGuess', A, 'numOfIC', 4);
%
%======================================================================
% --Graphics and text output:
%
% 'verbose'             (string) Either 'on' or 'off'. Default is
%                       'on': report progress of algorithm in text format.
%
% 'displayMode'         (string) Plot running estimates of independent
%                       components: 'signals', 'basis', 'filters' or
%                       'off'. Default is 'off'.
%
% 'displayInterval'     Number of iterations between plots.
%                       Default is 1 (plot after every iteration).
%
%======================================================================
% --Controlling reduction of dimension and whitening:
%
% Reduction of dimension is controlled by 'firstEig' and 'lastEig', or
% alternatively by 'interactivePCA'. 
%
% 'firstEig'            (integer) This and 'lastEig' specify the range for
%                       eigenvalues that are retained, 'firstEig' is
%                       the index of largest eigenvalue to be
%                       retained. Default is 1.
%
% 'lastEig'             (integer) This is the index of the last (smallest)
%                       eigenvalue to be retained. Default equals the
%                       dimension of data.
%
% 'interactivePCA'      (string) Either 'on' or 'off'. When set 'on', the
%                       eigenvalues are shown to the user and the
%                       range can be specified interactively. Default
%                       is 'off'. Can also be set to 'gui'. Then the user
%                       can use the same GUI that's in FASTICAG.
%
% If you already know the eigenvalue decomposition of the covariance
% matrix, you can avoid computing it again by giving it with the
% following options:
%
% 'pcaE'                (matrix) Eigenvectors
% 'pcaD'                (matrix) Eigenvalues
%
% If you already know the whitened data, you can give it directly to
% the algorithm using the following options:
%
% 'whiteSig'            (matrix) Whitened signal
% 'whiteMat'            (matrix) Whitening matrix
% 'dewhiteMat'          (matrix) dewhitening matrix
%
% If values for all the 'whiteSig', 'whiteSig' and 'dewhiteMat' are
% supplied, they will be used in computing the ICA. PCA and whitening
% are not performed. Though 'mixedsig' is not used in the main
% algorithm it still must be entered - some values are still
% calculated from it.
%
% Performing preprocessing only is possible by the option:
%
% 'only'                (string) Compute only PCA i.e. reduction of
%                       dimension ('pca') or only PCA plus whitening
%                       ('white'). Default is 'all': do ICA estimation
%                       as well.  This option changes the output
%                       format accordingly. For example: 
%
%                       [whitesig, WM, DWM] = FASTICA(mixedsig, 
%                       'only', 'white') 
%                       returns the whitened signals, the whitening matrix
%                       (WM) and the dewhitening matrix (DWM). (See also
%                       WHITENV.) In FastICA the whitening matrix performs
%                       whitening and the reduction of dimension. Dewhitening
%                       matrix is the pseudoinverse of whitening matrix.
%                        
%                       [E, D] = FASTICA(mixedsig, 'only', 'pca') 
%                       returns the eigenvector (E) and diagonal 
%                       eigenvalue (D) matrices  containing the 
%                       selected subspaces. 
%
%======================================================================
% EXAMPLES
%
%       [icasig] = FASTICA (mixedsig, 'approach', 'symm', 'g', 'tanh');
%               Do ICA with tanh nonlinearity and in parallel (like
%               maximum likelihood estimation for supergaussian data).
%
%       [icasig] = FASTICA (mixedsig, 'lastEig', 10, 'numOfIC', 3);
%               Reduce dimension to 10, and estimate only 3
%               independent components.
%
%       [icasig] = FASTICA (mixedsig, 'verbose', 'off', 'displayMode', 'off');
%               Don't output convergence reports and don't plot
%               independent components.
%
%
% A graphical user interface for FASTICA can be launched by the
% command FASTICAG
%
%   See also FASTICAG

% @(#)$Id: fastica.m,v 1.14 2005/10/19 13:05:34 jarmo Exp $


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Check some basic requirements of the data
if nargin == 0,
  error ('You must supply the mixed data as input argument.');
end

if length (size (mixedsig)) > 2,
  error ('Input data can not have more than two dimensions.');
end

if any (any (isnan (mixedsig))),
  error ('Input data contains NaN''s.');
end

if ~isa (mixedsig, 'double')
  fprintf ('Warning: converting input data into regular (double) precision.\n');
  mixedsig = double (mixedsig);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Remove the mean and check the data

[mixedsig, mixedmean] = remmean(mixedsig);

[Dim, NumOfSampl] = size(mixedsig);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Default values for optional parameters

% All
verbose           = 'on';

% Default values for 'pcamat' parameters
firstEig          = 1;
lastEig           = Dim;
interactivePCA    = 'off';

% Default values for 'fpica' parameters
approach          = 'defl';
numOfIC           = Dim;
g                 = 'pow3';
finetune          = 'off';
a1                = 1;
a2                = 1;
myy               = 1;
stabilization     = 'off';
epsilon           = 0.0001;
maxNumIterations  = 10000;
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
maxFinetune       = 5;
initState         = 'rand';
guess             = 0;
sampleSize        = 1;
displayMode       = 'off';
displayInterval   = 1;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Parameters for fastICA - i.e. this file

b_verbose = 1;
jumpPCA = 0;
jumpWhitening = 0;
only = 3;
userNumOfIC = 0;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read the optional parameters

if (rem(length(varargin),2)==1)
  error('Optional parameters should always go by pairs');
else
  for i=1:2:(length(varargin)-1)
    if ~ischar (varargin{i}),
      error (['Unknown type of optional parameter name (parameter' ...
	      ' names must be strings).']);
    end
    % change the value of parameter
    switch lower (varargin{i})
     case 'stabilization'
      stabilization = lower (varargin{i+1});
     case 'maxfinetune'
      maxFinetune = varargin{i+1};
     case 'samplesize'
      sampleSize = varargin{i+1};
     case 'verbose'
      verbose = lower (varargin{i+1});
      % silence this program also
      if strcmp (verbose, 'off'), b_verbose = 0; end
     case 'firsteig'
      firstEig = varargin{i+1};
     case 'lasteig'
      lastEig = varargin{i+1};
     case 'interactivepca'
      interactivePCA = lower (varargin{i+1});
     case 'approach'
      approach = lower (varargin{i+1});
     case 'numofic'
      numOfIC = varargin{i+1};
      % User has supplied new value for numOfIC.
      % We'll use this information later on...
      userNumOfIC = 1;
     case 'g'
      g = lower (varargin{i+1});
     case 'finetune'
      finetune = lower (varargin{i+1});
     case 'a1'
      a1 = varargin{i+1};
     case 'a2'
      a2 = varargin{i+1};
     case {'mu', 'myy'}
      myy = varargin{i+1};
     case 'epsilon'
      epsilon = varargin{i+1};
     case 'maxnumiterations'
      maxNumIterations = varargin{i+1};
     case 'initguess'
      % no use setting 'guess' if the 'initState' is not set
      initState = 'guess';
      guess = varargin{i+1};
     case 'displaymode'
      displayMode = lower (varargin{i+1});
     case 'displayinterval'
      displayInterval = varargin{i+1};
     case 'pcae'
      % calculate if there are enought parameters to skip PCA
      jumpPCA = jumpPCA + 1;
      E = varargin{i+1};
     case 'pcad'
      % calculate if there are enought parameters to skip PCA
      jumpPCA = jumpPCA + 1;
      D = varargin{i+1};
     case 'whitesig'
      % calculate if there are enought parameters to skip PCA and whitening
      jumpWhitening = jumpWhitening + 1;
      whitesig = varargin{i+1};
     case 'whitemat'
      % calculate if there are enought parameters to skip PCA and whitening
      jumpWhitening = jumpWhitening + 1;
      whiteningMatrix = varargin{i+1};
     case 'dewhitemat'
      % calculate if there are enought parameters to skip PCA and whitening
      jumpWhitening = jumpWhitening + 1;
      dewhiteningMatrix = varargin{i+1};
     case 'only'
      % if the user only wants to calculate PCA or...
      switch lower (varargin{i+1})
       case 'pca'
	only = 1;
       case 'white'
	only = 2;
       case 'all'
	only = 3;
      end
      
     otherwise
      % Hmmm, something wrong with the parameter string
      error(['Unrecognized parameter: ''' varargin{i} '''']);
    end;
  end;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% print information about data
if b_verbose
  fprintf('Number of signals: %d\n', Dim);
  fprintf('Number of samples: %d\n', NumOfSampl);
end

% Check if the data has been entered the wrong way,
% but warn only... it may be on purpose

if Dim > NumOfSampl
  if b_verbose
    fprintf('Warning: ');
    fprintf('The signal matrix may be oriented in the wrong way.\n');
    fprintf('In that case transpose the matrix.\n\n');
  end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculating PCA

% We need the results of PCA for whitening, but if we don't
% need to do whitening... then we dont need PCA...
if jumpWhitening == 3
  if b_verbose,
    fprintf ('Whitened signal and corresponding matrises supplied.\n');
    fprintf ('PCA calculations not needed.\n');
  end;
else
  
  % OK, so first we need to calculate PCA
  % Check to see if we already have the PCA data
  if jumpPCA == 2,
    if b_verbose,
      fprintf ('Values for PCA calculations supplied.\n');
      fprintf ('PCA calculations not needed.\n');
    end;
  else
    % display notice if the user entered one, but not both, of E and D.
    if (jumpPCA > 0) & (b_verbose),
      fprintf ('You must suply all of these in order to jump PCA:\n');
      fprintf ('''pcaE'', ''pcaD''.\n');
    end;
    
    % Calculate PCA
    [E, D, Vio]=iopcamat(mixedsig, firstEig, lastEig, interactivePCA, verbose);
  end
end

% skip the rest if user only wanted PCA
if only > 1
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Whitening the data
  
  % Check to see if the whitening is needed...
  if jumpWhitening == 3,
    if b_verbose,
      fprintf ('Whitening not needed.\n');
    end;
  else
    
    % Whitening is needed
    % display notice if the user entered some of the whitening info, but not all.
    if (jumpWhitening > 0) & (b_verbose),
      fprintf ('You must suply all of these in order to jump whitening:\n');
      fprintf ('''whiteSig'', ''whiteMat'', ''dewhiteMat''.\n');
    end;
    
    % Calculate the whitening
    [whitesig, whiteningMatrix, dewhiteningMatrix] = whitenv ...
						     (mixedsig, E, D, verbose);
  end
  
end % if only > 1

% skip the rest if user only wanted PCA and whitening
if only > 2
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Calculating the ICA
  
  % Check some parameters
  % The dimension of the data may have been reduced during PCA calculations.
  % The original dimension is calculated from the data by default, and the
  % number of IC is by default set to equal that dimension.
  
  Dim = size(whitesig, 1);
  
  % The number of IC's must be less or equal to the dimension of data
  if numOfIC > Dim
    numOfIC = Dim;
    % Show warning only if verbose = 'on' and user supplied a value for 'numOfIC'
    if (b_verbose & userNumOfIC)
      fprintf('Warning: estimating only %d independent components\n', numOfIC);
      fprintf('(Can''t estimate more independent components than dimension of data)\n');
    end
  end
  
  % Calculate the ICA with fixed point algorithm.
  [A, W] = iofpica2 (whitesig,  whiteningMatrix, dewhiteningMatrix, approach, ...
		  numOfIC, g, finetune, a1, a2, myy, stabilization, epsilon, ...
		  maxNumIterations, maxFinetune, initState, guess, sampleSize, ...
		  displayMode, displayInterval, verbose);
  
  % Check for valid return
  if ~isempty(W)
    % Add the mean back in.
    if b_verbose
      fprintf('Adding the mean back to the data.\n');
    end
    icasig = W * mixedsig + (W * mixedmean) * ones(1, NumOfSampl);
    %icasig = W * mixedsig;
    if b_verbose & ...
	  (max(abs(W * mixedmean)) > 1e-9) & ...
	  (strcmp(displayMode,'signals') | strcmp(displayMode,'on'))
      fprintf('Note that the plots don''t have the mean added.\n');
    end
  else
    icasig = [];
  end

end % if only > 2

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The output depends on the number of output parameters
% and the 'only' parameter.

if only == 1    % only PCA
  Out1 = E;
  Out2 = D;
elseif only == 2  % only PCA & whitening
  if nargout == 2
    Out1 = whiteningMatrix;
    Out2 = dewhiteningMatrix;
  else
    Out1 = whitesig;
    Out2 = whiteningMatrix;
    Out3 = dewhiteningMatrix;
  end
else      % ICA
  if nargout == 2
    Out1 = A;
    Out2 = W;
  else
    Out1 = icasig;
    Out2 = A;
    Out3 = W;
  end
end



%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************


function [A, W] = iofpica2(X, whiteningMatrix, dewhiteningMatrix, approach, ...
			numOfIC, g, finetune, a1, a2, myy, stabilization, ...
			epsilon, maxNumIterations, maxFinetune, initState, ...
			guess, sampleSize, displayMode, displayInterval, ...
			s_verbose);
%FPICA - Fixed point ICA. Main algorithm of FASTICA.
%
% [A, W] = fpica(whitesig, whiteningMatrix, dewhiteningMatrix, approach,
%        numOfIC, g, finetune, a1, a2, mu, stabilization, epsilon, 
%        maxNumIterations, maxFinetune, initState, guess, sampleSize,
%        displayMode, displayInterval, verbose);
% 
% Perform independent component analysis using Hyvarinen's fixed point
% algorithm. Outputs an estimate of the mixing matrix A and its inverse W.
%
% whitesig                              :the whitened data as row vectors
% whiteningMatrix                       :can be obtained with function whitenv
% dewhiteningMatrix                     :can be obtained with function whitenv
% approach      [ 'symm' | 'defl' ]     :the approach used (deflation or symmetric)
% numOfIC       [ 0 - Dim of whitesig ] :number of independent components estimated
% g             [ 'pow3' | 'tanh' |     :the nonlinearity used
%                 'gaus' | 'skew' ]     
% finetune      [same as g + 'off']     :the nonlinearity used in finetuning.
% a1                                    :parameter for tuning 'tanh'
% a2                                    :parameter for tuning 'gaus'
% mu                                    :step size in stabilized algorithm
% stabilization [ 'on' | 'off' ]        :if mu < 1 then automatically on
% epsilon                               :stopping criterion
% maxNumIterations                      :maximum number of iterations 
% maxFinetune                           :maximum number of iteretions for finetuning
% initState     [ 'rand' | 'guess' ]    :initial guess or random initial state. See below
% guess                                 :initial guess for A. Ignored if initState = 'rand'
% sampleSize    [ 0 - 1 ]               :percentage of the samples used in one iteration
% displayMode   [ 'signals' | 'basis' | :plot running estimate
%                 'filters' | 'off' ]
% displayInterval                       :number of iterations we take between plots
% verbose       [ 'on' | 'off' ]        :report progress in text format
%
% EXAMPLE
%       [E, D] = pcamat(vectors);
%       [nv, wm, dwm] = whitenv(vectors, E, D);
%       [A, W] = fpica(nv, wm, dwm);
%
%
% This function is needed by FASTICA and FASTICAG
%
%   See also FASTICA, FASTICAG, WHITENV, PCAMAT

% @(#)$Id: fpica.m,v 1.7 2005/06/16 12:52:55 jarmo Exp $

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Global variable for stopping the ICA calculations from the GUI
global g_FastICA_interrupt;
if isempty(g_FastICA_interrupt)
  clear global g_FastICA_interrupt;
  interruptible = 0;
else
  interruptible = 1;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Default values


if nargin < 3, error('Not enough arguments!'); end
[vectorSize, numSamples] = size(X);
if nargin < 20, s_verbose = 'on'; end
if nargin < 10, displayInterval = 1; end
if nargin < 18, displayMode = 'on'; end
if nargin < 17, sampleSize = 1; end
if nargin < 16, guess = 1; end
if nargin < 15, initState = 'rand'; end
if nargin < 14, maxFinetune = 100; end
if nargin < 13, maxNumIterations = 10000; end
if nargin < 12, epsilon = 0.0001; end
if nargin < 11, stabilization = 'on'; end
if nargin < 10, myy = 1; end
if nargin < 9, a2 = 1; end
if nargin < 8, a1 = 1; end
if nargin < 7, finetune = 'off'; end
if nargin < 6, g = 'pow3'; end
if nargin < 5, numOfIC = vectorSize; end     % vectorSize = Dim
if nargin < 4, approach = 'defl'; end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the data

if ~isreal(X)
  error('Input has an imaginary part.');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the value for verbose

switch lower(s_verbose)
 case 'on'
  b_verbose = 1;
 case 'off'
  b_verbose = 0;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''verbose''\n', s_verbose));
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the value for approach

switch lower(approach)
 case 'symm'
  approachMode = 1;
 case 'defl'
  approachMode = 2;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''approach''\n', approach));
end
if b_verbose, fprintf('Used approach [ %s ].\n', approach); end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the value for numOfIC

if vectorSize < numOfIC
  error('Must have numOfIC <= Dimension!');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the sampleSize
if sampleSize > 1
  sampleSize = 1;
  if b_verbose
    fprintf('Warning: Setting ''sampleSize'' to 1.\n');
  end  
elseif sampleSize < 1
  if (sampleSize * numSamples) < 1000
    sampleSize = min(1000/numSamples, 1);
    if b_verbose
      fprintf('Warning: Setting ''sampleSize'' to %0.3f (%d samples).\n', ...
	      sampleSize, floor(sampleSize * numSamples));
    end  
  end
end
if b_verbose
  if  b_verbose & (sampleSize < 1)
    fprintf('Using about %0.0f%% of the samples in random order in every step.\n',sampleSize*100);
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the value for nonlinearity.

switch lower(g)
 case 'pow3'
  gOrig = 10;
 case 'tanh'
  gOrig = 20;
 case {'gaus', 'gauss'}
  gOrig = 30;
 case 'skew'
  gOrig = 40;
 case 'kurtosis'
  gOrig = 50;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''g''\n', g));
end
if sampleSize ~= 1
  gOrig = gOrig + 2;
end
if myy ~= 1
  gOrig = gOrig + 1;
end

if b_verbose,
  fprintf('Used nonlinearity [ %s ].\n', g);
end

finetuningEnabled = 1;
switch lower(finetune)
 case 'pow3'
  gFine = 10 + 1;
 case 'tanh'
  gFine = 20 + 1;
 case {'gaus', 'gauss'}
  gFine = 30 + 1;
 case 'skew'
  gFine = 40 + 1;
 case 'kurtosis'
  gFine = 50 + 1;
 case 'off'
  if myy ~= 1
    gFine = gOrig;
  else 
    gFine = gOrig + 1;
  end
  finetuningEnabled = 0;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''finetune''\n', ...
		finetune));
end

if b_verbose & finetuningEnabled
  fprintf('Finetuning enabled (nonlinearity: [ %s ]).\n', finetune);
end

switch lower(stabilization)
 case 'on'
  stabilizationEnabled = 1;
 case 'off'
  if myy ~= 1
    stabilizationEnabled = 1;
  else
    stabilizationEnabled = 0;
  end
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''stabilization''\n', ...
		stabilization)); 
end

if b_verbose & stabilizationEnabled
  fprintf('Using stabilized algorithm.\n');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Some other parameters
myyOrig = myy;
% When we start fine-tuning we'll set myy = myyK * myy
myyK = 0.01;
% How many times do we try for convergence until we give up.
failureLimit = 5;


usedNlinearity = gOrig;
stroke = 0;
notFine = 1;
long = 0;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the value for initial state.

switch lower(initState)
 case 'rand'
  initialStateMode = 0;
 case 'guess'
  if size(guess,1) ~= size(whiteningMatrix,2)
    initialStateMode = 0;
    if b_verbosemyyK
      fprintf('Warning: size of initial guess is incorrect. Using random initial guess.\n');
    end
  else
    initialStateMode = 1;
    if size(guess,2) < numOfIC
      if b_verbose
	fprintf('Warning: initial guess only for first %d components. Using random initial guess for others.\n', size(guess,2)); 
      end
      guess(:, size(guess, 2) + 1:numOfIC) = ...
					     rand(vectorSize,numOfIC-size(guess,2))-.5;
    elseif size(guess,2)>numOfIC
      guess=guess(:,1:numOfIC);
      fprintf('Warning: Initial guess too large. The excess column are dropped.\n');
    end
    if b_verbose, fprintf('Using initial guess.\n'); end
  end
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''initState''\n', initState));
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Checking the value for display mode.

switch lower(displayMode)
 case {'off', 'none'}
  usedDisplay = 0;
 case {'on', 'signals'}
  usedDisplay = 1;
  if (b_verbose & (numSamples > 10000))
    fprintf('Warning: Data vectors are very long. Plotting may take long time.\n');
  end
  if (b_verbose & (numOfIC > 25))
    fprintf('Warning: There are too many signals to plot. Plot may not look good.\n');
  end
 case 'basis'
  usedDisplay = 2;
  if (b_verbose & (numOfIC > 25))
    fprintf('Warning: There are too many signals to plot. Plot may not look good.\n');
  end
 case 'filters'
  usedDisplay = 3;
  if (b_verbose & (vectorSize > 25))
    fprintf('Warning: There are too many signals to plot. Plot may not look good.\n');
  end
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''displayMode''\n', displayMode));
end

% The displayInterval can't be less than 1...
if displayInterval < 1
  displayInterval = 1;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if b_verbose, fprintf('Starting ICA calculation...\n'); end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SYMMETRIC APPROACH
if approachMode == 2,

  % set some parameters more...
  usedNlinearity = gOrig;
  stroke = 0;
  notFine = 1;
  long = 0;
  
  A = zeros(vectorSize, numOfIC);  % Dewhitened basis vectors.
  if initialStateMode == 0
    % Take random orthonormal initial vectors.
    B = orth (randn (vectorSize, numOfIC));
  elseif initialStateMode == 1
    % Use the given initial vector as the initial state
    B = whiteningMatrix * guess;
  end
  
  BOld = zeros(size(B));
  BOld2 = zeros(size(B));

  
  % This is the actual fixed-point iteration loop.
  for round = 1:maxNumIterations + 1,
    if round == maxNumIterations + 1,
      fprintf('No convergence after %d steps\n', maxNumIterations);
      fprintf('Note that the plots are probably wrong.\n');
      if ~isempty(B)
	% Symmetric orthogonalization.
	B = B * real(inv(B' * B)^(1/2));

	W = B' * whiteningMatrix;
	A = dewhiteningMatrix * B;

      else
	W = [];
	A = [];

      end
      return;
    end
    
    if (interruptible & g_FastICA_interrupt)
      if b_verbose 
        fprintf('\n\nCalculation interrupted by the user\n');
      end
      if ~isempty(B)
	W = B' * whiteningMatrix;
	A = dewhiteningMatrix * B;

      else
	W = [];
	A = [];

      end
      return;
    end
    
    
    % Symmetric orthogonalization.
    B = B * real(inv(B' * B)^(1/2));

    
    % Test for termination condition. Note that we consider opposite
    % directions here as well.
    minAbsCos = min(abs(diag(B' * BOld)));
    minAbsCos2 = min(abs(diag(B' * BOld2)));

    
    if (1 - minAbsCos < epsilon)
      if finetuningEnabled & notFine
        if b_verbose, fprintf('Initial convergence, fine-tuning: \n'); end;
        notFine = 0;
        usedNlinearity = gFine;
        myy = myyK * myyOrig;
        BOld = zeros(size(B));
        BOld2 = zeros(size(B));

	
      else
        if b_verbose, fprintf('Convergence after %d steps\n', round); end
	
        % Calculate the de-whitened vectors.
        A = dewhiteningMatrix * B;

        break;
      end
    elseif stabilizationEnabled
      if (~stroke) & (1 - minAbsCos2 < epsilon) && (1 - minAbsCoss2 < epsilon)
	if b_verbose, fprintf('Stroke!\n'); end;
	stroke = myy;
	myy = .5*myy;
	if mod(usedNlinearity,2) == 0
	  usedNlinearity = usedNlinearity + 1;
	end
      elseif stroke
	myy = stroke;
	stroke = 0;
	if (myy == 1) & (mod(usedNlinearity,2) ~= 0)
	  usedNlinearity = usedNlinearity - 1;
	end
      elseif (~long) & (round>maxNumIterations/2)
	if b_verbose, fprintf('Taking long (reducing step size)\n'); end;
	long = 1;
	myy = .5*myy;
	if mod(usedNlinearity,2) == 0
	  usedNlinearity = usedNlinearity + 1;
	end
      end
    end
    
    BOld2 = BOld;
    BOld = B;

    
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Show the progress...
    if b_verbose
      if round == 1
        fprintf('Step no. %d\n', round);
      else
        fprintf('Step no. %d, change in value of estimate: %.3g \n', round, 1 - minAbsCos);
      end
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Also plot the current state...
    switch usedDisplay
     case 1
      if rem(round, displayInterval) == 0,
	% There was and may still be other displaymodes...
	% 1D signals
	icaplot('dispsig',(X'*B)');
	drawnow;
      end
     case 2
      if rem(round, displayInterval) == 0,
	% ... and now there are :-)
	% 1D basis
	A = dewhiteningMatrix * B;
    AA = dewhiteningMatrix2 * BB;
	icaplot('dispsig',A');
	drawnow;
      end
     case 3
      if rem(round, displayInterval) == 0,
	% ... and now there are :-)
	% 1D filters
	W = B' * whiteningMatrix;
%     W = mu*W + (1-mu)*WW;
	icaplot('dispsig',W);
	drawnow;
      end
     otherwise
    end
    
    switch usedNlinearity
      % pow3
     case 10
      B = (X * (( X' * B) .^ 3)) / numSamples - 3 * B;

     case 11
      % optimoitu - epsilonin kokoisia eroja
      % t???m??? on optimoitu koodi, katso vanha koodi esim.
      % aikaisemmista versioista kuten 2.0 beta3
      Y = X' * B;
      Gpow3 = Y .^ 3;
      Beta = sum(Y .* Gpow3);
      D = diag(1 ./ (Beta - 3 * numSamples));
      B = B + myy * B * (Y' * Gpow3 - diag(Beta)) * D;
     case 12
      Xsub=X(:, getSamples(numSamples, sampleSize));
      B = (Xsub * (( Xsub' * B) .^ 3)) / size(Xsub,2) - 3 * B;
     case 13
      % Optimoitu
      Ysub=X(:, getSamples(numSamples, sampleSize))' * B;
      Gpow3 = Ysub .^ 3;
      Beta = sum(Ysub .* Gpow3);
      D = diag(1 ./ (Beta - 3 * size(Ysub', 2)));
      B = B + myy * B * (Ysub' * Gpow3 - diag(Beta)) * D;
      
      % tanh
     case 20
      hypTan = tanh(a1 * X' * B);
      B = X * hypTan / numSamples - ...
	  ones(size(B,1),1) * sum(1 - hypTan .^ 2) .* B / numSamples * ...
	  a1;
     case 21
      % optimoitu - epsilonin kokoisia 
      Y = X' * B;
      hypTan = tanh(a1 * Y);
      Beta = sum(Y .* hypTan);
      D = diag(1 ./ (Beta - a1 * sum(1 - hypTan .^ 2)));
      B = B + myy * B * (Y' * hypTan - diag(Beta)) * D;
     case 22
      Xsub=X(:, getSamples(numSamples, sampleSize));
      hypTan = tanh(a1 * Xsub' * B);
      B = Xsub * hypTan / size(Xsub, 2) - ...
	  ones(size(B,1),1) * sum(1 - hypTan .^ 2) .* B / size(Xsub, 2) * a1;
     case 23
      % Optimoitu
      Y = X(:, getSamples(numSamples, sampleSize))' * B;
      hypTan = tanh(a1 * Y);
      Beta = sum(Y .* hypTan);
      D = diag(1 ./ (Beta - a1 * sum(1 - hypTan .^ 2)));
      B = B + myy * B * (Y' * hypTan - diag(Beta)) * D;
      
      % gauss
     case 30
      U = X' * B;
      Usquared=U .^ 2;
      ex = exp(-a2 * Usquared / 2);
      gauss =  U .* ex;
      dGauss = (1 - a2 * Usquared) .*ex;
      B = X * gauss / numSamples - ...
	  ones(size(B,1),1) * sum(dGauss)...
	  .* B / numSamples ;
     case 31
      % optimoitu
      Y = X' * B;
      ex = exp(-a2 * (Y .^ 2) / 2);
      gauss = Y .* ex;
      Beta = sum(Y .* gauss);
      D = diag(1 ./ (Beta - sum((1 - a2 * (Y .^ 2)) .* ex)));
      B = B + myy * B * (Y' * gauss - diag(Beta)) * D;
     case 32
      Xsub=X(:, getSamples(numSamples, sampleSize));
      U = Xsub' * B;
      Usquared=U .^ 2;
      ex = exp(-a2 * Usquared / 2);
      gauss =  U .* ex;
      dGauss = (1 - a2 * Usquared) .*ex;
      B = Xsub * gauss / size(Xsub,2) - ...
	  ones(size(B,1),1) * sum(dGauss)...
	  .* B / size(Xsub,2) ;
     case 33
      % Optimoitu
      Y = X(:, getSamples(numSamples, sampleSize))' * B;
      ex = exp(-a2 * (Y .^ 2) / 2);
      gauss = Y .* ex;
      Beta = sum(Y .* gauss);
      D = diag(1 ./ (Beta - sum((1 - a2 * (Y .^ 2)) .* ex)));
      B = B + myy * B * (Y' * gauss - diag(Beta)) * D;
      
      % skew
     case 40
      B = (X * ((X' * B) .^ 2)) / numSamples;
     case 41
      % Optimoitu
      Y = X' * B;
      Gskew = Y .^ 2;
      Beta = sum(Y .* Gskew);
      D = diag(1 ./ (Beta));
      B = B + myy * B * (Y' * Gskew - diag(Beta)) * D;
     case 42
      Xsub=X(:, getSamples(numSamples, sampleSize));
      B = (Xsub * ((Xsub' * B) .^ 2)) / size(Xsub,2);
     case 43
      % Uusi optimoitu
      Y = X(:, getSamples(numSamples, sampleSize))' * B;
      Gskew = Y .^ 2;
      Beta = sum(Y .* Gskew);
      D = diag(1 ./ (Beta));
      B = B + myy * B * (Y' * Gskew - diag(Beta)) * D;
       % kurtosis
     case 50
      B = (X * (( X' * B) .^ 3)) / numSamples - 3 * B;

      
     otherwise
      error('Code for desired nonlinearity not found!');
    end
  end

  
  % Calculate ICA filters.
  W = B' * whiteningMatrix ;

  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Also plot the last one...
  switch usedDisplay
   case 1 
    % There was and may still be other displaymodes...
    % 1D signals
    icaplot('dispsig',(X'*B)');
    drawnow;
   case 2
    % ... and now there are :-)
    % 1D basis
    icaplot('dispsig',A');
    drawnow;
   case 3
    % ... and now there are :-)
    % 1D filters
    icaplot('dispsig',W);
    drawnow;
   otherwise
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DEFLATION APPROACH
if approachMode == 1
  
  B = zeros(vectorSize);
  
  % The search for a basis vector is repeated numOfIC times.
  round = 1;
  
  numFailures = 0;
  
  while round <= numOfIC,
    myy = myyOrig;
    usedNlinearity = gOrig;
    stroke = 0;
    notFine = 1;
    long = 0;
    endFinetuning = 0;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Show the progress...
    if b_verbose, fprintf('IC %d ', round); end
    
    % Take a random initial vector of lenght 1 and orthogonalize it
    % with respect to the other vectors.
    if initialStateMode == 0
      w = randn (vectorSize, 1);
    elseif initialStateMode == 1
      w=whiteningMatrix*guess(:,round);
    end
    w = w - B * B' * w;
    w = w / norm(w);
    
    wOld = zeros(size(w));
    wOld2 = zeros(size(w));
    
    % This is the actual fixed-point iteration loop.
    %    for i = 1 : maxNumIterations + 1
    i = 1;
    gabba = 1;
    while i <= maxNumIterations + gabba
      if (usedDisplay > 0)
	drawnow;
      end
      if (interruptible & g_FastICA_interrupt)
        if b_verbose 
          fprintf('\n\nCalculation interrupted by the user\n');
        end
        return;
      end
      
      % Project the vector into the space orthogonal to the space
      % spanned by the earlier found basis vectors. Note that we can do
      % the projection with matrix B, since the zero entries do not
      % contribute to the projection.
      w = w - B * B' * w;
      w = w / norm(w);
      
      if notFine
	if i == maxNumIterations + 1
	  if b_verbose
	    fprintf('\nComponent number %d did not converge in %d iterations.\n', round, maxNumIterations);
	  end
	  round = round - 1;
	  numFailures = numFailures + 1;
	  if numFailures > failureLimit
	    if b_verbose
	      fprintf('Too many failures to converge (%d). Giving up.\n', numFailures);
	    end
	    if round == 0
	      A=[];
	      W=[];
	    end
	    return;
	  end
	  % numFailures > failurelimit
	  break;
	end
	% i == maxNumIterations + 1
      else
	% if notFine
	if i >= endFinetuning
	  wOld = w; % So the algorithm will stop on the next test...
	end
      end
      % if notFine
      
      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      % Show the progress...
      if b_verbose, fprintf('.'); end;
      
      
      % Test for termination condition. Note that the algorithm has
      % converged if the direction of w and wOld is the same, this
      % is why we test the two cases.
      if norm(w - wOld) < epsilon | norm(w + wOld) < epsilon
        if finetuningEnabled & notFine
          if b_verbose, fprintf('Initial convergence, fine-tuning: '); end;
          notFine = 0;
	  gabba = maxFinetune;
          wOld = zeros(size(w));
          wOld2 = zeros(size(w));
          usedNlinearity = gFine;
          myy = myyK * myyOrig;
	  
	  endFinetuning = maxFinetune + i;
	  
        else
          numFailures = 0;
          % Save the vector
          B(:, round) = w;
	  
          % Calculate the de-whitened vector.
          A(:,round) = dewhiteningMatrix * w;
          % Calculate ICA filter.
          W(round,:) = w' * whiteningMatrix;
	  
          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
          % Show the progress...
          if b_verbose, fprintf('computed ( %d steps ) \n', i); end
	  
          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
          % Also plot the current state...
          switch usedDisplay
	   case 1
	    if rem(round, displayInterval) == 0,
	      % There was and may still be other displaymodes...   
	      % 1D signals
	      temp = X'*B;
	      icaplot('dispsig',temp(:,1:numOfIC)');
	      drawnow;
	    end
	   case 2
	    if rem(round, displayInterval) == 0,
	      % ... and now there are :-) 
	      % 1D basis
	      icaplot('dispsig',A');
	      drawnow;
	    end
	   case 3
	    if rem(round, displayInterval) == 0,
	      % ... and now there are :-) 
	      % 1D filters
	      icaplot('dispsig',W);
	      drawnow;
	    end
          end
	  % switch usedDisplay
	  break; % IC ready - next...
        end
	%if finetuningEnabled & notFine
      elseif stabilizationEnabled
	if (~stroke) & (norm(w - wOld2) < epsilon | norm(w + wOld2) < ...
			epsilon)
	  stroke = myy;
	  if b_verbose, fprintf('Stroke!'); end;
	  myy = .5*myy;
	  if mod(usedNlinearity,2) == 0
	    usedNlinearity = usedNlinearity + 1;
	  end
	elseif stroke
	  myy = stroke;
	  stroke = 0;
	  if (myy == 1) & (mod(usedNlinearity,2) ~= 0)
	    usedNlinearity = usedNlinearity - 1;
	  end
	elseif (notFine) & (~long) & (i > maxNumIterations / 2)
	  if b_verbose, fprintf('Taking long (reducing step size) '); end;
	  long = 1;
	  myy = .5*myy;
	  if mod(usedNlinearity,2) == 0
	    usedNlinearity = usedNlinearity + 1;
	  end
	end
      end
      
      wOld2 = wOld;
      wOld = w;
      
      switch usedNlinearity
	% pow3
       case 10
	w = (X * ((X' * w) .^ 3)) / numSamples - 3 * w;
       case 11
	EXGpow3 = (X * ((X' * w) .^ 3)) / numSamples;
	Beta = w' * EXGpow3;
	w = w - myy * (EXGpow3 - Beta * w) / (3 - Beta);
       case 12
	Xsub=X(:,getSamples(numSamples, sampleSize));
	w = (Xsub * ((Xsub' * w) .^ 3)) / size(Xsub, 2) - 3 * w;
       case 13
	Xsub=X(:,getSamples(numSamples, sampleSize));
	EXGpow3 = (Xsub * ((Xsub' * w) .^ 3)) / size(Xsub, 2);
	Beta = w' * EXGpow3;
	w = w - myy * (EXGpow3 - Beta * w) / (3 - Beta);
	% tanh
       case 20
	hypTan = tanh(a1 * X' * w);
	w = (X * hypTan - a1 * sum(1 - hypTan .^ 2)' * w) / numSamples;
       case 21
	hypTan = tanh(a1 * X' * w);
	Beta = w' * X * hypTan;
	w = w - myy * ((X * hypTan - Beta * w) / ...
		       (a1 * sum((1-hypTan .^2)') - Beta));
       case 22
	Xsub=X(:,getSamples(numSamples, sampleSize));
	hypTan = tanh(a1 * Xsub' * w);
	w = (Xsub * hypTan - a1 * sum(1 - hypTan .^ 2)' * w) / size(Xsub, 2);
       case 23
	Xsub=X(:,getSamples(numSamples, sampleSize));
	hypTan = tanh(a1 * Xsub' * w);
	Beta = w' * Xsub * hypTan;
	w = w - myy * ((Xsub * hypTan - Beta * w) / ...
		       (a1 * sum((1-hypTan .^2)') - Beta));
	% gauss
       case 30
	% This has been split for performance reasons.
	u = X' * w;
	u2=u.^2;
	ex=exp(-a2 * u2/2);
	gauss =  u.*ex;
	dGauss = (1 - a2 * u2) .*ex;
	w = (X * gauss - sum(dGauss)' * w) / numSamples;
       case 31
	u = X' * w;
	u2=u.^2;
	ex=exp(-a2 * u2/2);
	gauss =  u.*ex;
	dGauss = (1 - a2 * u2) .*ex;
	Beta = w' * X * gauss;
	w = w - myy * ((X * gauss - Beta * w) / ...
		       (sum(dGauss)' - Beta));
       case 32
	Xsub=X(:,getSamples(numSamples, sampleSize));
	u = Xsub' * w;
	u2=u.^2;
	ex=exp(-a2 * u2/2);
	gauss =  u.*ex;
	dGauss = (1 - a2 * u2) .*ex;
	w = (Xsub * gauss - sum(dGauss)' * w) / size(Xsub, 2);
       case 33
	Xsub=X(:,getSamples(numSamples, sampleSize));
	u = Xsub' * w;
	u2=u.^2;
	ex=exp(-a2 * u2/2);
	gauss =  u.*ex;
	dGauss = (1 - a2 * u2) .*ex;
	Beta = w' * Xsub * gauss;
	w = w - myy * ((Xsub * gauss - Beta * w) / ...
		       (sum(dGauss)' - Beta));
	% skew
       case 40
	w = (X * ((X' * w) .^ 2)) / numSamples;
       case 41
	EXGskew = (X * ((X' * w) .^ 2)) / numSamples;
	Beta = w' * EXGskew;
	w = w - myy * (EXGskew - Beta*w)/(-Beta);
       case 42
	Xsub=X(:,getSamples(numSamples, sampleSize));
	w = (Xsub * ((Xsub' * w) .^ 2)) / size(Xsub, 2);
       case 43
	Xsub=X(:,getSamples(numSamples, sampleSize));
	EXGskew = (Xsub * ((Xsub' * w) .^ 2)) / size(Xsub, 2);
	Beta = w' * EXGskew;
	w = w - myy * (EXGskew - Beta*w)/(-Beta);
	
    
    % kurtosis
       case 50
	w = (X * ((X' * w) .^ 4)) / numSamples - w - 4*(X * ((X' * w) .^ 3)) / numSamples;
       otherwise
	error('Code for desired nonlinearity not found!');
      end
      
    
      % Normalize the new w.
      w = w / norm(w);
      i = i + 1;
    end
    round = round + 1;
  end
  if b_verbose, fprintf('Done.\n'); end
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Also plot the ones that may not have been plotted.
  if (usedDisplay > 0) & (rem(round-1, displayInterval) ~= 0)
    switch usedDisplay
     case 1
      % There was and may still be other displaymodes...
      % 1D signals
      temp = X'*B;
      icaplot('dispsig',temp(:,1:numOfIC)');
      drawnow;
     case 2
      % ... and now there are :-)
      % 1D basis
      icaplot('dispsig',A');
      drawnow;
     case 3
      % ... and now there are :-)
      % 1D filters
      icaplot('dispsig',W);
      drawnow;
     otherwise
    end
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% In the end let's check the data for some security
if ~isreal(A)
  if b_verbose, fprintf('Warning: removing the imaginary part from the result.\n'); end
  A = real(A);
  W = real(W);
end




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Subfunction
% Calculates tanh simplier and faster than Matlab tanh.
function y=tanh(x)
y = 1 - 2 ./ (exp(2 * x) + 1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Samples = getSamples(max, percentage)
Samples = find(rand(1, max) < percentage);



%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************



function [E, D, Vio] = iopcamat(vectors, firstEig, lastEig, s_interactive, ...
    s_verbose);
%PCAMAT - Calculates the pca for data
%
% [E, D] = pcamat(vectors, firstEig, lastEig, ... 
%                 interactive, verbose);
%
% Calculates the PCA matrices for given data (row) vectors. Returns
% the eigenvector (E) and diagonal eigenvalue (D) matrices containing the
% selected subspaces. Dimensionality reduction is controlled with
% the parameters 'firstEig' and 'lastEig' - but it can also be done
% interactively by setting parameter 'interactive' to 'on' or 'gui'.
%
% ARGUMENTS
%
% vectors       Data in row vectors.
% firstEig      Index of the largest eigenvalue to keep.
%               Default is 1.
% lastEig       Index of the smallest eigenvalue to keep.
%               Default is equal to dimension of vectors.
% interactive   Specify eigenvalues to keep interactively. Note that if
%               you set 'interactive' to 'on' or 'gui' then the values
%               for 'firstEig' and 'lastEig' will be ignored, but they
%               still have to be entered. If the value is 'gui' then the
%               same graphical user interface as in FASTICAG will be
%               used. Default is 'off'.
% verbose       Default is 'on'.
%
%
% EXAMPLE
%       [E, D] = pcamat(vectors);
%
% Note 
%       The eigenvalues and eigenvectors returned by PCAMAT are not sorted.
%
% This function is needed by FASTICA and FASTICAG

% For historical reasons this version does not sort the eigenvalues or
% the eigen vectors in any ways. Therefore neither does the FASTICA or
% FASTICAG. Generally it seams that the components returned from
% whitening is almost in reversed order. (That means, they usually are,
% but sometime they are not - depends on the EIG-command of matlab.)

% @(#)$Id: pcamat.m,v 1.5 2003/12/15 18:24:32 jarmo Exp $

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Default values:
if nargin < 5, s_verbose = 'on'; end
if nargin < 4, s_interactive = 'off'; end
if nargin < 3, lastEig = size(vectors, 1); end
if nargin < 2, firstEig = 1; end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Check the optional parameters;
switch lower(s_verbose)
 case 'on'
  b_verbose = 1;
 case 'off'
  b_verbose = 0;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''verbose''\n', s_verbose));
end

switch lower(s_interactive)
 case 'on'
  b_interactive = 1;
 case 'off'
  b_interactive = 0;
 case 'gui'
  b_interactive = 2;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''interactive''\n', ...
		s_interactive));
end

oldDimension = size (vectors, 1);
if ~(b_interactive)
  if lastEig < 1 | lastEig > oldDimension
    error(sprintf('Illegal value [ %d ] for parameter: ''lastEig''\n', lastEig));
  end
  if firstEig < 1 | firstEig > lastEig
    error(sprintf('Illegal value [ %d ] for parameter: ''firstEig''\n', firstEig));
  end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculate PCA

% Calculate the covariance matrix.
if b_verbose, fprintf ('Calculating PCA...\n'); end
%covarianceMatrix = cov(vectors', 1);

% Calculate the eigenvalues and eigenvectors of covariance
% matrix.
%[E, D] = eig (covarianceMatrix);

NN=size(vectors,2);

[E,D,Vio]=svds(vectors/sqrt(NN),lastEig+1);

D=D.^2;

% The rank is determined from the eigenvalues - and not directly by
% using the function rank - because function rank uses svd, which
% in some cases gives a higher dimensionality than what can be used
% with eig later on (eig then gives negative eigenvalues).
rankTolerance = 1e-7;
maxLastEig = sum (diag (D) > rankTolerance);
if maxLastEig == 0,
  fprintf (['Eigenvalues of the covariance matrix are' ...
	    ' all smaller than tolerance [ %g ].\n' ...
	    'Please make sure that your data matrix contains' ...
	    ' nonzero values.\nIf the values are very small,' ...
	    ' try rescaling the data matrix.\n'], rankTolerance);
  error ('Unable to continue, aborting.');
end

% Sort the eigenvalues - decending.
eigenvalues = diag(D);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Interactive part - command-line
if b_interactive == 1

  % Show the eigenvalues to the user
  hndl_win=figure;
  bar(eigenvalues);
  title('Eigenvalues');

  % ask the range from the user...
  % ... and keep on asking until the range is valid :-)
  areValuesOK=0;
  while areValuesOK == 0
    firstEig = input('The index of the largest eigenvalue to keep? (1) ');
    lastEig = input(['The index of the smallest eigenvalue to keep? (' ...
                    int2str(oldDimension) ') ']);
    % Check the new values...
    % if they are empty then use default values
    if isempty(firstEig), firstEig = 1;end
    if isempty(lastEig), lastEig = oldDimension;end
    % Check that the entered values are within the range
    areValuesOK = 1;
    if lastEig < 1 | lastEig > oldDimension
      fprintf('Illegal number for the last eigenvalue.\n');
      areValuesOK = 0;
    end
    if firstEig < 1 | firstEig > lastEig
      fprintf('Illegal number for the first eigenvalue.\n');
      areValuesOK = 0;
    end
  end
  % close the window
  close(hndl_win);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Interactive part - GUI
if b_interactive == 2

  % Show the eigenvalues to the user
  hndl_win = figure('Color',[0.8 0.8 0.8], ...
    'PaperType','a4letter', ...
    'Units', 'normalized', ...
    'Name', 'FastICA: Reduce dimension', ...
    'NumberTitle','off', ...
    'Tag', 'f_eig');
  h_frame = uicontrol('Parent', hndl_win, ...
    'BackgroundColor',[0.701961 0.701961 0.701961], ...
    'Units', 'normalized', ...
    'Position',[0.13 0.05 0.775 0.17], ...
    'Style','frame', ...
    'Tag','f_frame');

b = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'BackgroundColor',[0.701961 0.701961 0.701961], ...
	'HorizontalAlignment','left', ...
	'Position',[0.142415 0.0949436 0.712077 0.108507], ...
	'String','Give the indices of the largest and smallest eigenvalues of the covariance matrix to be included in the reduced data.', ...
	'Style','text', ...
	'Tag','StaticText1');
e_first = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'Callback',[ ...
          'f=round(str2num(get(gcbo, ''String'')));' ...
          'if (f < 1), f=1; end;' ...
          'l=str2num(get(findobj(''Tag'',''e_last''), ''String''));' ...
          'if (f > l), f=l; end;' ...
          'set(gcbo, ''String'', int2str(f));' ...
          ], ...
	'BackgroundColor',[1 1 1], ...
	'HorizontalAlignment','right', ...
	'Position',[0.284831 0.0678168 0.12207 0.0542535], ...
	'Style','edit', ...
        'String', '1', ...
	'Tag','e_first');
b = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'BackgroundColor',[0.701961 0.701961 0.701961], ...
	'Horizo)ntalAlignment','left', ...
	'Position',[0.142415 0.0678168 0.12207 0.0542535], ...
	'String','Range from', ...
	'Style','text', ...
	'Tag','StaticText2');
e_last = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'Callback',[ ...
          'l=round(str2num(get(gcbo, ''String'')));' ...
          'lmax = get(gcbo, ''UserData'');' ...
          'if (l > lmax), l=lmax; fprintf([''The selected value was too large, or the selected eigenvalues were close to zero\n'']); end;' ...
          'f=str2num(get(findobj(''Tag'',''e_first''), ''String''));' ...
          'if (l < f), l=f; end;' ...
          'set(gcbo, ''String'', int2str(l));' ...
          ], ...
	'BackgroundColor',[1 1 1], ...
	'HorizontalAlignment','right', ...
	'Position',[0.467936 0.0678168 0.12207 0.0542535], ...
	'Style','edit', ...
        'String', int2str(maxLastEig), ...
        'UserData', maxLastEig, ...
	'Tag','e_last');
% in the first version oldDimension was used instead of 
% maxLastEig, but since the program would automatically
% drop the eigenvalues afte maxLastEig...
b = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'BackgroundColor',[0.701961 0.701961 0.701961], ...
	'HorizontalAlignment','left', ...
	'Position',[0.427246 0.0678168 0.0406901 0.0542535], ...
	'String','to', ...
	'Style','text', ...
	'Tag','StaticText3');
b = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'Callback','uiresume(gcbf)', ...
	'Position',[0.630697 0.0678168 0.12207 0.0542535], ...
	'String','OK', ...
	'Tag','Pushbutton1');
b = uicontrol('Parent',hndl_win, ...
	'Units','normalized', ...
	'Callback',[ ...
          'gui_help(''pcamat'');' ...
          ], ...
	'Position',[0.767008 0.0678168 0.12207 0.0542535], ...
	'String','Help', ...
	'Tag','Pushbutton2');

  h_axes = axes('Position' ,[0.13 0.3 0.775 0.6]);
  set(hndl_win, 'currentaxes',h_axes);
  bar(eigenvalues);
  title('Eigenvalues');

  uiwait(hndl_win);
  firstEig = str2num(get(e_first, 'String'));
  lastEig = str2num(get(e_last, 'String'));

  % close the window
  close(hndl_win);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% See if the user has reduced the dimension enought

if lastEig > maxLastEig
  lastEig = maxLastEig;
  if b_verbose
    fprintf('Dimension reduced to %d due to the singularity of covariance matrix\n',...
           lastEig-firstEig+1);
  end
else
  % Reduce the dimensionality of the problem.
  if b_verbose
    if oldDimension == (lastEig - firstEig + 1)
      fprintf ('Dimension not reduced.\n');
    else
      fprintf ('Reducing dimension...\n');
    end
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Drop the smaller eigenvalues
if lastEig < oldDimension
  lowerLimitValue = (eigenvalues(lastEig) + eigenvalues(lastEig + 1)) / 2;
else
  lowerLimitValue = eigenvalues(oldDimension) - 1;
end

lowerColumns = diag(D) > lowerLimitValue;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Drop the larger eigenvalues
if firstEig > 1
  higherLimitValue = (eigenvalues(firstEig - 1) + eigenvalues(firstEig)) / 2;
else
  higherLimitValue = eigenvalues(1) + 1;
end
higherColumns = diag(D) < higherLimitValue;

% Combine the results from above
selectedColumns = lowerColumns & higherColumns;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% print some info for the user
if b_verbose
  fprintf ('Selected [ %d ] dimensions.\n', sum (selectedColumns));
end
if sum (selectedColumns) ~= (lastEig - firstEig + 1),
  error ('Selected a wrong number of dimensions.');
end

if b_verbose
  fprintf ('Smallest remaining (non-zero) eigenvalue [ %g ]\n', eigenvalues(lastEig));
  fprintf ('Largest remaining (non-zero) eigenvalue [ %g ]\n', eigenvalues(firstEig));
  fprintf ('Sum of removed eigenvalues [ %g ]\n', sum(diag(D) .* ...
    (~selectedColumns)));
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Select the colums which correspond to the desired range
% of eigenvalues.
E = selcol(E, selectedColumns);
Vio = selcol(Vio, selectedColumns);
D = selcol(selcol(D, selectedColumns)', selectedColumns);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Some more information
if b_verbose
  sumAll=sum(eigenvalues);
  sumUsed=sum(diag(D));
  retained = (sumUsed / sumAll) * 100;
  fprintf('[ %g ] %% of (non-zero) eigenvalues retained.\n', retained);
end





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function newMatrix = selcol(oldMatrix, maskVector);

% newMatrix = selcol(oldMatrix, maskVector);
%
% Selects the columns of the matrix that marked by one in the given vector.
% The maskVector is a column vector.

% 15.3.1998

if size(maskVector, 1) ~= size(oldMatrix, 2),
  error ('The mask vector and matrix are of uncompatible size.');
end

numTaken = 0;

for i = 1 : size (maskVector, 1),
  if maskVector(i, 1) == 1,
    takingMask(1, numTaken + 1) = i;
    numTaken = numTaken + 1;
  end
end

newMatrix = oldMatrix(:, takingMask);



%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************



function [newVectors, meanValue] = remmean(vectors);
%REMMEAN - remove the mean from vectors
%
% [newVectors, meanValue] = remmean(vectors);
%
% Removes the mean of row vectors.
% Returns the new vectors and the mean.
%
% This function is needed by FASTICA and FASTICAG

% @(#)$Id: remmean.m,v 1.2 2003/04/05 14:23:58 jarmo Exp $

newVectors = zeros (size (vectors));
meanValue = mean (vectors')';
newVectors = vectors - meanValue * ones (1,size (vectors, 2));


%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************


function [newVectors, whiteningMatrix, dewhiteningMatrix] = whitenv ...
    (vectors, E, D, s_verbose);
%WHITENV - Whitenv vectors.
%
% [newVectors, whiteningMatrix, dewhiteningMatrix] = ...
%                               whitenv(vectors, E, D, verbose);
%
% Whitens the data (row vectors) and reduces dimension. Returns
% the whitened vectors (row vectors), whitening and dewhitening matrices.
%
% ARGUMENTS
%
% vectors       Data in row vectors.
% E             Eigenvector matrix from function 'pcamat'
% D             Diagonal eigenvalue matrix from function 'pcamat'
% verbose       Optional. Default is 'on'
%
% EXAMPLE
%       [E, D] = pcamat(vectors);
%       [nv, wm, dwm] = whitenv(vectors, E, D);
%
%
% This function is needed by FASTICA and FASTICAG
%
%   See also PCAMAT

% @(#)$Id: whitenv.m,v 1.3 2003/10/12 09:04:43 jarmo Exp $

% ========================================================
% Default value for 'verbose'
if nargin < 5, s_verbose = 'on'; end

% Check the optional parameter verbose;
switch lower(s_verbose)
 case 'on'
  b_verbose = 1;
 case 'off'
  b_verbose = 0;
 otherwise
  error(sprintf('Illegal value [ %s ] for parameter: ''verbose''\n', s_verbose));
end

% ========================================================
% In some cases, rounding errors in Matlab cause negative
% eigenvalues (elements in the diagonal of D). Since it
% is difficult to know when this happens, it is difficult
% to correct it automatically. Therefore an error is 
% signalled and the correction is left to the user.
if any (diag (D) < 0),
  error (sprintf (['[ %d ] negative eigenvalues computed from the' ...
		   ' covariance matrix.\nThese are due to rounding' ...
		   ' errors in Matlab (the correct eigenvalues are\n' ...
		   'probably very small).\nTo correct the situation,' ...
		   ' please reduce the number of dimensions in the' ...
		   ' data\nby using the ''lastEig'' argument in' ...
		   ' function FASTICA, or ''Reduce dim.'' button\nin' ...
		   ' the graphical user interface.'], ...
		  sum (diag (D) < 0)));
end

% ========================================================
% Calculate the whitening and dewhitening matrices (these handle
% dimensionality simultaneously).
whiteningMatrix = inv (sqrt (D)) * E';
dewhiteningMatrix = E * sqrt (D);




% Project to the eigenvectors of the covariance matrix.
% Whiten the samples and reduce dimension simultaneously.
if b_verbose, fprintf ('Whitening...\n'); end
newVectors =  whiteningMatrix * vectors;


% ========================================================
% Just some security...
if ~isreal(newVectors)
  error ('Whitened vectors have imaginary values.');
end

% Print some information to user
if b_verbose
  fprintf ('Check: covariance differs from identity by [ %g ].\n', ...
    max (max (abs (cov (newVectors', 1) - eye (size (newVectors, 1))))));
end



%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************
%**************************************************************************


% --- Executes on button press in apply_threshold.
function apply_threshold_Callback(hObject, eventdata, handles)
% hObject    handle to apply_threshold (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

maskthreshold=str2num(get(handles.ica_th,'string'));
spcomp=sign(maskthreshold)*handles.currentspcomp;
spcompCompl=sign(-maskthreshold)*handles.currentspcomp;

maskthreshold=abs(maskthreshold);

mask=zeros(size(spcomp));
maskComp=zeros(size(spcomp));
inds=find(spcomp>maskthreshold);
indsComp=find(spcompCompl>maskthreshold);

mask(inds)=1;
maskComp(indsComp)=1;
%%%% update threshold new code andre Dec 2012
mov=double(handles.mov);
[x,y,z]=size(mov);
mov=reshape(mov,x*y,z);
cmask=reshape(mask,1,x*y);
cmaskComp=reshape(maskComp,1,x*y);

cla(handles.axes3);
axes(handles.axes3);
hold on
plot(cmask*mov/sum(cmask));
plot(cmaskComp*mov/sum(cmaskComp),'r');
hold off
%%%%




cla(handles.axes5);
axes(handles.axes5);
imagesc(mask)
colormap(gray)

cla(handles.axes6);
axes(handles.axes6);
imagesc(maskComp)
colormap(gray)



handles.currentmask=mask;
handles.dendmask=mask;
guidata(hObject, handles);


function ica_th_Callback(hObject, eventdata, handles)
% hObject    handle to ica_th (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of ica_th as text
%        str2double(get(hObject,'String')) returns contents of ica_th as a double


% --- Executes during object creation, after setting all properties.
function ica_th_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ica_th (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in remove_points.
function remove_points_Callback(hObject, eventdata, handles)
% hObject    handle to remove_points (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

currentmask=handles.currentmask;
tempmask=currentmask;

maskfig=figure; 
imagesc(currentmask); 
colormap(gray);

inds=roipoly;
currentmask(inds)=0;

currentmask=tempmask-currentmask;
handles.currentmask=currentmask;
handles.dendmask=currentmask;
close (maskfig);

cla(handles.axes5);
axes(handles.axes5);
imagesc(currentmask)
colormap(gray)

guidata(hObject, handles);

% --- Executes on button press in add_to_mask.
function add_to_mask_Callback(hObject, eventdata, handles)
% hObject    handle to add_to_mask (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

pixels_x=handles.pixels_x;
maskcnt=handles.maskcnt;
handles.posx=zeros(maskcnt+1,1);
handles.posy=zeros(maskcnt+1,1);

hororder=get(handles.hororder,'value');
pixels_y=handles.pixels_y;

if maskcnt~=1;
    tempmask=handles.mainmask;
end

if maskcnt==1;
    tempmask=zeros(pixels_x,pixels_y,maskcnt);
end

mainmask=zeros(pixels_x,pixels_y,maskcnt);

if maskcnt~=1;
    for i=1:maskcnt-1;
        mainmask(:,:,i)=tempmask(:,:,i);
    end
end

mov=double(handles.mov);
[x,y,z]=size(mov);
mov=reshape(mov,x*y,z);
cmask=reshape(handles.dendmask,1,x*y);

cla(handles.axes3);
axes(handles.axes3);
plot(cmask*mov/sum(cmask));

mainmask(:,:,maskcnt)=handles.dendmask(:,:);
totaldendmask=zeros(pixels_x,pixels_y);

for i=1:maskcnt;
    totaldendmask(:,:)=totaldendmask(:,:)+mainmask(:,:,i);    
end

for i=1:pixels_x;
    for j=1:pixels_y;
        if totaldendmask(i,j)>2;
            totaldendmask(i,j)=2;
        end
    end
end

totalmasktemp(:,:,1)=totaldendmask(:,:);

mainmask=cat(3,mainmask,totalmasktemp);

set(handles.slider2,'max',(maskcnt+1));
set(handles.slider2,'min',0);
set(handles.slider2,'SliderStep',[1/(maskcnt+1) 1/(maskcnt+1)]);

set(handles.mask_num,'String',num2str(maskcnt+1));
set(handles.slider2,'Value',(maskcnt+1));


handles.maskcnt=maskcnt+1;
masksize=size(mainmask);
numdendrites=masksize(3)-1;

for i=1:numdendrites;
    dend(:,:)=mainmask(:,:,i);
    [x,y]=find(dend);
    posx(i)=(max(x)+min(x))/2;
    posy(i)=round((max(y)+min(y))/2);   
end

tempx=posx(numdendrites);
tempy=posy(numdendrites); 

tempmask=(zeros(masksize(1),masksize(2)));
qq=0;   

for i=1:numdendrites;
    if hororder==0;
        [posval,posmin]=min(posx(i:numdendrites));
        if posmin==numdendrites-i+1&qq==0;
            lastdendrite=i;
            qq=qq+1;
        end
        aa=posy(posmin+i-1);
        posy(posmin+i-1)=posy(i);
        posy(i)=aa;
        posx(posmin+i-1)=posx(i);
        posx(i)=posval;
        if posval==tempx;
            curmasknum=i;
        end
        tempmask(:,:)=mainmask(:,:,posmin+i-1);
        mainmask(:,:,posmin+i-1)=mainmask(:,:,i);
        mainmask(:,:,i)=tempmask(:,:);
    else
        [posval,posmin]=min(posy(i:numdendrites));
        if posmin==numdendrites-i+1&qq==0;
            lastdendrite=i;
            qq=qq+1;
        end
        aa=posx(posmin+i-1);
        posx(posmin+i-1)=posy(i);
        posx(i)=aa;
        posy(posmin+i-1)=posy(i);
        posy(i)=posval;
        if posval==tempy;
            curmasknum=i;
        end
        tempmask(:,:)=mainmask(:,:,posmin+i-1);
        mainmask(:,:,posmin+i-1)=mainmask(:,:,i);
        mainmask(:,:,i)=tempmask(:,:);
    end
end

handles.posy=posy;
handles.posx=posx;
handles.mainmask=mainmask;
handles.lastdendrite=lastdendrite;

set(handles.mask_num,'string',num2str(curmasknum));
set(handles.slider2,'value',curmasknum);
axes(handles.axes2);
imagesc(mainmask(:,:,curmasknum))
colormap(gray)



guidata(hObject, handles);


% --- Executes on button press in save_mask.
function save_mask_Callback(hObject, eventdata, handles)
% hObject    handle to save_mask (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

filterspec = {'*.mat','mat files (.mat)'; '*.dat','dat files (.dat)'};
title = 'Save';

temp_file=[handles.fname '_dendmask'];
dendmask=handles.mainmask;

filename=handles.fname;

[filename, path, index] = uiputfile (filterspec, title, temp_file);
if index == 0
    % No action taken...the user has cancelled the Save
    
elseif index == 1
    % Save the file as a .mat file   
     save([path filename],'dendmask');
     handles.folder=path;
end

guidata(hObject, handles);


% --- Executes on button press in load_mask.
function load_mask_Callback(hObject, eventdata, handles)
% hObject    handle to load_mask (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
filterspec = {'*.mat','MAT files (.mat)'};
title = 'load';
folder=handles.folder
temp_file=folder;

[filename, path, index] = uigetfile (filterspec, title, temp_file);
eval(['addpath' ' ' char(39) path char(39) ';']);

handles.folder=path;

total_filename=filename;
fnlength=length(total_filename);

if index == 0;
    % No action taken...the user has cancelled the Save
    
elseif index == 1;
    eval(['mask=load(' char(39) total_filename char(39) ');']);
    
end

s=fieldnames(mask);

eval(['mainmask=mask.' s{1} ';']);

mainmask_size=size(mainmask);
numdend=mainmask_size(3)-1;

totaldendmask(:,:)=mainmask(:,:,(numdend+1));

set(handles.slider2,'max',(numdend+1));
set(handles.slider2,'min',0);
set(handles.slider2,'SliderStep',[1/(numdend+1) 1/(numdend+1)]);

set(handles.mask_num,'String',num2str(numdend+1));
set(handles.slider2,'Value',(numdend+1));


cla(handles.axes2);
axes(handles.axes2);
imagesc(totaldendmask)
colormap(gray)

handles.eventcounter=zeros(100,1);
handles.posx=zeros(numdend,1);
handles.posy=zeros(numdend,1);
handles.xLength=zeros(numdend,1);
handles.yLength=zeros(numdend,1);
handles.numpixel=zeros(numdend,1);
handles.maskcnt=numdend+1;
handles.numdend=numdend;
handles.mainmask=mainmask;

guidata(hObject, handles);


% --- Executes on button press in plot_all_traces.
function plot_all_traces_Callback(hObject, eventdata, handles)
% hObject    handle to plot_all_traces (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

mainmask=double(handles.mainmask);
nummasks=size(mainmask,3)-1;
mov=double(handles.mov);


[x,y,z]=size(mov);
mov=reshape(mov,x*y,z);
trmat=zeros(nummasks,z);

for i=1:nummasks;
    chosenmask=mainmask(:,:,i);
    chosenmask=reshape(chosenmask,1,x*y);
    numpix=sum(chosenmask);
    tr=chosenmask*mov/numpix;
    trmat(i,:)=tr;
end
    
handles.trmat=trmat;
ioplottracesgui(trmat);
guidata(hObject, handles);


function [ ] = ioplottracesgui (eventsmat);

[numdends,numpnts]=size(eventsmat);
 
figure
hold on
for i=1:numdends;
    eval(['x=eventsmat(' num2str(i) ',:);']);
    x=squeeze(x);
    x=(x-min(x))/(max(x)-min(x));
    
    if nargin<4
        if i>=2;
            x=1.1*(i-1)+x;
        end
    else
        if i>=2;
            x=space*(i-1)+x;
        end
    end
    plot(1:numpnts, x)
end


% --- Executes on button press in save_all_traces.
function save_all_traces_Callback(hObject, eventdata, handles)
% hObject    handle to save_all_traces (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
trmat=handles.trmat;
dendfrnummat=handles.dendfrnummat;

filterspec = {'*.mat','mat files (.mat)'; '*.dat','dat files (.dat)'};
title = 'Save';
temp_file=[handles.fname '_trmat'];
filename=handles.fname;

[filename, path, index] = uiputfile (filterspec, title, temp_file);
if index == 0
    % No action taken...the user has cancelled the Save
    
elseif index == 1
    % Save the file as a .mat file
     save([path filename],'trmat');
     handles.folder=path;
end

guidata(hObject, handles);


% --- Executes on button press in load_ica.
function load_ica_Callback(hObject, eventdata, handles)
% hObject    handle to load_ica (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

filterspec = {'*.mat','MAT files (.mat)'};
title = 'load';
temp_file=handles.folder;
[filename, path, index] = uigetfile (filterspec, title, temp_file);
eval(['addpath' ' ' char(39) path char(39) ';']);
cd(path);
handles.folder=path;
total_filename=[handles.folder filename];

if index == 0;    
elseif index == 1;

    fin=load(total_filename);
    names=fieldnames(fin);
    icacomps=getfield(fin,names{1})
    spcomps=icacomps.spcomps;
    tpcomps=icacomps.tpcomps;
    
end
    

handles.spcomps=spcomps;
handles.tpcomps=tpcomps;

numcomps=size(tpcomps,1);

set(handles.ica_num,'string',num2str(numcomps));
set(handles.slider3,'max',numcomps);
set(handles.slider3,'min',0);
set(handles.slider3,'SliderStep',[1/numcomps 1/numcomps]);
set(handles.slider3,'value',numcomps);

axes(handles.axes4);
imagesc(spcomps(:,:,end))
colormap(jet)

x=tpcomps(end,:);
if mean(x)-min(x)>max(x)-mean(x);
    x=-x;
end

handles.currentspcomp=spcomps(:,:,end);

cla(handles.axes3);
axes(handles.axes3);
plot(x)
zoom on
    

[x,y,z]=size(spcomps);

handles.pixels_x=x;
handles.pixels_y=y;

guidata(hObject, handles);


% --- Executes on button press in choose_nonzero.
function choose_nonzero_Callback(hObject, eventdata, handles)
% hObject    handle to choose_nonzero (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
frames=handles.frames;
pixels_x=handles.pixels_x;
pixels_y=handles.pixels_y;
dendmask=zeros(pixels_x,pixels_y);
mov=double(handles.mov);

mov1=reshape(mov,pixels_x*pixels_y,handles.frames);
mov1=mov1';
mov1=mean(mov1);
mov1=mov1';
mov1=reshape(mov1,pixels_x,pixels_y);

[x,y]=find(mov1>0);
allinds= mov1>0;

dendmask(allinds)=1;

xmax=max(x);
ymax=max(y);
xmin=min(x);
ymin=min(y);
xsize=xmax-xmin+1;
ysize=ymax-ymin+1;

frame2=mov1;

cla(handles.axes2);
axes(handles.axes2);
imagesc(frame2)
colormap(gray)

set(handles.slider2,'value',0);
set(handles.mask_num,'string',num2str(0));

mov=reshape(mov,pixels_x*pixels_y,handles.frames);
mask=reshape(dendmask,pixels_x*pixels_y,1);

tr=mask'*mov/length(x);

cla(handles.axes3);
axes(handles.axes3);
plot(tr)
zoom on

handles.masktrace=tr;
handles.xsize=xsize;
handles.ysize=ysize;
handles.xmax=xmax;
handles.xmin=xmin;
handles.ymax=ymax;
handles.ymin=ymin;
handles.dendmask=dendmask;
    
guidata(hObject, handles);
