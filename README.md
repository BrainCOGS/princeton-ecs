# princeton-ecs
Originally Andrea Giovannucci's Princeton Extensible Calcium-Imaging Suite

Assuming that you have downloaded this package to the location `C:\Path\To\princeton-ecs`, then add to your Matlab path (e.g. in your `startup.m` file):
```
addpath(genpath('C:\Path\To\princeton-ecs'));
rmpath(genpath(fullfile('C:\Path\To\princeton-ecs', '.git')));
```

## Dependencies
OpenCV 3.0 or higher. For Windows this is available at (follow installation instructions): https://github.com/sakoay/opencv-ecs

Visual C++ redistributables for version 2015 or later:
https://www.microsoft.com/en-us/download/details.aspx?id=48145

Or, you can also install the full compiler package:
https://visualstudio.microsoft.com/downloads/

If so, make sure that the C++ compiler options are checked as part of the installation packages selection process.

### Required for compilation (but not running presupplied MEX programs)
OpenCV 3.0 or higher and libtiff are required for compilation. You should be able to download this for Linux systems using some package installation program like `apt-get`.

On Windows you can obtained precompiled libraries here:
https://github.com/sakoay/opencv-ecs
https://github.com/sakoay/libtiff

Then add these environmental variables to your Matlab `startup.m` (assuming you downloaded the above to `C:\Path\To\libtiff`):
```
setenv('OPENCV_DIR' , 'C:\Path\To\opencv-ecs\install\x64\vc14');
setenv('LIBTIFF_DIR', 'C:\Path\To\libtiff');
setenv('ECS_DIR'    , fullfile(rootDir, 'MatLab\princeton-ecs'));
setenv('PATH',  [ getenv('PATH')                                                              ...
                , pathsep, fullfile(getenv('OPENCV_DIR'), 'bin')                              ...
                , pathsep, fullfile(getenv('LIBTIFF_DIR'), 'project', 'libtiff', 'Release')   ...
                ]);
```

Then to compile, execute `ecsCompile` in your Matlab console.


## Troubleshooting
You can test that this package is correctly setup by executing e.g. `cv.imfinfox`. All of the MEX programs in the `cv` package will print out usage info when run without any arguments.

If you get an error like `Invalid MEX-file ... : The specified module could not be found.`, this likely means that one or more dependent libraries could not be found in your system search path. You can check this in a Matlab session by executing `getenv('PATH')`. 

Other potential errors include the precompiled MEX files not being compatible with e.g. the C++ libraries on your system. In the worst case scenario, you may have to compile the package specifically for your system (see above). 
