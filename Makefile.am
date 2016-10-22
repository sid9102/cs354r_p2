noinst_HEADERS = BaseApplication.h Room.h Sphere.h TutorialApplication.h Physics.h Paddle.h
bin_PROGRAMS = assignment3
assignment3_CPPFLAGS = -I$(top_srcdir)
assignment3_SOURCES = BaseApplication.cpp Room.cpp Sphere.cpp TutorialApplication.cpp Physics.cpp Paddle.cpp
assignment3_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) $(SDL_CFLAGS) $(SDL_mixer_CFLAGS) -std=c++11
assignment3_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(SDL_LIBS) $(SDL_mixer_LIBS)
assignment3_LDFLAGS = -lOgreOverlay -lboost_system

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
