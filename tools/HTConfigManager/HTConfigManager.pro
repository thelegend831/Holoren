######################################################################
# Author: Matus Fedorko (xfedor01@stud.fit.vutbr.cz)
######################################################################

TEMPLATE = app            # program type
TARGET = HTConfigManager  # target application name
DESTDIR = bin             # the destination directory
MOC_DIR = obj             # meta object compiler directory
RCC_DIR = obj             # a directory for compiled resource files
OBJECTS_DIR = obj         # a directory for temporary compile output


#
DEPENDPATH += .

# Additional include directories
INCLUDEPATH += \
  . \
  ./src

# Header filed
HEADERS += \
  src/CConfigViewer.h \
  src/CDFtoHologramEditor.h \
  src/CHoloPropagLargeEditor.h \
  src/CMainWindow.h \
  src/CPathPicker.h \
  src/CImageViewer.h \
  src/CSettingsDlg.h \
  src/CConsoleWidget.h

# Source files
SOURCES += \
  src/CConfigViewer.cpp \
  src/CDFtoHologramEditor.cpp \
  src/CHoloPropagLargeEditor.cpp \
  src/CMainWindow.cpp \
  src/CPathPicker.cpp \
  src/CImageViewer.cpp \
  src/CConsoleWidget.cpp \
  src/CSettingsDlg.cpp \
  src/main.cpp
