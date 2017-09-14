AM_DEFAULT_SOURCE_EXT = .cpp

sbin_PROGRAMS = \
	openpower-proc-control

openpower_proc_control_SOURCES = \
	proc_control.cpp \
	cfam_access.cpp \
	filedescriptor.cpp \
	registration.cpp \
	targeting.cpp \
	openpower_procedures.cpp \
	ext_interface.cpp \
	org/open_power/Proc/CFAM/error.cpp

CLEANFILES = openpower_procedures.cpp

nobase_nodist_include_HEADERS = \
	org/open_power/Proc/CFAM/error.hpp

BUILT_SOURCES = \
	org/open_power/Proc/CFAM/error.cpp \
	org/open_power/Proc/CFAM/error.hpp

openpower_proc_control_LDFLAGS = $(PHOSPHOR_LOGGING_LIBS) \
                                 $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
                                 $(SDBUSPLUS_LIBS) \
                                 -lstdc++fs

openpower_proc_control_CXXFLAGS = $(PHOSPHOR_LOGGING_CFLAGS) \
                                  $(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
                                  $(SDBUSPLUS_CFLAGS)

org/open_power/Proc/CFAM/error.hpp: ${srcdir}/org/open_power/Proc/CFAM.errors.yaml
	@mkdir -p `dirname $@`
	$(SDBUSPLUSPLUS) -r $(srcdir) error exception-header org.open_power.Proc.CFAM > $@

org/open_power/Proc/CFAM/error.cpp: ${srcdir}/org/open_power/Proc/CFAM.errors.yaml
	@mkdir -p `dirname $@`
	$(SDBUSPLUSPLUS) -r $(srcdir) error exception-cpp org.open_power.Proc.CFAM > $@

SUBDIRS = test

-include Makefile.generated
