AC_INIT([lpsdr], [0.1], [indigo@bitglue.com])

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

: ${CXXFLAGS="-g -O2 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Werror -pedantic -std=c++11"}

AC_PROG_CXX

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.8.0])
PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 >= 2.4.0])
PKG_CHECK_MODULES([JACK], [jack])
PKG_CHECK_MODULES([GNURADIO], [gnuradio-runtime])
PKG_CHECK_MODULES([GNURADIO_ANALOG], [gnuradio-analog])
PKG_CHECK_MODULES([GNURADIO_BLOCKS], [gnuradio-blocks])
PKG_CHECK_MODULES([GNURADIO_FFT], [gnuradio-fft])
PKG_CHECK_MODULES([GNURADIO_AUDIO], [gnuradio-audio])
PKG_CHECK_MODULES([GNURADIO_IQBALANCE], [gnuradio-iqbalance])
PKG_CHECK_MODULES([HAMLIB], [hamlib])

AC_ARG_WITH([pigpio],
    [AS_HELP_STRING([--with-pigpio], [support manipulating GPIO on the Raspberry Pi @<:@default=check@:>@])],
    [],
    [with_pigpio=check])

AS_IF([test "x$with_pigpio" != xno],
    [AC_CHECK_LIB([pigpiod_if2], [i2c_open],
        [
            HAVE_PIGPIO=yes
            AC_SUBST([PIGPIO_LIBS], ["-lpigpiod_if2"])
        ],
        [
            HAVE_PIGPIO=no
            if test "x$with_pigpio" != xcheck; then
                AC_MSG_FAILURE([--with-pigpio was given, but test for pigpio failed])
            fi
        ]
    )]
)

AM_CONDITIONAL([USE_PIGPIO], [test "x$HAVE_PIGPIO" = xyes])

AM_INIT_AUTOMAKE([-Wall -Werror])
AC_OUTPUT
