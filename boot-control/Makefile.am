AM_DEFAULT_SOURCE_EXT = .cpp

HWP_INC = -DFAPI2_NO_FFDC \
          -I$(INCDIR)/hwpf/fapi2/include \
          -I$(INCDIR)/hwpf/fapi2/include/attributes \
          -I$(INCDIR)/hwpf/fapi2/include/error_info \
          -I$(INCDIR)/hwpf/fapi2/include/plat \
          -I$(INCDIR)/ekb/hwpf/fapi2/include \
          -I$(INCDIR)/ekb/chips/$(CHIP_VERSION)/procedures/hwp/perv/

bin_PROGRAMS = \
        openpower-boot-control

openpower_boot_control_SOURCES = \
        bmc_boot_steps.cpp \
        boot_control.cpp \
        boot_control_main.cpp \
        util.cpp \
        pdbg.cpp

openpower_boot_control_LDFLAGS = $(PHOSPHOR_LOGGING_LIBS)

openpower_boot_control_CXXFLAGS = $(PHOSPHOR_LOGGING_CFLAGS) \
                                  $(HWP_INC)

openpower_boot_control_LDADD = -lekb -latdb -lpdbg

bmc_boot_steps.cpp :
	ln -sf $(CHIP_VERSION)/bmc_boot_steps.cpp bmc_boot_steps.cpp
	ln -sf $(CHIP_VERSION)/bmc_boot_steps.hpp bmc_boot_steps.hpp

CLEANFILES = bmc_boot_steps.cpp bmc_boot_steps.hpp
