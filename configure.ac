AC_INIT([lpsdr], [0.1], [indigo@bitglue.com])

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_PROG_CXX

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.8.0])
PKG_CHECK_MODULES([JACK], [jack])
PKG_CHECK_MODULES([GNURADIO], [gnuradio-runtime])
PKG_CHECK_MODULES([GNURADIO_ANALOG], [gnuradio-analog])
PKG_CHECK_MODULES([GNURADIO_BLOCKS], [gnuradio-blocks])
PKG_CHECK_MODULES([GNURADIO_FFT], [gnuradio-fft])
PKG_CHECK_MODULES([GNURADIO_AUDIO], [gnuradio-audio])
PKG_CHECK_MODULES([GNURADIO_IQBALANCE], [gnuradio-iqbalance])
PKG_CHECK_MODULES([HAMLIB], [hamlib])

AM_INIT_AUTOMAKE([-Wall -Werror])
AC_OUTPUT
