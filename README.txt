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

See agExampleScriptMovieIJ.m for a simplified and more tested version of the class
See agExampleScriptXMovie.m for a more advanced and less tested version

author:Andrea Giovannucci