#include <QtGui/QApplication>
#include <QtGui/QTableWidget.h>
#include "mainwindow.h"
#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
//#	define NOMINMAX
#	include <windows.h>
#endif

struct Application : QApplication
{
	MainWindow * m_main_window;
	Application (int argc, char *argv[])
		: QApplication(argc, argv)
		, m_main_window(0)
	{}

	void setMainWindow (MainWindow * mw) { m_main_window = mw; }

#ifdef WIN32
	bool winEventFilter ( MSG * msg, long * result )
	{
		DWORD const hotkey = VK_SCROLL;
		if (msg->message == WM_HOTKEY)
		{
			if (GetKeyState(hotkey) & 1)
				if (m_main_window)
					m_main_window->onHotkeyShowOrHide();
		}
		return QApplication::winEventFilter(msg, result);
	}
#endif
};

int main(int argc, char *argv[])
{
	Application a(argc, argv);
	MainWindow w;
	w.show();
	a.setMainWindow(&w);
	return a.exec();
}
