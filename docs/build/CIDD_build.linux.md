# The CIDD build and run-time environment - LINUX

We build CIDD using AUTOMAKE and CONFIGURE

CIDD must be built as a 32-bit application.

And to run CIDD, you must have the required 32-bit packages installed.

## Required LINUX packages for the CIDD build

See: [lrose_package_dependencies.cidd.md](./lrose_package_dependencies.cidd.md)

## Choose your install directory (prefix)

The CIDD display uses the xview library, which must be compiled with 32-bit emulation.

**It is important** to keep the **cidd** build/install separate from the main build,
so that you do not mix 32-bit object files with 64-bit executables.

So, for a CIDD build we recommend you use a separate prefix location, for example:

  `$HOME/cidd`

## Check out LROSE

```
  git clone https://github.com/NCAR/lrose-core
```

## Run the CIDD checkout and build

You can run the CIDD build from within the ```lrose-core/build``` directory.

```
  cd lrose-core/build  
  ./build_cidd.py
```

```build_cidd``` is a wrapper script which runs the following:


```
  ./checkout_and_build_auto.py --package lrose-cidd --prefix $HOME/cidd --buildNetcdf
```

CIDD is always built with static linking. The default build location is:

```
  /tmp/cidd_build
```

and the logs will be in:

```
  /tmp/cidd_build/logs
```

## Build output and log files

The build process creates log files as it proceeds. You should see the following messages during the build:

```
========================= git-checkout =========================
Running cmd: git clone --branch master https://github.com/NCAR/lrose-core
Log file is: /tmp/cidd_build/logs/git-checkout.log
    ....
    done
Running cmd: git clone https://github.com/NCAR/lrose-netcdf
Log file is: /tmp/cidd_build/logs/git-checkout.log
    ....
    done
Running cmd: git clone https://github.com/NCAR/lrose-displays
Log file is: /tmp/cidd_build/logs/git-checkout.log
    ....
    done
========================= install-package-makefiles =========================
Running cmd: ./make_bin/installPackageMakefiles.py --package lrose-cidd --codedir .
Log file is: /tmp/cidd_build/logs/install-package-makefiles.log
    ....
    done
========================= setup-autoconf =========================
Running cmd: ./make_bin/createConfigure.am.py --dir . --baseName configure.base --pkg lrose-cidd --debug 
Log file is: /tmp/cidd_build/logs/setup-autoconf.log
    ....
    done
========================= create-qt-moc-files =========================
========================= build-netcdf =========================
Running cmd: ./build_and_install_netcdf.m32 -x /tmp/cidd_build/scratch
Log file is: /tmp/cidd_build/logs/build-netcdf.log
    ....
    done
========================= print-environment =========================
Running cmd: env
Log file is: /tmp/cidd_build/logs/print-environment.log
    ....
    done
========================= run-configure =========================
Running cmd: ./configure --with-hdf5=/tmp/cidd_build/scratch --with-netcdf=/tmp/cidd_build/scratch --prefix=/tmp/cidd_build/scratch
Log file is: /tmp/cidd_build/logs/run-configure.log
    ....
    done
========================= build-libs =========================
Running cmd: make -k -j 8
Log file is: /tmp/cidd_build/logs/build-libs.log
    ....
    done
========================= install-libs-to-tmp =========================
Running cmd: make -k install-strip
Log file is: /tmp/cidd_build/logs/install-libs-to-tmp.log
    ....
    done
========================= build-apps =========================
Running cmd: make -k -j 8
Log file is: /tmp/cidd_build/logs/build-apps.log
    ....
    done
========================= install-apps-to-tmp =========================
Running cmd: make -k install-strip
Log file is: /tmp/cidd_build/logs/install-apps-to-tmp.log
    ....
    done
========================= do-final-install =========================
Running cmd: rsync -av LICENSE.txt /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av release_notes /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av docs /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av ./codebase/apps/cidd/src/CIDD/scripts /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av color_scales /tmp/cidd/share
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av bin /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av lib /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
Running cmd: rsync -av include /tmp/cidd
Log file is: /tmp/cidd_build/logs/do-final-install.log
    ....
    done
============= Checking libs for lrose-cidd =============
Running cmd: ./codebase/make_bin/check_libs.py --listPath ./build/checklists/libs_check_list.lrose-cidd --libDir /tmp/cidd/lib --label lrose-cidd --maxAge 3600
=================>> SUCCESS <<===================
=========>> ALL lrose-cidd LIBS INSTALLED <<========
=================================================
    done
====================================================
============= Checking apps for lrose-cidd =============
Running cmd: ./codebase/make_bin/check_apps.py --listPath ./build/checklists/apps_check_list.lrose-cidd --appDir /tmp/cidd/bin --label lrose-cidd --maxAge 3600
================>> SUCCESS <<==================
=========>> ALL lrose-cidd APPS INSTALLED <<========
===============================================
    done
====================================================

```

If problems occur, you should view the last log file to appear in the printout, because that is where the error is most likely to have occurred.

## Install directory

That build should install CIDD in:

```
  $HOME/cidd/bin
```

## Running CIDD

If you perform the build as above you will need to add:

```
  $HOME/cidd/bin
```

to your path.

As a test, run:

```
  CIDD -font fixed -p http://front.eol.ucar.edu/displayParams/CIDD.pecan
```






