dnl $Id: acinclude.m4,v 1.6 2004/08/17 02:03:04 joshk Exp $

AC_DEFUN([AC_SUBST_DIR], [
	ifelse($2,,,$1="[$]$2")
	$1=`(
		test "x$prefix" = xNONE && prefix="$ac_default_prefix"
		test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
		eval echo \""[$]$1"\"
	)`
	AC_SUBST($1)
])

AC_DEFUN([AC_CXX_MT_BROKEN], [
	AC_MSG_CHECKING([if using -MT breaks $CXX])
	echo 'int test_func (void) {}' > conftest.cpp
	$CXX -Wp,-MMD,.confdep,-MTconftest.so -shared conftest.cpp -o conftest.so

	if test -f conftest.so -a `nm conftest.so | grep test_func | wc -l` -ne 0; then
		CXX_MT_BROKEN=no
	else
		CXX_MT_BROKEN=yes
	fi

	AC_MSG_RESULT($CXX_MT_BROKEN)
	AC_SUBST(CXX_MT_BROKEN)

	rm -f conftest.cpp conftest.so .confdep
])

AC_DEFUN([AC_LINKER_DYNAMIC], [
        hold_LDFLAGS=$LDFLAGS
        AC_MSG_CHECKING(how to run the linker dynamically)

        if test "$system" = "Darwin/OS X"; then
          found=yes
          AC_MSG_RESULT(no flags)
        else
          for opt in -Wl,-export-dynamic -rdynamic -Wl,-B,dynamic; do
            LDFLAGS="${hold_LDFLAGS} ${opt}"
            AC_LINK_IFELSE(AC_LANG_PROGRAM([],[int i;]), found=yes, found=no)
            if test "$found" = yes; then
              AC_MSG_RESULT($opt)
              break
            fi
          done
        fi  

        if test "$found" = no; then
          AC_MSG_RESULT([don't know, ignoring])
        fi
])
