AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = lpsdr
lpsdr_SOURCES = \
    applicationcontroller.cc \
    dispatcher_sink.cc \
    flowgraph.cc \
    main.cc \
    freq_db.cc \
    waterfall.cc \
    mode/ssb.cc \
    rig/test.cc \
    rig/iq_audio_source.cc \
    rig/softrock.cc \
    rig/iqonly.cc \
    rig/test_source.cc

lpsdr_CXXFLAGS = @GTKMM_CFLAGS@ @GLIBMM_CFLAGS@ @JACK_CFLAGS@ @GNURADIO_CFLAGS@ @GNURADIO_FILTER_CFLAGS@ @GNURADIO_ANALOG_CFLAGS@ @GNURADIO_BLOCKS_CFLAGS@ @GNURADIO_FFT_CFLAGS@ @GNURADIO_AUDIO_CFLAGS@ @GNURADIO_IQBALANCE_CFLAGS@ @HAMLIB_CFLAGS@
lpsdr_LDFLAGS = @GTKMM_LIBS@ @GLIBMM_CFLAGS@ @JACK_LIBS@ @GNURADIO_LIBS@ @GNURADIO_FILTER_LIBS@ @GNURADIO_ANALOG_LIBS@ @GNURADIO_BLOCKS_LIBS@ @GNURADIO_FFT_LIBS@ @GNURADIO_AUDIO_LIBS@ @GNURADIO_IQBALANCE_LIBS@ @HAMLIB_LIBS@ -lboost_system -lhamlib++

if USE_PIGPIO
lpsdr_SOURCES += rig/breadboard.cc
lpsdr_LDFLAGS += @PIGPIO_LIBS@
endif
