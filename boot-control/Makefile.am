AM_DEFAULT_SOURCE_EXT = .cpp

bin_PROGRAMS = \
        openpower-boot-control

openpower_boot_control_SOURCES = \
        boot_control.cpp \
        boot_control_main.cpp \
        bmc_boot_steps.cpp

openpower_boot_control_LDFLAGS = $(PHOSPHOR_LOGGING_LIBS)

openpower_boot_control_CXXFLAGS = $(PHOSPHOR_LOGGING_CFLAGS)
