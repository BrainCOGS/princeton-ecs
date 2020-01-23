# princeton-ecs
Originally Andrea Giovannucci's Princeton Extensible Calcium-Imaging Suite

## Dependencies
Visual C++ redistributables for version 2015 or later:
https://www.microsoft.com/en-us/download/details.aspx?id=48145

Or, you can also install the full compiler package.

### Required for compilation on Windows (but not running presupplied MEX programs)
Download the repository:  https://github.com/sakoay/libtiff

Then add these environmental variables to your Matlab `startup.m` (assuming you downloaded the above to `C:\Path\To\libtiff`):
```
setenv('LIBTIFF_DIR', 'C:\Path\To\libtiff');
setenv('ECS_DIR'    , fullfile(rootDir, 'MatLab\princeton-ecs'));
setenv('PATH',  [ getenv('PATH')                                                              ...
                , pathsep, fullfile(getenv('OPENCV_DIR'), 'bin')                              ...
                , pathsep, fullfile(getenv('LIBTIFF_DIR'), 'project', 'libtiff', 'Release')   ...
                ]);
```
