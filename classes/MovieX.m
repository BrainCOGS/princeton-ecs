%% MOVIEX   Movie data structure.
%
%   This class collects various metadata for a given image stack, which can consist of multiple
%   input files. The main constraint is that all frames are required to have the same width and
%   height. 
%
%   Example pipeline:
%       
%
classdef MovieX < handle

  %------- Constants
  properties (Constant)
  end

  %------- Private data
  properties (Access = protected, Transient)
  end
  properties (Access = protected)
  end
  
  %------- Public data
  properties (SetAccess = protected, Transient)
  end
  properties (SetAccess = protected)
    
    width                   % Width of a single frame
    height                  % Height of a single frame
    frames                  % Number of frames
    bitsPerSample           % Number of bits per pixel
    pixelFcn                % Pixel data type conversion function, e.g. @uint16
    data                    % Pixel intensity data as a 3D matrix, or an empty matrix if not loaded
    
    motionCorr              % Motion correction data structure, if loaded
    statistics              % Min, max, and other summary statistics for pixel values
    pixelMasked             % True if the indexed pixel is masked (no data)
    frameMasked             % True if the indexed frame is masked (no data)
    
  end
  
  %________________________________________________________________________
  methods

    %----- Structure version to store an object of this class to disk
    function frozen = saveobj(obj)
      % Use class metadata to determine what properties to save
      metadata      = metaclass(obj);
      
      % Store all mutable and non-transient data
      for iProp = 1:numel(metadata.PropertyList)
        property    = metadata.PropertyList(iProp);
        if ~property.Transient && ~property.Constant
          frozen.(property.Name)  = obj.(property.Name);
        end
      end
      
    end
    
    %----- Constructor
    function obj = MovieX(inputFiles)
      
      obj.data      = [];
      
    end
    
  end
  
  %________________________________________________________________________
  methods (Static)

    %----- Structure conversion to load an object of this class from disk
    function obj = loadobj(frozen)
    end
    
  end
  
end
