AUTOMAKE_OPTIONS = subdir-objects

noinst_LIBRARIES = liblpsdr.a
check_PROGRAMS = catch2
bin_PROGRAMS = lpsdr

gresource_files = \
    iq_audio.glade \
    mode/ssb.glade \
    test_rig.glade \
    ui.glade

BUILT_SOURCES = gresources.cc gresources.h

gresources.cc: gresources.xml $(gresource_files)
	$(AM_V_GEN) glib-compile-resources --manual-register --target=$@ --sourcedir=$(srcdir) --generate-source $<

gresources.h: gresources.xml $(gresource_files)
	$(AM_V_GEN) glib-compile-resources --manual-register --target=$@ --sourcedir=$(srcdir) --generate-header $<

liblpsdr_a_SOURCES = \
    $(gresource_files) \
    applicationcontroller.cc \
    applicationcontroller.h \
    dispatcher_sink.cc \
    dispatcher_sink.h \
    flowgraph.cc \
    flowgraph.h \
    freq_db.cc \
    freq_db.h \
    gresources.cc \
    gresources.h \
    gresources.xml \
    gr_lock.h \
    mode/mode.h \
    mode/ssb.cc \
    mode/ssb.h \
    rig/iq_audio_source.cc \
    rig/iq_audio_source.h \
    rig/iqonly.cc \
    rig/iqonly.h \
    rig/rig.h \
    rig/softrock.cc \
    rig/softrock.h \
    rig/test.cc \
    rig/test.h \
    rig/test_source.cc \
    rig/test_source.h \
    waterfall.cc \
    waterfall.h

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
catch2_SOURCES = catch.hpp catch2_main.cc
catch2_LDADD = $(lpsdr_LDADD)
TESTS = catch2
