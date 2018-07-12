TPOR: Treatment Planning Optimization Routines
====

BUILD INTRUCTIONS FOR TPOR


BUILDING LIBRARIES

1.) Download the following libraries:
  HDF5: www.hdf5group.org/ftp/HDF5/current/src/hdf5-1.8.12.tar.gz
  h5py: www.h5py.org/dl/h5py-2.2.1.tar.gz
  moab-4.6.2: ftp.mcs.anl.gov/pub/fathom/moab-4.6.2.tar.gz
  boost-1.55.0: http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz/download

2.) Create a directory for building software libraries (e.g. /home/user/software)

3.) Move the tarballs you just downloaded into the software directory

4.) create a directory called hdf5, h5py, moab, boost and archive in the software directory (mkdir dirname)

Build HDF5
5.a) mv the hdf5-1.8.12.tar.gz tarball to the hdf5 directory

5.b) enter the hdf5 directory

5.c) run the command tar -xf hdf5-1.8.12.tar.gz to unpack the tarball

5.d) mv the hdf5-1.8.12.tar.gz to the archive directory so that you don't have to download it again if something goes wrong (mv hdf5-1.8.12.tar.gz ../archive/.)

5.e) create a soft link to the hdf5-1.8.12 directory called src (ln -s hdf5-1.8.12 src)

5.f) create a directory called build (mkdir build)

5.g) enter the build directory

5.h) execute the following command: 
cmake -DCMAKE_INSTALL_PREFIX:PATH=/home/user/software/hdf5 -DCMAKE_BUILD_TYPE:STRING=RELEASE -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DHDF5_ENABLE_Z_LIB_SUPPORT:BOOL=OFF -DHDF5_ENABLE_USING_MEMCHECKER:BOOL=ON -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_EXECS:BOOL=OFF -DBUILD_TESTING:BOOL=ON -DHDF5_BUILD_EXAMPLES:BOOL=ON -DHDF5_BUILD_CPP_LIB:BOOL=ON -DHDF5_BUILD_TOOLS:BOOL=ON -DHDF5_BUILD_HL_LIB:BOOL=ON ../src/

Note: /home/user/software is the absolute path to the software directory that you created
Note: this command configures the build system to build shared libraries.

5.i) execute the following commands if no errors occurred: make, make test, make install

5.j) execute the following commands if no make errors occurred:
cmake -DCMAKE_INSTALL_PREFIX:PATH=/home/user/software/hdf5 -DCMAKE_BUILD_TYPE:STRING=RELEASE -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DHDF5_ENABLE_Z_LIB_SUPPORT:BOOL=OFF -DHDF5_ENABLE_USING_MEMCHECKER:BOOL=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_EXECS:BOOL=OFF -DBUILD_TESTING:BOOL=ON -DHDF5_BUILD_EXAMPLES:BOOL=ON -DHDF5_BUILD_CPP_LIB:BOOL=ON -DHDF5_BUILD_TOOLS:BOOL=ON -DHDF5_BUILD_HL_LIB:BOOL=ON ../src/

Note: this command configures the build system to build static libraries (archives)

5.k) execute the following commands if no errors occurred: make, make install

Note: Do not proceed unless all of these steps are successful!

Configure system to find HDF5 shared libs
5.l) open ~/.bashrc using a text editor (e.g. emacs ~/.bashrc)

5.m) at the end of the file add the following lines:
export LD_LIBRARY_PATH=/home/user/software/hdf5/lib:$LD_LIBRARY_PATH

Note: Shared libraries aren't loaded until a program executes (dynamic loading). The program needs to know where to look when loading shared libraries. There are several defaults that are checked first. The LD_LIBRARY_PATH environment variable allows you to append the locations that are checked for shared libraries.

5.n) execute the following command: exec bash

Note: This command loads in your new environment variables from .bashrc

Build h5py
6.a) mv the h5py-2.2.1.tar.gz to the h5py directory

6.b) enter the h5py directory

6.c) run the command tar -xf h5py-2.2.1.tar.gz to unpack the tarball

6.d) enter the h5py-2.2.1 directory

6.e) execute the following command:
./setup.py build --hdf5=/home/user/software/hdf5

6.f) execute the following command:
./setup.py install --prefix=/home/user/software/h5py

Configure system to find h5py python module
6.g) open ~/.bashrc using a text editor (e.g. emacs ~/.bashrc)

6.h) at the end of the file add the following lines:
export PYTHONPATH=/home/user/software/h5py/lib/python2.7/site-packages:$PYTHONPATH

Note: Python has several default paths that it checks when loading modules. the PYTHONPATH environment variables allows you to append the paths where it looks.

6.i) execute the following command: export bash

Build MOAB
6.j) mv the moab-4.6.2.tar.gz tarball to the moab directory

6.k) enter the moab directory

6.l) run the command tar -xf moab-4.6.2.tar.gz to unpack the tarball

6.m) create a soft link to the moab-4.6.2 directory called src (ln -s moab-4.6.2 src)

6.n) create a directory called build (mkdir build)

6.o) enter the build directory

6.p) execute the following command: 
../src/configure --prefix=/home/user/software/moab --enable-optimize --with-hdf5=/home/user/software/hdf5 --enable-64bit"

Note: MOAB currently uses a different build system called autotools

6.q) execute the following commands if no errors occurred: make, make install

Build boost
7.a) mv the boost_1_55_0.tar.gz tarball to the boost directory

7.b) enter the boost directory

7.c) run the command tar -xf boost_1_55_0.tar.gz to unpack the tarball

7.d) enter the boost_1_55_0 directory

7.e) execute the following command:
./bootstrap.sh --prefix=/home/user/software/boost

7.f) execute the following command:
./b2 install


BUILDING TPOR

1.) in the /home/user/software directory, create a new directory called tpor

2.) enter the tpor directory

3.) execute the following command:
git clone https://github.com/aprobinson/TPOR.git

4.) create a directory called build

5.) enter the build directory

6.) copy the example configuration file from the TPOR repo (cp ../TPOR/scripts/configure.sh .)

7.) open the configure.sh file with a text editor (e.g. emacs configure.sh)

8.) set the MOAB_PREFIX_PATH variable equal to /home/user/software/moab, 
HDF5_PREFIX_PATH variable equal to /home/user/software/hdf5, 
BOOST_PREFIX_PATH variable equal to /home/user/software/boost, 
TPOR_PATH variable equal to /home/user/software/tpor/TPOR,
CMAKE_INSTALL_PREFIX:PATH to /home/user/software/tpor/ 

9.) close the configure.sh file

10.) run the configure.sh file (./configure.sh)

Note: every time you run this fill it will pull down commits to the TPOR repo

11.) run the following commands: make, make test, make install

Configure system to find tpor execs and pytpor python module
12.) open ~/.bashrc using a text editor (e.g. emacs ~/.bashrc)

13.) In the PYTHONPATH variable, add the following after the ":"
/home/user/software/tpor:

Note: the variable should look like
export PYTHONPATH=/home/user/software/h5py/lib/python2.7/site-packages:/home/user/software/tpor:$PYTHONPATH

14.) at the end of the file add the following lines
# Executable Path
export PATH=/home/user/software/tpor/bin:$PATH

Note: when running commands in the terminal there are several default locations that are searched for the executables. The PATH environment variable allows you to add to that paths that are searched. Now, the tpor executables can be run from any directory without creating soft links, aliases or relative paths.

15.) run exec bash.
