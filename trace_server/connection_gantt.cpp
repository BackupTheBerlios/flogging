#include "connection.h"
#include "constants.h"
#include <tlv_parser/tlv_encoder.h>
#include "cmd.h"
#include "utils.h"
#include "dock.h"
#include "delegates.h"
#include "gantt/ganttview.h"
#include "gantt/ganttwidget.h"
//#include <cstdlib>

DataGantt::DataGantt (Connection * connection, QString const & confname, QStringList const & path)
	: DockedData<e_data_gantt>(connection, confname, path)
{
	qDebug("%s this=0x%08x name=%s", __FUNCTION__, this, confname.toStdString().c_str());
	m_widget = new gantt::GanttWidget(connection, 0, m_config, confname, path);
	//m_widget->setItemDelegate(new SyncedGanttItemDelegate(m_widget));
}

void Connection::onShowGantts ()
{
	/*qDebug("%s", __FUNCTION__);
	for (datagantts_t::iterator it = m_data.get<e_data_gantt>().begin(), ite = m_data.get<e_data_gantt>().end(); it != ite; ++it)
	{
		(*it)->onShow();
		m_main_window->restoreDockWidget((*it)->m_wd);
	}*/
}

void Connection::onHideGantts ()
{
	/*qDebug("%s", __FUNCTION__);
	for (datagantts_t::iterator it = m_data.get<e_data_gantt>().begin(), ite = m_data.get<e_data_gantt>().end(); it != ite; ++it)
	{
		(*it)->onHide();
	}*/
}

void Connection::onShowGanttContextMenu (QPoint const &)
{
	qDebug("%s", __FUNCTION__);
	for (datagantts_t::iterator it = m_data.get<e_data_gantt>().begin(), ite = m_data.get<e_data_gantt>().end(); it != ite; ++it)
	{
		(*it)->widget().onHideContextMenu();
	}
}

datagantts_t::iterator Connection::findOrCreateGantt (QString const & tag)
{
	datagantts_t::iterator it = dataWidgetFactory<e_data_gantt>(tag);
	return it;
}

void Connection::appendGantt (gantt::DecodedData & dd)
{
	//qDebug("appendGantt type=%i tag=%s subtag=%s text=%s", dd.m_type, dd.m_tag.toStdString().c_str(), dd.m_subtag.toStdString().c_str(), dd.m_text.toStdString().c_str());
	datagantts_t::iterator it = findOrCreateGantt(dd.m_tag);
	DataGantt & dp = **it;
	gantt::GanttView * gv = dp.widget().findOrCreateGanttView(dd.m_subtag);
	gv->appendGantt(dd);
}

