### ModelAnalytics
Interactive model driven data exploration tool based on XmdvTool

#### Project Description

This project is a visual analytic tool called model analytics that integrates multiple machine learning models with a plug-and- play style to describe the input data. It allows the analysts to choose the way they prefer to summarize the data. The MaVis framework provides multiple linked analytic spaces for interpretation at different levels. The low level data space handles data binning strategy while the high level model space handles model summarizations (i.e. clusters or trends). This tool supports model analytics that visualize the summarized patterns and compare and contrast them. This framework is shown to provide several novel methods of investigating co-movement patterns of timeseries dataset which is a common interest of medical sciences, finance, business and engineering alike.

#### Dependencies

1. Qt 4.8.1
2. R 2.15.1
3. Rtools 2.15
4. RInside 0.2.10

#### How to build

It is strongly recommended to compile/build all the dependencies from source code using your system default c++ compiler, otherwise there will be linkage errors.

##### Windows

1. Install [R 2.15.1](http://cran.us.r-project.org/) and set enviornment variable R_HOME to the installation directory
2. Install [Qt 4.8.1](https://download.qt.io/archive/qt/4.8/4.8.1/) to your system
3. Download [Rtools 2.15](https://cran.r-project.org/bin/windows/Rtools/) and install it, you may want to use the compiler comes with Rtools as your default compiler for this project. Then rebuild Qt libraries using it.
4. Download [RInside](http://dirk.eddelbuettel.com/code/rinside.html) and follow the [Embedding Qt Example](http://dirk.eddelbuettel.com/blog/2011/03/25#rinside_and_qt) to setup your Qt.
5. Buid this project using Qt Creater

#### Mac

1. Download [R 2.15.1](http://cran.us.r-project.org/) source code and compile it using your system compiler
2. Download [Qt 4.8.1](https://download.qt.io/archive/qt/4.8/4.8.1/) to your system
4. Download [RInside](http://dirk.eddelbuettel.com/code/rinside.html) and follow the [Embedding Qt Example](http://dirk.eddelbuettel.com/blog/2011/03/25#rinside_and_qt) to setup your Qt.
5. Buid the project
