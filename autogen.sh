#!/bin/sh

PKG=thinksynth

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have autoconf installed to compile $PKG";
	echo;
	exit;
}


echo "Generating configuration files for $PKG, please wait ..."
echo "Ignore all non-fatal warnings ..."
echo;

if test -d /usr/local/share/aclocal ; then
	ACLOCAL_FLAGS="$ACLOCAL_FLAGS -I /usr/local/share/aclocal"
fi

aclocal $ACLOCAL_FLAGS || exit;
autoheader || exit;
autoconf || exit;
if [ "$1" != "--no-configure" ]; then ./configure $@; fi

