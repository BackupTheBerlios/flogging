/**
 * Copyright (C) 2011 Mojmir Svoboda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QComboBox>
#include <QSystemTrayIcon>
#include <tlv_parser/tlv_parser.h>
#include "config.h"
#include "dock.h"
#include "findwidget.h"

namespace Ui {
	class MainWindow;
	class SettingsDialog;
	class HelpDialog;
}

class Server;
class QSystemTrayIcon;
class QAction;
class QMenu;
class QListView;
class QStandardItemModel;
class QLabel;
class SessionState;
class TreeView;
class QTreeView;
class Connection;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow (QWidget * parent, bool quit_delay, bool dump_mode, QString const & log_name, int level);
	~MainWindow ();

	QTabWidget * getTabTrace ();
	QTabWidget const * getTabTrace () const;

	QString getAppDir () const { return m_config.m_appdir; }

	int createAppName (QString const & appname, E_SrcProtocol const proto);
	void addNewApplication (QString const & appname);
	int findAppName (QString const & appname)
	{
		for (int i = 0, ie = m_config.m_app_names.size(); i < ie; ++i)
			if (m_config.m_app_names[i] == appname)
				return i;
		return e_InvalidItem;
	}


/*
 *
*/


	// presets
	int findRegistryPresetName (QString const & name);
	int addRegistryPresetName (QString const & name);
	void saveLayout (QString const & preset_name);
	void loadLayout (QString const & preset_name);
	void saveSession (SessionState const & s, QString const & preset_name) const;
	bool loadSession (SessionState & s, QString const & preset_name);

	void onPresetActivate (Connection * conn, QString const & pname);
	QString getCurrentPresetName () const;
	QString promptAndCreatePresetName (QString const & app_name);
	QString getValidCurrentPresetName ();
	QString matchClosestPresetName (QString const & appname);
	void setPresetAsCurrent (QString const & pname);
	void mentionInPresetHistory (QString const & str);
	void mentionInMultiTabPresetHistory (QString const & str);


	QTreeView const * getDockedWidgetsTreeView () const;

	// global config
	GlobalConfig const & getConfig () const { return m_config; }
	bool dumpModeEnabled () const { return m_config.m_dump_mode; }
	void setLevel (int i);
	unsigned getHotKey () const;
	bool onTopEnabled () const;

	// per connection config
	int getLevel () const; // @TODO: per connection
	bool buffEnabled () const; 
	Qt::CheckState buffState () const;
	float getTimeUnits () const { return m_time_units; }
	int plotState () const;	// @TODO: should be per connection
	int tableState () const;
	int ganttState () const;


	// log widget
	bool dtEnabled () const;
	QList<QColor> const & getThreadColors () const { return m_config.m_thread_colors; }

	// drag and drop
	void changeEvent (QEvent * e);
	void dropEvent (QDropEvent * event);
	void dragEnterEvent (QDragEnterEvent * event);
	bool eventFilter (QObject * o, QEvent * e);
	void keyPressEvent (QKeyEvent * e);
	bool handleTab (QKeyEvent * e);

	//OBS Server const * getServer () const { return m_server; }
	//OBS Server * getServer () { return m_server; }

	DockManager const & dockManager () const { return m_dock_mgr; }
	DockManager & dockManager () { return m_dock_mgr; }
	QString const & dockedName () const { return m_docked_name; }

	void createTailDataStream (QString const & fname);
	void createTailLogStream (QString const & fname, QString const & separator);
	void importDataStream (QString const & fname);
	void copyStorageTo (QString const & filename);
	Connection * findConnectionByName (QString const & app_name);
	Connection * findCurrentConnection ();
	Connection * createNewConnection ();
	void destroyConnection (Connection * connection);

public slots:
	void newConnection (Connection * connection);
	void onHotkeyShowOrHide ();
	void hide ();
	void showNormal ();
	void showMaximized ();
	void onOnTop (int);
	void restoreDockedWidgetGeometry ();
	void onDockRestoreButton ();
	void onDockedWidgetsToolButton ();
	void onDockManagerClosed ();

	void onCloseTab (int idx, QWidget * w);
	void onCloseTab (QWidget * w);
	void onCloseMarkedTabs ();
	void onCloseTabWithIndex (int idx);
	void onCloseCurrentTab ();
	void onPresetActivate ();
	void onPresetActivate (int idx);

	friend class Connection;
private slots:

	void loadState ();
	void storeGeometry ();
	void storeState ();
	void storePresetNames ();
	void timerHit ();
	void onQuit ();
	//void onFocusChanged (QWidget * old, QWidget * now);
	void onQuitReally ();
	void openFiles (QStringList const & list);

	void onFileLoad ();
	void onFileTail ();
	void onLogTail ();
	void tailFiles (QStringList const & list);
	void onFileSave ();
	void onFileExportToCSV ();
	void closeEvent (QCloseEvent *event);
	void iconActivated (QSystemTrayIcon::ActivationReason reason);

	//void onCopyToClipboard ();

	// preset
	void onPresetChanged (int idx);
	void onSaveCurrentState ();
	void onSaveCurrentStateTo (QString const & name);
	void onAddPreset ();
	void onRmCurrentPreset ();

	void onLevelValueChanged (int i);
	void onBufferingStateChanged (int state);
	void onShowHelp ();
	void onTabTraceFocus (int i);
	
	//void onReuseTabChanged (int state);
	//void ondtToolButton ();
	//void onTimeUnitsChanged (int i);
	void onTablesStateChanged (int state);

private:
	void showServerStatus ();
	void loadNetworkSettings ();
	void setupMenuBar ();
	void createActions ();
	void createTrayIcon ();
	void convertBloodyBollockyBuggeryRegistry ();

	typedef std::map<QWidget *, Connection *> connections_t;
	connections_t 		m_connections;

	Ui::MainWindow * 	ui;
	Ui::SettingsDialog * ui_settings;
	QDialog *			m_settings_dialog;

	Ui::HelpDialog * 	m_help;
	GlobalConfig 		m_config;

	QTimer *  			m_timer;
	Server *  			m_server;
	QAction * 			m_minimize_action;
	QAction * 			m_maximize_action;
	QAction * 			m_restore_action;
	QAction * 			m_quit_action;
	QMenu *   			m_tray_menu;
	QSystemTrayIcon * 	m_tray_icon;
	QLabel *			m_status_label;

	// docked widgets
	DockManager 		m_dock_mgr;
	QString				m_docked_name;
	QString 			m_log_name;

	int 				m_start_level; // @TODO: to config?
	float 				m_time_units;
};

#endif // MAINWINDOW_H
