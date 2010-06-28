##############################
# metabolism Qt Project File #
##############################

TEMPLATE = app
# RESOURCES = resources.qrc
INCLUDEPATH += ../SFMT

QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
CONFIG += warn_on 
QT += opengl

contains( DEFINES, HAVE_NCURSES ) {
   message( "Building with Qt and ncurses." )
} else {
   message( "Building with Qt and without ncurses." )
}

isEmpty( MACTARGET ) {
   MACTARGET = intel
}

!contains( CONFIG, static ) {
   CONFIG *= shared
}

unix:!macx {
   message( "Generating makefile for Linux systems." )
   INCLUDEPATH += /usr/include/qwt-qt4
   LIBS += -lqwt-qt4
   QMAKE_CFLAGS += -msse2
}

macx {
   INCLUDEPATH += /usr/local/qwt-5.1.1/include
   QMAKE_LIBDIR += /usr/local/qwt-5.1.1/lib
   LIBS += -lqwt
   #ICON = img/nernst.icns

   contains( MACTARGET, intel ) {
      message( "Generating makefile for Intel Macs." )
      QMAKE_CFLAGS += -msse2
      CONFIG += x86
   }

   contains( MACTARGET, ppc ) {
      message( "Generating makefile for PowerPC Macs." )
      CONFIG += ppc

      # if we're building a universal application, disable SSE (because it 
      # won't work on PPC) and set the SDK path to the universal SDK
      contains( MACTARGET, intel ) {
         QMAKE_CFLAGS -= -msse2
         QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk  
      }
   }
}

win32 {
   message( "Generating makefile for Windows." )
   INCLUDEPATH += "C:\Qwt\static\src"
   QMAKE_LIBDIR += "C:\Qwt\static\lib"
   LIBS += -lqwt
   RC_FILE = win32_resources.rc
}

