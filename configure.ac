AC_PREREQ([2.69])
AC_INIT([openpower-proc-control], [1.0], [https://github.com/openbmc/openpower-proc-control/issues])
AC_LANG([C++])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

# Checks for programs
AC_PROG_CXX
AC_PROG_INSTALL #Checks/sets the install variable to be used
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_17([noext])
AX_APPEND_COMPILE_FLAGS([-Wall -Werror], [CXXFLAGS])

# Checks for library functions
LT_INIT

# Check/set gtest specific functions.
AX_PTHREAD([GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=1"],[GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=0"])
AC_SUBST(GTEST_CPPFLAGS)
AC_ARG_ENABLE([oe-sdk],
    AS_HELP_STRING([--enable-oe-sdk], [Link testcases absolutely against OE SDK so they can be ran within it.])
)
AC_ARG_VAR(OECORE_TARGET_SYSROOT,
    [Path to the OE SDK SYSROOT])
AS_IF([test "x$enable_oe_sdk" == "xyes"],
    AS_IF([test "x$OECORE_TARGET_SYSROOT" == "x"],
          AC_MSG_ERROR([OECORE_TARGET_SYSROOT must be set with --enable-oe-sdk])
    )
    AC_MSG_NOTICE([Enabling OE-SDK at $OECORE_TARGET_SYSROOT])
    [
        testcase_flags="-Wl,-rpath,\${OECORE_TARGET_SYSROOT}/lib"
        testcase_flags="${testcase_flags} -Wl,-rpath,\${OECORE_TARGET_SYSROOT}/usr/lib"
        testcase_flags="${testcase_flags} -Wl,-dynamic-linker,`find \${OECORE_TARGET_SYSROOT}/lib/ld-*.so | sort -r -n | head -n1`"
    ]
    AC_SUBST([OESDK_TESTCASE_FLAGS], [$testcase_flags])
)

#CHIPS can be passed in a from a recipe, or it will default to P9 and openfsi
AC_ARG_VAR(CHIPS, [The list of chips to build the procedures for])
AS_IF([test "x$CHIPS" == "x"], [CHIPS="p9 openfsi"])

AC_CONFIG_FILES([Makefile.generated],
    [${srcdir}/gen_makefile.sh ${srcdir} "$myChips" > Makefile.generated],
    [myChips="$CHIPS"])

AC_ARG_ENABLE([phal-boot],
              [
AS_HELP_STRING([--enable-phal-boot],
               [Turn on pHAL infrastructure boot control application.
                This feature is required chip version.
                Please check the supported chip verion in --with-chip-version under optional
                packages list])
              ])
AC_ARG_WITH([chip-version],
            [
AS_HELP_STRING([--with-chip-version],
               [To use pHAL boot control application chip version is required.
                Supported chip version=p9,p10.
                Ex: --with-chip-version=p10])
            ])
AC_MSG_CHECKING(pHAL boot application is required)
AS_IF([test "x$enable_phal_boot" == "xyes"],
      [
       AC_MSG_RESULT(yes)

       case "$with_chip_version" in
           p9)  {
                 AC_CHECK_LIB([ekb], [p9_start_cbs], [LIBS="-lekb"], [], [])
                 if test x"$ac_cv_lib_ekb_p9_start_cbs" != "xyes" ; then
                        AC_MSG_ERROR([EKB library not found])
                 fi
                } ;;
           p10) {
                 AC_CHECK_LIB([ekb], [p10_start_cbs], [LIBS="-lekb"], [], [])
                 if test x"$ac_cv_lib_ekb_p10_start_cbs" != "xyes" ; then
                        AC_MSG_ERROR([EKB library not found])
                 fi
                } ;;
           *)   AC_MSG_ERROR("Chips version is not given. To use pHAL boot application chip version is required. Please check the supported chip version in --with-chip-version under optional packages list by using ./configure --help");;
       esac
       AC_SUBST([CHIP_VERSION], [$with_chip_version])

       AS_IF([test "$with_chip_version" == "p10"], [P10_CHIP=1], [P10_CHIP=0])
       AC_DEFINE_UNQUOTED([P10_CHIP], [$P10_CHIP], [To check whether chip is p10])
       AC_SUBST([P10_CHIP])

       AX_ABSOLUTE_HEADER([libpdbg.h])
       if test x"$gl_cv_absolute_libpdbg_h" == "x" ; then
               AC_MSG_ERROR([Cannot find libpdbg.h path])
       fi
       INCDIR=$(dirname $gl_cv_absolute_libpdbg_h)
       AC_SUBST([INCDIR])

       AC_CHECK_LIB([pdbg], [pdbg_targets_init], [LIBS="-lpdbg"], [], [])
       if test x"$ac_cv_lib_pdbg_pdbg_targets_init" != "xyes" ; then
               AC_MSG_ERROR([PDBG library not found])
       fi
       TEST=$(dirname $ac_cv_lib_pdbg_pdbg_targets_init)

       AC_CHECK_LIB([atdb], [atdb_blob_open], [LIBS="-latdb"], [], [])
       if test x"$ac_cv_lib_atdb_atdb_blob_open" != "xyes" ; then
               AC_MSG_ERROR([ATDB library not found])
       fi

       AC_CONFIG_FILES([boot-control/Makefile])

       AC_ARG_VAR(ATTRIBUTES_INFO_FILE_PATH, [Attributes information file path])
       AS_IF([test "x$ATTRIBUTES_INFO_FILE_PATH" == "x"],
               [ATTRIBUTES_INFO_FILE_PATH="/usr/share/pdata/attributes.atdb"])
       AC_DEFINE_UNQUOTED([ATTRIBUTES_INFO_FILE_PATH], ["$ATTRIBUTES_INFO_FILE_PATH"],
                          [Attributes information file path])
      ],
      [
       AC_MSG_RESULT(no)
      ]
     )
AM_CONDITIONAL([IS_PHAL_BOOT_REQUIRED], [test "x$enable_phal_boot" == "xyes"])

AC_CHECK_LIB([pdbg], [thread_sreset_all])

PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221])
PKG_CHECK_MODULES([PHOSPHOR_LOGGING], [phosphor-logging])
PKG_CHECK_MODULES([PHOSPHOR_DBUS_INTERFACES], [phosphor-dbus-interfaces])
PKG_CHECK_MODULES([OPENPOWER_DBUS_INTERFACES], [openpower-dbus-interfaces])

PKG_PROG_PKG_CONFIG
AC_ARG_WITH([systemdsystemunitdir],
     [AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files])],,
     [with_systemdsystemunitdir=auto])
AS_IF([test "x$with_systemdsystemunitdir" = "xyes" -o "x$with_systemdsystemunitdir" = "xauto"], [
     def_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)

     AS_IF([test "x$def_systemdsystemunitdir" = "x"],
   [AS_IF([test "x$with_systemdsystemunitdir" = "xyes"],
    [AC_MSG_ERROR([systemd support requested but pkg-config unable to query systemd package])])
    with_systemdsystemunitdir=no],
   [with_systemdsystemunitdir="$def_systemdsystemunitdir"])])
AS_IF([test "x$with_systemdsystemunitdir" != "xno"],
      [AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])])
AC_CONFIG_FILES([Makefile
        test/Makefile])

AC_CONFIG_FILES([pcie-poweroff@.service])
AC_CONFIG_FILES([xyz.openbmc_project.Control.Host.NMI.service])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT
