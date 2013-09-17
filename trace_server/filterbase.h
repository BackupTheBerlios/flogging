#pragma once
#include <QObject>
#include <QWidget>
#include "cmd.h"

enum E_FilterType {
	e_Filter_Mgr = 0,
	e_Filter_Script,
	e_Filter_String,
	e_Filter_Regex,
	e_Filter_Ctx,
	e_Filter_Lvl,
	e_Filter_Tid,
	e_Filter_FileLine,
	e_Filter_User0,
	e_Filter_User1,
	e_Filter_User2,

	e_filtertype_max_value
};

QString const g_filterNames[] = {
	QString("Mgr"),
	QString("Script"),
	QString("String"),
	QString("Regex"),
	QString("Ctx"),
	QString("Level"),
	QString("Tid"),
	QString("FileLn"),
	QString("User0"),
	QString("User1"),
	QString("User2"),
	QString("max")
};



struct FilterBase : public QWidget
{
	bool m_enabled;
	QWidget * m_widget;

	FilterBase (QWidget * parent = 0) : QWidget(parent), m_enabled(true), m_widget(0) { }
	virtual ~FilterBase () { }

	virtual void initUI () = 0;
	virtual void doneUI () = 0;
	QWidget const * ui () const { return m_widget; }
	QWidget * ui () { return m_widget; }

	virtual E_FilterType type () const = 0;

	virtual bool accept (DecodedCommand const & cmd) const = 0;
	void enable (bool state) { m_enabled = state; }
	bool enabled () const { return m_enabled; }

	virtual void loadConfig (QString const & path) = 0;
	virtual void saveConfig (QString const & path) = 0;
	virtual void applyConfig () = 0;

	virtual void clear () = 0;

	Q_OBJECT
};
