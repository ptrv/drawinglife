# drawinglife

## Installation

### openFrameworks
 
* Download openFrameworks [here](http://www.openframeworks.cc/download/)

* On Linux: 
    * `cd openFrameworks\_path/scripts/linux/ubuntu`
    * run `sudo ./install_dependencies.sh`
    * run `sudo ./install_codecs.sh`
    * (optional) run `install_codeblocks.sh`

### other libraries

* On Linux:
    * install libsqlite3-dev : `sudo apt-get install libsqlite3-dev`
    * install GeographicLib:
        * if available in apt repository: `sudo apt-get install libgeographiclib-dev`
        * or with provided package: 
            * `cd libs_mac/geographiclib-1.1`
            * `./configure, make, sudo make install`
            * eventually `sudo ldconfig`

### building drawinglife

* On Linux:
    * make

### running drawinglife

* On Linux:
    * `cd bin`
    * `./drawinglife`
