JACK_CFLAGS=-I/usr/local/include
JACK_LDLIBS=-L/usr/local/lib -ljack -framework CoreAudio -framework CoreServices -framework AudioUnit
SF_CFLAGS=-I/usr/local/Cellar/libsndfile/1.0.30_1/include -I/usr/local/Cellar/flac/1.3.3/include -I/usr/local/Cellar/libvorbis/1.3.7/include -I/usr/local/Cellar/libogg/1.3.4/include -I/usr/local/Cellar/opus/1.3.1/include/opus
SF_LDLIBS=-L/usr/local/Cellar/libsndfile/1.0.30_1/lib -lsndfile
