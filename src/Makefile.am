AUTOMAKE_OPTIONS = subdir-objects

noinst_LIBRARIES = liblpsdr.a
check_PROGRAMS = catch2
bin_PROGRAMS = lpsdr

liblpsdr_a_SOURCES = \
    applicationcontroller.cc \
    dispatcher_sink.cc \
    flowgraph.cc \
    freq_db.cc \
    waterfall.cc \
    mode/ssb.cc \
    rig/test.cc \
    rig/iq_audio_source.cc \
    rig/softrock.cc \
    rig/iqonly.cc \
    rig/test_source.cc

AM_CPPFLAGS = \
    @GTKMM_CFLAGS@ \
    @GLIBMM_CFLAGS@ \
    @JACK_CFLAGS@ \
    @GNURADIO_CFLAGS@ \
    @GNURADIO_FILTER_CFLAGS@ \
    @GNURADIO_ANALOG_CFLAGS@ \
    @GNURADIO_BLOCKS_CFLAGS@ \
    @GNURADIO_FFT_CFLAGS@ \
    @GNURADIO_AUDIO_CFLAGS@ \
    @GNURADIO_IQBALANCE_CFLAGS@ \
    @HAMLIB_CFLAGS@

lpsdr_LDADD = \
    liblpsdr.a \
    @GTKMM_LIBS@ \
    @GLIBMM_CFLAGS@ \
    @JACK_LIBS@ \
    @GNURADIO_LIBS@ \
    @GNURADIO_FILTER_LIBS@ \
    @GNURADIO_ANALOG_LIBS@ \
    @GNURADIO_BLOCKS_LIBS@ \
    @GNURADIO_FFT_LIBS@ \
    @GNURADIO_AUDIO_LIBS@ \
    @GNURADIO_IQBALANCE_LIBS@ \
    @HAMLIB_LIBS@ \
    -lboost_system \
    -lhamlib++

if USE_PIGPIO
liblpsdr_a_SOURCES += rig/breadboard.cc
lpsdr_LDADD += @PIGPIO_LIBS@
endif

lpsdr_SOURCES = main.cc
catch2_SOURCES = catch2_main.cc
catch2_LDADD = $(lpsdr_LDADD)
TESTS = catch2
