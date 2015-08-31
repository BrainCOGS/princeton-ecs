function ecsCompile(varargin)
% ecsCompile    MEX compilation utility for the Princeton ECS package.
%
% This script will compile all MEX files in the src subdirectory (relative
% to the directory in which ecsCompile.m is located). Additionally you can
% install the mexopencv (https://github.com/kyamagu/mexopencv) package,
% which has more MEX interfaces to OpenCV; however this is not required.
%
% Users will typically only have to call ecsCompile() without arguments.
% Arguments can be provided, in which case they will be passed on to the
% mex() calls verbatim, except for '-F' which will force recompilation of
% all programs.
%
% ======================
%  Note for developers
% ======================
% 
% To add a MEX program to the compilation list, simply put it in the src
% directory. ecsCompile() will attempt to guess whether or not a given MEX
% program depends on OpenCV, by detecting use of the cv namespace (e.g.
% "cv::Mat"), or use of an OpenCV constant (e.g. "CV_8U"). If neither of
% these are found, it will compile the MEX program without linking OpenCV
% libraries. MEX programs that use OpenCV will be located in the +cv
% directory, and those without will be located in the +ecs directory.
%
% Code in the following subdirectories are treated in special ways:
%   src/lib       C++ code that should be compiled into shared object
%                 libraries instead of standalone programs. These object
%                 files will be linked to all MEX programs if they are not
%                 OpenCV dependent; otherwise they will be linked to only
%                 OpenCV MEX programs.
%   src/private   MEX programs here will be located in the appropriate
%                 private subdirectories, i.e. not directly accessible to
%                 users.


%===================================================================================================
%   Argument parsing
%===================================================================================================

lazy            = true;
debug           = false;
genEnums        = false;
for iArg = numel(varargin):-1:1
  if strcmp(varargin{iArg}, '-F')
    lazy        = false;
    varargin(iArg)  = [];
  elseif strcmp(varargin{iArg}, '-g')
    debug       = true;
  elseif strcmp(varargin{iArg}, '-E')
    genEnums    = true;
  end
end


%===================================================================================================
%   Installation check and dependency lists
%===================================================================================================

%----------  Configuration
PRINCETONECS    = fileparts(mfilename('fullpath'));
ECS_SRC         = fullfile(PRINCETONECS, 'src');
ECS_PRIVATE     = fullfile(ECS_SRC     , 'private');
ECS_LIB         = fullfile(ECS_SRC     , 'lib');
ECS_OPENCV      = fullfile(ECS_SRC     , 'opencv');
MEX_CV          = fullfile(PRINCETONECS, '+cv');
MEX_CVENUM      = fullfile(PRINCETONECS, '+cve');
MEX_ECS         = fullfile(PRINCETONECS, '+ecs');
PRIVATE_CV      = fullfile(PRINCETONECS, '+cv' , 'private');
PRIVATE_ECS     = fullfile(PRINCETONECS, '+ecs', 'private');

% Add required 3rd party libraries here
THIRDPARTY      = {'libtiff', 'zlib'};

currentDir      = pwd();
cleanup         = onCleanup(@() cd(currentDir));



% .lib -> .so
% .obj -> .o
% opencv & opencv_base -> /usr/local
% user configurable 3rdparty location
% no 'debug' mode

%----------  OpenCV installation
try
  OPENCV        = getenv('OPENCV_DIR');
catch err
  fprintf('Error encountered while trying to detect OpenCV installation:\n  %s    %s\n', err.identifier, err.message);
  fprintf('Make sure that you have OpenCV installed and the OPENCV_DIR environmental variable set.\n');
  fprintf('For example it could look like "C:\OpenCV\install\x64\vc11" in a 64-bit Windows machine.\n');
  return;
end

OPENCV_BASE     = fileparts(fileparts(OPENCV));
OPENCV_INC      = fullfile(OPENCV_BASE, 'include');
if ~exist(OPENCV_INC, 'dir')
  error('ecsCompile:opencv', 'OpenCV headers directory %s not found.', OPENCV_INC);
end

OPENCV_LIB      = fullfile(OPENCV, 'lib');
cvLibs          = dir( fullfile(OPENCV_LIB,'opencv_*d.lib') );
[~, cvLibs]     = cellfun(@fileparts, {cvLibs.name}, 'UniformOutput', false);
if ~debug       % Select non-debug libraries
  cvLibs        = cellfun(@(x) x(1:end-1), cvLibs, 'UniformOutput', false);
end
cvLibs          = strcat('-l', cvLibs);
if isempty(cvLibs)
  error('ecsCompile:opencv', 'OpenCV libraries not found in %s.', OPENCV_LIB);
end

OPENCV_3RD      = fullfile(fileparts(OPENCV_BASE), '3rdparty', 'lib');
if debug
  OPENCV_3RD    = fullfile(OPENCV_3RD, 'Debug');
  postfix       = 'd';
else
  OPENCV_3RD    = fullfile(OPENCV_3RD, 'Release');
  postfix       = '';
end

for iLib = 1:numel(THIRDPARTY)
  cvLibs{end+1} = sprintf('-l%s%s.lib', THIRDPARTY{iLib}, postfix);
end


%----------  Global code compilation options
if debug
  varargin{end+1} = '-D_DEBUG';
end
ecsOpts       = varargin;

%----------  Additional OpenCV code compilation options
if debug
  varargin{end+1} = 'COMPFLAGS="$COMPFLAGS /D_SECURE_SCL=1 /MDd"';
end
cvOpts        = [ varargin                                ...
                , { '-largeArrayDims'                     ...
                  , sprintf('-I"%s"', OPENCV_INC)         ...
                  , sprintf('-I"%s"', ECS_OPENCV)         ...
                  , sprintf('-L"%s"', OPENCV_LIB)         ...
                  , sprintf('-L"%s"', OPENCV_3RD)         ...
                  }                                       ...
                ];
if strncmpi(computer('arch'), 'win', 3)
  cvOpts{end+1} = '-DWIN32';
end
cvOpts        = [ cvOpts                                  ...
                , cvLibs                                  ...
                ];


%===================================================================================================
%   Compilation procedure
%===================================================================================================

% Compile OpenCV clones
opencvObjs    = doCompile(dir(fullfile(ECS_OPENCV, '*.cpp')), ECS_OPENCV, ECS_OPENCV, 'obj', [{'-c'} cvOpts], false, lazy);
cvOpts        = [cvOpts , opencvObjs];
  
% Get separate lists of OpenCV dependent and non-dependent code files
[cvSrc,ecsSrc]= getMEXCode(ECS_LIB, '*.cpp');

% Compile common object files
ecsObjs       = doCompile(ecsSrc, ECS_LIB, ECS_LIB, 'obj', [{'-c'} ecsOpts], false, lazy);
ecsOpts       = [ecsOpts, ecsObjs];
cvOpts        = [cvOpts , ecsObjs];

% Compile OpenCV specific object files
cvObjs        = doCompile(cvSrc, ECS_LIB, ECS_LIB, 'obj', [{'-c'} cvOpts], false, lazy);
cvOpts        = [cvOpts, cvObjs];

% Compile separately OpenCV dependent and non-dependent MEX programs
[cvSrc,ecsSrc]= getMEXCode(ECS_SRC, '*.cpp');
doCompile(ecsSrc, ECS_SRC, MEX_ECS, mexext, ecsOpts, true, lazy);
doCompile(cvSrc , ECS_SRC, MEX_CV , mexext, cvOpts , true, lazy);

% Compile separately private MEX programs
[cvSrc,ecsSrc]= getMEXCode(ECS_PRIVATE, '*.cpp');
doCompile(ecsSrc, ECS_PRIVATE, PRIVATE_ECS, mexext, ecsOpts, true, lazy);
doCompile(cvSrc , ECS_PRIVATE, PRIVATE_CV , mexext, cvOpts , true, lazy);

% Generate enumeration types
if genEnums
  generateEnums(fullfile(OPENCV_INC, 'opencv2'), MEX_CVENUM);
end


fprintf('\n\n     ********************  Princeton ECS compilation complete  ********************\n\n');

end

%%
function [cvMex, otherMex] = getMEXCode(srcDir, srcMask)

  if ~exist(srcDir, 'dir')
    cvMex             = {};
    otherMex          = {};
    return;
  end

  cd(srcDir);
  srcFile             = dir(fullfile(srcDir, srcMask));
  cvMex               = srcFile;
  otherMex            = srcFile;
  
  for iFile = numel(srcFile):-1:1
    source            = fileread(srcFile(iFile).name);
    cvMatch           = regexp( source                                                        ...
                              , '^\s*#\s*include\s+.*(opencv)|\<cv\s*::|\<CV_|\<Cv[A-Z]'      ...
                              , 'lineanchors', 'dotexceptnewline', 'once'                     ...
                              );
    if isempty(cvMatch)
      cvMex(iFile)    = [];
    else
      otherMex(iFile) = [];
    end
  end

end

%%
function outFile = doCompile(srcFile, srcDir, outDir, outExt, options, generateMFile, lazy, verbose)

  % Default arguments
  if nargin < 6
    generateMFile   = false;
  end
  if nargin < 7
    lazy            = true;
  end
  if nargin < 8
    verbose         = true;
  end
  
  % Get list of files to compile
  outFile           = cell(1, numel(srcFile));
  if isempty(srcFile)
    return;
  end
  cd(srcDir);
  
  fprintf('\n     ********************  %s\n', srcDir);
  hWait             = waitbar(0, strrep(srcDir, '\', '\\'));
  
  % Create output directory if necessary
  if ~exist(outDir, 'dir')
    mkdir(outDir);
  end
  
  for iFile = 1:numel(srcFile)
    [~, target, ~]  = fileparts(srcFile(iFile).name);
    target          = fullfile(outDir, [target '.' outExt]);
    outFile{iFile}  = target;
    waitbar(iFile / numel(srcFile), hWait);
    
    % Skip if already compiled
    if lazy && exist(target, 'file')
      targetInfo    = dir(target);
      if targetInfo.datenum >= srcFile(iFile).datenum
        fprintf('Skipped:  %s\n', srcFile(iFile).name);
        continue;
      end
    end
    
    % Display the compilation command
    if verbose
      fprintf('mex -outdir %s', outDir);
      fprintf(' %s', options{:});
      fprintf(' %s\n', srcFile(iFile).name);
    end

    % Run compilation
    try
      mex('-outdir', outDir, options{:}, srcFile(iFile).name);
    catch err
      close(hWait);
      fprintf('\n\nSTOPPED due to compilation error.\n\n');
      rethrow(err);
    end
    
    % Generate an m-file if so requested
    if generateMFile
      source        = fileread(srcFile(iFile).name);
      srcDoc        = regexp(source, '^\s*/\*\*\s*(.*?)\n\s*\n(.*?)\*/', 'tokens');
      if isempty(srcDoc)
        warning ( 'ecsCompile:documentation'                                                  ...
                , [ 'No documentation found for MEX program %s. '                             ...
                    'Please add this if possible; see ecsCompile.m for the required syntax.'  ...
                  ]                                                                           ...
                , srcFile(iFile).name                                                         ...
                );
        continue;
      end
      
      [~,target,~]  = fileparts(target);
      targetID      = fopen(fullfile(outDir, [target '.m']), 'w');
      fprintf ( targetID, '%% %s    %s\n%%\n%%%s\n'                     ...
              , upper(target), srcDoc{1}{1}                             ...
              , strrep(srcDoc{1}{2}, sprintf('\n'), sprintf('\n%%'))    ...
              );
      fclose(targetID);
    end
  end
  
  % Cleanup and exit
  close(hWait);

end

%%
function generateEnums(incPath, outPath)

  hWait           = waitbar(0, strrep(incPath, '\', '\\'));
  listing         = dir(incPath);
  
  for iList = 1:numel(listing)
    waitbar(iList / numel(listing), hWait);
    
    % Only process header files
    [~,name,ext]  = fileparts(listing(iList).name);
    if ~strcmpi(ext, '.hpp')
      continue;
    end
    
    % Search for named enums
    source        = fileread(fullfile(incPath, listing(iList).name));
    enums         = regexp(source, '\<enum\s+(\w+)\s*{\s*(.*?)\s*}\s*;', 'tokens');

    for iEnum = 1:numel(enums)
      % Clean up comments
      def         = regexprep(enums{iEnum}{2}, '(//.*?$)|(/\*\*.*?\*/)', '', 'lineanchors');
      createEnumeration(enums{iEnum}{1}, def, outPath);
    end
    
    % Search for printable enums
    enums         = regexp(source, '(?<!#define\s+)\<CV_ENUM\s*\((\w+)\s*,\s*(.*?)\s*)', 'tokens');
    for iEnum = 1:numel(enums)
      createEnumeration(enums{iEnum}{1}, enums{iEnum}{2}, outPath);
    end
  end
  
  close(hWait);
  

  % Recursive call for subdirectories
  for iList = 1:numel(listing)
    if listing(iList).isdir && ~all(listing(iList).name == '.')
      generateEnums ( fullfile(incPath, listing(iList).name)        ...
                    , fullfile(outPath, ['+' listing(iList).name])  ...
                    );
    end
  end
  
end

%%
function createEnumeration(name, contents, outPath)

  % Create enumeration file
  if ~exist(outPath, 'dir')
    mkdir(outPath);
  end
  outFID      = fopen(fullfile(outPath, [name '.m']), 'w');
  fprintf(outFID, 'classdef %s\n  properties (Constant)\n', name);

  def         = regexp(contents, ',', 'split');
  for iDef = 1:numel(def)
    % Parse for enum name and value
    value     = regexp(def{iDef}, '(\w+)\s*(=\s*[-+]*\d+)?', 'tokens', 'once');
    if isempty(value{2})
      value{2}= iDef - 1;
    else
      value{2}= str2double(value{2}(2:end));
    end
    fprintf(outFID, '    %s = %d\n', value{:});
  end

  fprintf(outFID, '  end\nend\n');
  fclose(outFID);
  
end
