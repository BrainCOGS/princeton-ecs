function ecsCompile(varargin)
% ecsCompile    MEX compilation utility for the Princeton ECS package.
%
% This script will compile all MEX files in the src subdirectory (relative
% to the directory in which ecsCompile.m is located). It depends on the
% mexopencv (https://github.com/kyamagu/mexopencv) package for interfacing
% OpenCV to Matlab; it is required to have performed the OpenCV and
% mexopencv installation procedures before invoking this command.
%
% Users will typically only have to call ecsCompile() without arguments.
% Arguments can be provided, in which case they will be passed on to the
% mex() calls verbatim.
%
% ======================
%  Note for developers
% ======================
% 
% To add a MEX program to the compilation list, simply put it in the src
% directory. ecsCompile() will attempt to guess whether or not a given MEX
% program depends on OpenCV, by detecting either a mexopencv.hpp include
% statement, or use of the cv namespace (e.g. "cv::Mat"), or use of an
% OpenCV constant (e.g. "CV_8U"). If neither of these are found, it will
% compile the MEX program without linking OpenCV libraries. MEX programs
% that use OpenCV will be located in the +cv directory, and those without
% will be located in the +ecs directory.
%
% Code in the following subdirectories are treated in special ways:
%   src/lib       C++ code that should be compiled into shared object
%                 libraries instead of standalone programs. These object
%                 files will be linked to all MEX programs (OpenCV or not).
%   src/cvlib     C++ code that should be compiled into shared object
%                 libraries instead of standalone programs. These object
%                 files will be linked to all OpenCV MEX programs.
%   src/private   MEX programs here will be located in the appropriate
%                 private subdirectories, i.e. not directly accessible to
%                 users.


%===================================================================================================
%   Installation check and dependency lists
%===================================================================================================

%----------  Princeton ECS installation
PRINCETONECS    = fileparts(mfilename('fullpath'));
ECS_SRC         = fullfile(PRINCETONECS, 'src');
ECS_PRIVATE     = fullfile(ECS_SRC     , 'private');
ECS_LIB         = fullfile(ECS_SRC     , 'lib');
ECS_CVLIB       = fullfile(ECS_SRC     , 'cvlib');
MEX_CV          = fullfile(PRINCETONECS, '+cv');
MEX_ECS         = fullfile(PRINCETONECS, '+ecs');

currentDir      = pwd();
cleanup         = onCleanup(@() cd(currentDir));


%----------  OpenCV installation
try
  OPENCV        = getenv('OPENCV_DIR');
catch err
  fprintf('Error encountered while trying to detect OpenCV installation:\n  %s    %s\n', err.identifier, err.message);
  fprintf('Make sure that you have OpenCV installed and the OPENCV_DIR environmental variable set.\n');
  fprintf('For example it could look like "C:\OpenCV\install\x64\vc11" in a 64-bit Windows machine.\n');
  return;
end

OPENCV_INC      = fullfile(fileparts(fileparts(OPENCV)), 'include');
if ~exist(OPENCV_INC, 'dir')
  error('ecsCompile:opencv', 'OpenCV headers directory %s not found.', OPENCV_INC);
end

OPENCV_LIB      = fullfile(OPENCV, 'lib');
cvLibs          = dir( fullfile(OPENCV_LIB,'opencv_*d.lib') );
[~, cvLibs]     = cellfun(@fileparts, {cvLibs.name}, 'UniformOutput', false);
cvLibs          = strcat('-l', cvLibs);
if isempty(cvLibs)
  error('ecsCompile:opencv', 'OpenCV libraries not found in %s.', OPENCV_LIB);
end

%----------  mexopencv installation
try
  MEXOPENCV     = mexopencv.root();
catch err
  fprintf('Error encountered while trying to detect mexopencv installation:\n  %s    %s\n', err.identifier, err.message);
  fprintf('Make sure that you have mexopencv installed and in the Matlab path.\n');
  fprintf('For example mexopencv.root() should return the directory in which it was installed.\n');
  return;
end

MEXOPENCV_INC   = fullfile(MEXOPENCV, 'include');
if ~exist(MEXOPENCV_INC, 'dir')
  error('ecsCompile:mexopencv', 'mexopencv headers directory %s not found.', MEXOPENCV_INC);
end

MEXOPENCV_OBJ   = fullfile(MEXOPENCV, 'lib');
cvObjs          = dir(fullfile(MEXOPENCV_OBJ, '*.obj'));
cvObjs          = fullfile(MEXOPENCV_OBJ, {cvObjs.name});
if isempty(cvObjs)
  error('ecsCompile:mexopencv', 'mexopencv object files not found in %s.', MEXOPENCV_OBJ);
end


%----------  Global code compilation options
ecsOpts       = varargin;

%----------  Additional OpenCV code compilation options
cvOpts        = [ { '-largeArrayDims'                     ...
                  , sprintf('-I"%s"', OPENCV_INC)         ...
                  , sprintf('-I"%s"', MEXOPENCV_INC)      ...
                  , sprintf('-L"%s"', OPENCV_LIB)         ...
                  }                                       ...
                , cvLibs                                  ...
                , cvObjs                                  ...
                , varargin                                ...
                ];


%===================================================================================================
%   Compilation procedure
%===================================================================================================

% Compile common object files
ecsObjs       = doCompile(ECS_LIB, ECS_LIB, 'obj', [{'-c'} ecsOpts]);
ecsOpts       = [ecsOpts, ecsObjs];

% Compile OpenCV specific object files
cvObjs        = doCompile(ECS_CVLIB, ECS_CVLIB, 'obj', [{'-c'} cvOpts]);
cvOpts        = [cvOpts, cvObjs];





end

function outFile = doCompile(srcDir, outDir, outExt, options, lazy, verbose)

  % Default arguments
  if nargin < 5
    lazy            = true;
  end
  if nargin < 6
    verbose         = true;
  end
  
  % Get list of files to compile
  cd(srcDir);
  srcFile           = dir('*.cpp');
  outFile           = cell(size(srcFile));
  if isempty(srcFile)
    return;
  end
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
      srcInfo       = dir(srcFile(iFile).name);
      targetInfo    = dir(target);
      if targetInfo.datenum >= srcInfo.datenum
        fprintf('Skipping %s\n', srcFile(iFile).name);
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
  end
  
  % Cleanup and exit
  close(hWait);

end