bool Connection::handleGanttClearCommand (DecodedCommand const & cmd, E_ReceiveMode mode)
{
	QString msg;
	for (size_t i=0, ie=cmd.m_tvs.size(); i < ie; ++i)
	{
		if (cmd.m_tvs[i].m_tag == tlv::tag_msg)
			msg = cmd.m_tvs[i].m_val;
	}

	if (m_main_window->plotState() != e_FtrDisabled)
	{
		QString tag = msg;
		int const slash_pos = tag.lastIndexOf(QChar('/'));
		tag.chop(msg.size() - slash_pos);

		QString subtag = msg;
		subtag.remove(0, slash_pos + 1);

		//dataplots_t::iterator it = m_dataplots.find(tag);
		//if (it != m_dataplots.end())
		//{
		//	(*it)->widget().clearCurveData(subtag);
		//}
	}
	return true;
}

	bool parseCommand (DecodedCommand const & cmd, gantt::DecodedData & dd)
	{
		QString msg;
		QString tid;
		QString time;
		QString fgc;
		QString bgc;
		for (size_t i=0, ie=cmd.m_tvs.size(); i < ie; ++i)
		{
			if (cmd.m_tvs[i].m_tag == tlv::tag_msg)
				msg = cmd.m_tvs[i].m_val;
			else if (cmd.m_tvs[i].m_tag == tlv::tag_ctime)
				time = cmd.m_tvs[i].m_val;
			else if (cmd.m_tvs[i].m_tag == tlv::tag_tid)
				tid = cmd.m_tvs[i].m_val;
			else if (cmd.m_tvs[i].m_tag == tlv::tag_fgc)
				fgc = cmd.m_tvs[i].m_val;
			else if (cmd.m_tvs[i].m_tag == tlv::tag_bgc)
				bgc = cmd.m_tvs[i].m_val;
		}

		QString subtag = msg;
		int const slash_pos0 = subtag.lastIndexOf(QChar('/'));
		subtag.chop(msg.size() - slash_pos0);

		QString tag = subtag;
		int const slash_pos1 = tag.lastIndexOf(QChar('/'));
		tag.chop(tag.size() - slash_pos1);

		subtag.remove(0, slash_pos1 + 1);
		msg.remove(0, slash_pos0 + 1);

		//if (!subtag.contains("Dude"))
		//	return false;

		dd.m_time = time.toULongLong();
		dd.m_ctx = tid.toULongLong();
		dd.m_tag = tag;
		dd.m_subtag = subtag;
		dd.m_text = msg;
		return true;
	}

	bool Connection::handleGanttBgnCommand (DecodedCommand const & cmd, E_ReceiveMode mode)
	{
		if (m_main_window->ganttState() == e_FtrDisabled)
			return true;

		gantt::DecodedData dd;
		if (!parseCommand(cmd, dd))
			return true;
		dd.m_type = gantt::e_GanttBgn;

		//qDebug("+decoded Gantt type=%i tag='%s' subtag='%s' text='%s'", dd.m_type, dd.m_tag.toStdString().c_str(), dd.m_subtag.toStdString().c_str(), dd.m_text.toStdString().c_str());
		appendGantt(dd);
		return true;
	}

	bool Connection::handleGanttEndCommand (DecodedCommand const & cmd, E_ReceiveMode mode)
	{
		if (m_main_window->ganttState() == e_FtrDisabled)
			return true;

		gantt::DecodedData dd;
		if (!parseCommand(cmd, dd))
			return true;
		dd.m_type = gantt::e_GanttEnd;
		//qDebug("+decoded Gantt type=%i tag='%s' subtag='%s' text='%s'", dd.m_type, dd.m_tag.toStdString().c_str(), dd.m_subtag.toStdString().c_str(), dd.m_text.toStdString().c_str());
		appendGantt(dd);
		return true;
	}
	bool Connection::handleGanttFrameBgnCommand (DecodedCommand const & cmd, E_ReceiveMode mode)
	{
		if (m_main_window->ganttState() == e_FtrDisabled)
			return true;

		gantt::DecodedData dd;
		if (!parseCommand(cmd, dd))
			return true;
		dd.m_type = gantt::e_GanttFrameBgn;
		appendGantt(dd);
		return true;

	}
	bool Connection::handleGanttFrameEndCommand (DecodedCommand const & cmd, E_ReceiveMode mode)
	{
		if (m_main_window->ganttState() == e_FtrDisabled)
			return true;

		gantt::DecodedData dd;
		if (!parseCommand(cmd, dd))
			return true;
		dd.m_type = gantt::e_GanttFrameEnd;

		//appendGantt(dd);

		//qDebug("appendGantt type=%i tag=%s subtag=%s text=%s", dd.m_type, dd.m_tag.toStdString().c_str(), dd.m_subtag.toStdString().c_str(), dd.m_text.toStdString().c_str());
		datagantts_t::iterator it = findOrCreateGantt(dd.m_tag);
		DataGantt & dp = **it;
		dp.widget().appendFrameEnd(dd);

		return true;
	}




/*void Connection::requestGanttSynchronization (int sync_group, unsigned long long time)
{
	for (datagantts_t::iterator it = m_data.get<e_data_gantt>().begin(), ite = m_data.get<e_data_gantt>().end(); it != ite; ++it)
	{
		DataGantt * const tbl = *it;
		if (tbl->widget().getConfig().m_sync_group == sync_group)
			tbl->widget().findNearestTimeRow(time);
	}
}

void Connection::requestGanttWheelEventSync (int sync_group, QWheelEvent * ev, QGanttView const * source)
{
	for (datagantts_t::iterator it = m_data.get<e_data_gantt>().begin(), ite = m_data.get<e_data_gantt>().end(); it != ite; ++it)
	{
		DataGantt * const tbl = *it;
		if (tbl->widget().getConfig().m_sync_group == sync_group)
			tbl->widget().requestGanttWheelEventSync(ev, source);
	}
}

void Connection::requestGanttActionSync (int sync_group, unsigned long long t, int cursorAction, Qt::KeyboardModifiers modifiers, QGanttView const * source)
{
	for (datagantts_t::iterator it = m_data.get<e_data_gantt>().begin(), ite = m_data.get<e_data_gantt>().end(); it != ite; ++it)
	{
		DataGantt * const tbl = *it;
		if (tbl->widget().getConfig().m_sync_group == sync_group)
			tbl->widget().requestActionSync(t, cursorAction, modifiers, source);
	}
}*/

