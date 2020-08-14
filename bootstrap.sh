#!/bin/sh -xe

AUTOCONF_FILES="Makefile.in aclocal.m4 ar-lib autom4te.cache compile \
        config.guess config.h.in config.sub configure depcomp install-sh \
        ltmain.sh missing *libtool test-driver op-enter-mpreboot@.service.in \
		op-continue-mpreboot@.service.in"

if [ ! -f *mpreboot@.service.in ]; then
    touch op-enter-mpreboot@.service.in
	touch op-continue-mpreboot@.service.in
fi

case $1 in
    clean)
        test -f Makefile && make maintainer-clean
        for file in ${AUTOCONF_FILES}; do
            find -name "$file" | xargs -r rm -rf
        done
        exit 0
        ;;
esac

autoreconf -i
echo 'Run "./configure ${CONFIGURE_FLAGS} && make"'
