rem QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
rem QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
rem

set PATH=C:\strawberry\perl\bin;%PATH%
rem configure.bat -debug-and-release -opensource -platform win32-msvc2012 -static -ltcg -no-accessibility -no-openvg -no-gif -no-dbus -no-audio-backend -no-sql-sqlite -no-qml-debug -nomake demos -nomake examples -nomake tests
configure -debug-and-release -opensource -platform win32-msvc2012 -static -ltcg -no-accessibility -no-openvg -no-libtiff -no-libmng -no-gif -no-dbus -no-audio-backend -no-sql-sqlite -no-phonon -no-multimedia -no-webkit -nomake demos -nomake examples -nomake tests
