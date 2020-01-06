AM_DEFAULT_SOURCE_EXT = .cpp

systemdsystemunit_DATA = \
  pcie-poweroff@.service \
  xyz.openbmc_project.Control.Host.NMI.service

bin_PROGRAMS = \
	openpower-proc-control \
	openpower-proc-nmi

openpower_proc_control_SOURCES = \
	proc_control.cpp \
	cfam_access.cpp \
	filedescriptor.cpp \
	registration.cpp \
	targeting.cpp \
	openpower_procedures.cpp \
	ext_interface.cpp

if IPL_LIB_BUILD 
openpower_proc_control_SOURCES += \
    create_pel.cpp
endif

openpower_proc_nmi_SOURCES = \
	nmi_main.cpp \
	nmi_interface.cpp

CLEANFILES = openpower_procedures.cpp

openpower_proc_control_LDFLAGS = $(PHOSPHOR_LOGGING_LIBS) \
                                 $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
                                 $(SDBUSPLUS_LIBS) \
                                 -lstdc++fs

if IPL_LIB_BUILD 
openpower_proc_control_LDFLAGS += -lipl
endif

openpower_proc_control_CXXFLAGS = $(PHOSPHOR_LOGGING_CFLAGS) \
                                  $(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
                                  $(SDBUSPLUS_CFLAGS) \
                                  $(EKB_CXXFLAGS)

if IPL_LIB_BUILD
openpower_proc_control_CXXFLAGS += $(EKB_CXXFLAGS)
endif
 
openpower_proc_nmi_LDFLAGS = $(PHOSPHOR_LOGGING_LIBS) \
                             $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
                             $(SDBUSPLUS_LIBS)

openpower_proc_nmi_CXXFLAGS = $(PHOSPHOR_LOGGING_CFLAGS) \
                              $(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
                              $(SDBUSPLUS_CFLAGS)

SUBDIRS = test

-include Makefile.generated
