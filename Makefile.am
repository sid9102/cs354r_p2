noinst_HEADERS = BaseApplication.h Room.h Sphere.h TutorialApplication.h Physics.h Block.h
bin_PROGRAMS = assignment2
assignment2_CPPFLAGS = -I$(top_srcdir)
assignment2_SOURCES = BaseApplication.cpp Room.cpp Sphere.cpp TutorialApplication.cpp Physics.cpp Block.cpp
assignment2_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) -std=c++11
assignment2_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS)
assignment2_LDFLAGS = -lOgreOverlay -lboost_system

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
