 
#-------------------------------------------------
#
# Project created by WhiZTiM 2013-September-09 11:34:16
#
#-------------------------------------------------

QT       += core gui widgets
#QT       -= declarative
RC_FILE = icon.rc

QMAKE_CXXFLAGS += -std=c++11

TARGET = Duplichase
TEMPLATE = app

#Actually, these are my Non-conventional platform specific dependencies...
#Change it to fit yours... just make sure you link with "libboost_fulesystem" and "libboost_system"
win32 {
    INCLUDEPATH += C:/tim/libs/headers
    LIBS += \
        C:/tim/libs/lib/gcc/boost/lib/libboost_filesystem-mgw48-mt-1_55.a \
        C:/tim/libs/lib/gcc/boost/lib/libboost_system-mgw48-mt-1_55.a
}

!win32 {
    LIBS += \
        -L/usr/lib/boost/lib \
        -lboost_system \
        -lboost_filesystem
}

SOURCES += \
	backend/src/directory_elements/directory_iterator.cpp \
	backend/src/directory_elements/simple_directory_tree.cpp \
	backend/src/duplicatefinder.cpp \
	backend/src/exception.cpp \
	backend/src/fileproperty.cpp \
	backend/src/filereader.cpp \
	backend/src/hashers/md5_hash.cpp \
	backend/src/path.cpp \
	backend/src/pathhelper.cpp \
	backend/src/pathlister.cpp \
	backend/src/recommendation/path_recommender.cpp \
	backend/src/rule_facade/rule_facade.cpp \
	backend/src/rule_factory/dls_filerules.cpp \
	backend/src/rule_factory/extension_aggregator.cpp \
	backend/src/rule_factory/filerules.cpp \
    dlsmainwindow.cpp \
    backend/cmd_main.cpp \
    main.cpp \
    dupscan/dupscansetupwidget.cpp \
    dupscan/dupscanfindwidget.cpp \
    dupscan/dupscanactionwidget.cpp \
    dupscan/dupscanhelper.cpp \
    dupscan/dialogs/aboutdialog.cpp \
    dupscan/modelview/dactionslistmodel.cpp \
    dupscan/logformatter.cpp \
    dupscan/useful_qt_functions.cpp \
    dupscan/useful_functions.cpp \
    dupscan/modelview/dactionslistview.cpp \
    dupscan/modelview/extrafileproperty.cpp \
    dupscan/safe_deletion.cpp \
    dupscan/modelview/ditemcontainer.cpp \
    dupscan/modelview/filepropertyserializer.cpp \
    dupscan/modelview/filepropertystoremanagement.cpp \
    dupscan/modelview/filterwidgets.cpp

HEADERS  += \
	backend/include/container_helpers/fp_holders.hpp \
	backend/include/container_helpers/fp_unordered_set.hpp \
	backend/include/directory_elements/directory_iterator.hpp \
	backend/include/directory_elements/simple_directory_tree.hpp \
	backend/include/duplicatefinder.hpp \
	backend/include/exception.hpp \
	backend/include/fileproperty.hpp \
	backend/include/filereader.hpp \
	backend/include/hashers/md5_hash.hpp \
	backend/include/header_defines.hpp \
	backend/include/path.hpp \
	backend/include/pathhelper.hpp \
	backend/include/pathlister.hpp \
	backend/include/recommendation/path_recommender.hpp \
	backend/include/rule_facade/rule_facade.hpp \
	backend/include/rule_factory/dls_filerules.hpp \
	backend/include/rule_factory/extension_aggregator.hpp \
	backend/include/rule_factory/filerules.hpp \
	backend/include/rule_factory/rule_bag.hpp \
	backend/include/test/testsuite_1.hpp \
	backend/include/test/testsuite_2.hpp \
	backend/include/test/testsuite_filereader_1.hpp \
    dlsmainwindow.hpp \
    dupscan/dupscansetupwidget.hpp \
    dupscan/useful_functions.hpp \
    dupscan/useful_qt_functions.hpp \
    dupscan/dupscanfindwidget.hpp \
    dupscan/dupscanactionwidget.hpp \
    dupscan/dupscanhelper.hpp \
    dupscan/dialogs/aboutdialog.hpp \
    dupscan/modelview/dactionslistmodel.hpp \
    dupscan/modelview/ditem.hpp \
    dupscan/logformatter.hpp \
    dupscan/modelview/dactionslistview.hpp \
    test/fileproperty/sample_fileProperty.hpp \
    dupscan/modelview/extrafileproperty.hpp \
    dupscan/safe_deletion.hpp \
    dglobals.hpp \
    dupscan/modelview/ditemcontainer.hpp \
    dupscan/modelview/filepropertyserializer.hpp \
    dupscan/modelview/filepropertystoremanagement.hpp \
    dupscan/modelview/filterwidgets.hpp

OTHER_FILES += \
    resources/categoryTypeExplanation.txt
    resources/categoryType.txt

FORMS += \
    dlsmainwindow.ui \
    dupscan/dialogs/aboutdialog.ui

RESOURCES += \
    resources/resources.qrc
