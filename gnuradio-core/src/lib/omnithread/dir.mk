ifeq ($(ThreadSystem),Solaris)
CXXSRCS = solaris.cc
DIR_CPPFLAGS = $(OMNITHREAD_CPPFLAGS)
endif

ifeq ($(ThreadSystem),Posix)
CXXSRCS = posix.cc
DIR_CPPFLAGS = $(OMNITHREAD_CPPFLAGS) $(OMNITHREAD_POSIX_CPPFLAGS)
endif

ifeq ($(ThreadSystem),NT)
CXXSRCS = nt.cc
DIR_CPPFLAGS = $(OMNITHREAD_CPPFLAGS)
MSVC_STATICLIB_CXXNODEBUGFLAGS += -D_WINSTATIC
MSVC_STATICLIB_CXXDEBUGFLAGS += -D_WINSTATIC
MSVC_DLL_CXXNODEBUGFLAGS += -D_OMNITHREAD_DLL
MSVC_DLL_CXXDEBUGFLAGS += -D_OMNITHREAD_DLL
endif

ifeq ($(ThreadSystem),NTPosix)
CXXSRCS = posix.cc
DIR_CPPFLAGS = $(OMNITHREAD_CPPFLAGS)
MSVC_STATICLIB_CXXNODEBUGFLAGS += -D_WINSTATIC
MSVC_STATICLIB_CXXDEBUGFLAGS += -D_WINSTATIC
MSVC_DLL_CXXNODEBUGFLAGS += -D_OMNITHREAD_DLL
MSVC_DLL_CXXDEBUGFLAGS += -D_OMNITHREAD_DLL
endif

ifeq ($(ThreadSystem),Mach)
CXXSRCS = mach.cc
DIR_CPPFLAGS = $(OMNITHREAD_CPPFLAGS)
endif

ifeq ($(ThreadSystem),vxWorks)
CXXSRCS = vxWorks.cc
OBJS = vxWorks.o
DIR_CPPFLAGS = $(OMNITHREAD_CPPFLAGS)
endif

LIB_NAME     := omnithread
LIB_VERSION  := $(OMNITHREAD_VERSION)
LIB_OBJS     := $(CXXSRCS:.cc=.o)
LIB_IMPORTS  := $(OMNITHREAD_PLATFORM_LIB)

all:: mkstatic mkshared

export:: mkstatic mkshared

ifdef INSTALLTARGET
install:: mkstatic mkshared
endif

vers := $(subst ., ,$(LIB_VERSION))
ifeq ($(words $(vers)), 2)
  vers  := _ $(vers)
  major := ""
else
  major := $(word 1, $(vers))
endif

namespec := $(LIB_NAME) $(vers)

##############################################################################
# Build Static library
##############################################################################

ifndef NoStaticLibrary

staticlib := static/$(patsubst %,$(LibNoDebugPattern),$(LIB_NAME)$(major))

mkstatic::
	@(dir=static; $(CreateDir))

mkstatic:: $(staticlib)

$(staticlib): $(patsubst %, static/%, $(LIB_OBJS))
	@$(StaticLinkLibrary)

export:: $(staticlib)
	@$(ExportLibrary)

ifdef INSTALLTARGET
install:: $(staticlib)
	@$(InstallLibrary)
endif

clean::
	$(RM) static/*.o
	$(RM) $(staticlib)

veryclean::
	$(RM) static/*.o
	$(RM) $(staticlib)

else

mkstatic::

endif


##############################################################################
# Build Shared library
##############################################################################
ifdef BuildSharedLibrary

shlib := shared/$(shell $(SharedLibraryFullName) $(namespec))

ifdef Win32Platform
# in case of Win32 lossage:
  imps := $(patsubst $(DLLDebugSearchPattern),$(DLLNoDebugSearchPattern), \
          $(LIB_IMPORTS))
else
  imps := $(LIB_IMPORTS)
endif

mkshared::
	@(dir=shared; $(CreateDir))

mkshared:: $(shlib)

$(shlib): $(patsubst %, shared/%, $(LIB_OBJS))
	@(namespec="$(namespec)" extralibs="$(imps)" nodeffile=1; \
         $(MakeCXXSharedLibrary))

export:: $(shlib)
	@(namespec="$(namespec)"; \
          $(ExportSharedLibrary))

ifdef INSTALLTARGET
install:: $(shlib)
	@(namespec="$(namespec)"; \
          $(InstallSharedLibrary))
endif

clean::
	$(RM) shared/*.o
	(dir=shared; $(CleanSharedLibrary))

veryclean::
	$(RM) shared/*.o
	@(dir=shared; $(CleanSharedLibrary))

else

mkshared::

endif

##############################################################################
# Build debug libraries for Win32
##############################################################################
ifdef Win32Platform

ifdef BuildSharedLibrary

all:: mkstaticdbug mkshareddbug

export:: mkstaticdbug mkshareddbug

else

all:: mkstaticdbug

export:: mkstaticdbug

endif


#####################################################
#      Static debug libraries
#####################################################

dbuglib := debug/$(patsubst %,$(LibDebugPattern),$(LIB_NAME)$(major))

mkstaticdbug::
	@(dir=debug; $(CreateDir))

mkstaticdbug:: $(dbuglib)

$(dbuglib): $(patsubst %, debug/%, $(LIB_OBJS))
	@$(StaticLinkLibrary)

export:: $(dbuglib)
	@$(ExportLibrary)

clean::
	$(RM) debug/*.o
	$(RM) $(dbuglib)

veryclean::
	$(RM) debug/*.o
	$(RM) $(dbuglib)

#####################################################
#      DLL debug libraries
#####################################################

ifdef BuildSharedLibrary

dbugshlib := shareddebug/$(shell $(SharedLibraryDebugFullName) $(namespec))

dbugimps  := $(patsubst $(DLLNoDebugSearchPattern),$(DLLDebugSearchPattern), \
               $(LIB_IMPORTS))

mkshareddbug::
	@(dir=shareddebug; $(CreateDir))

mkshareddbug:: $(dbugshlib)

$(dbugshlib): $(patsubst %, shareddebug/%, $(LIB_OBJS))
	(namespec="$(namespec)" debug=1 extralibs="$(dbugimps)" nodeffile=1; \
         $(MakeCXXSharedLibrary))

export:: $(dbugshlib)
	@(namespec="$(namespec)" debug=1; \
          $(ExportSharedLibrary))

clean::
	$(RM) shareddebug/*.o
	@(dir=shareddebug; $(CleanSharedLibrary))

veryclean::
	$(RM) shareddebug/*.o
	@(dir=shareddebug; $(CleanSharedLibrary))

endif
endif

