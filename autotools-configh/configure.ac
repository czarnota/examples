AC_INIT([helloworld-cpp-program], 1.0)
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_HEADERS([config.h])

AC_ARG_VAR(MY_VARIABLE, My variable)

if test -n "$MY_VARIABLE"; then
    AC_DEFINE_UNQUOTED([MY_VARIABLE], [$MY_VARIABLE], ["my variable"])
fi

AC_PROG_CXX
AC_CONFIG_FILES(Makefile)

AC_OUTPUT
