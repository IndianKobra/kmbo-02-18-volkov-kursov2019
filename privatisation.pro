QT += widgets

HEADERS += \
    graphwidget.h

SOURCES += \
        main.cpp \
        graphwidget.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/elasticnodes
INSTALLS += target
