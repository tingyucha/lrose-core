# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:40 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
###########################################################################
#
# Makefile for grid classes
#
# Terri Betancourt, RAP, NCAR, Boulder, Co, USA, 80307
#
# Jan 1998
#
###########################################################################

include $(LROSE_CORE_DIR)/build/make_include/lrose_make_macros

LOC_INCLUDES = -I../include
LOC_CFLAGS =

TARGET_FILE = ../libeuclid.a
MODULE_TYPE=library

#
# file lists
#

HDRS = \
	$(LROSE_INSTALL_DIR)/include/euclid/FuzzyFcn.hh \
	$(LROSE_INSTALL_DIR)/include/euclid/Grid.hh \
	$(LROSE_INSTALL_DIR)/include/euclid/TypeGrid.hh \
	$(LROSE_INSTALL_DIR)/include/euclid/GridGeom.hh

CPPC_SRCS = \
	Grid.cc \
	GridGeom.cc

#
# general targets
#

include $(LROSE_CORE_DIR)/build/make_include/lrose_make_lib_module_targets

#
# local targets
#

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
