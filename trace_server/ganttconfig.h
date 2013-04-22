#pragma once
#include "types.h"

namespace gantt {

	struct GanttViewConfig
	{
		QString m_tag;
		QString m_label;
		QString m_strtimeunits;
		int m_sync_group;
		QColor m_fgcolor;
		QColor m_bgcolor;
		QColor m_symbolcolor;
		int m_symbolsize;
		int m_fontsize;
		float m_timeunits;
		float m_scale;
		bool m_show;
		bool m_auto_scroll;
		bool m_auto_color;

		GanttViewConfig ()
			: m_strtimeunits(QString("ms"))
			, m_fgcolor(Qt::yellow)
			, m_bgcolor(Qt::black)
			, m_symbolcolor(Qt::red)
			, m_symbolsize(6)
			, m_fontsize(10)
			, m_timeunits(0.001f)
			, m_scale(1.0f)
			, m_show(true)
			, m_auto_scroll(true)
			, m_auto_color(true)
		{ }

		template <class ArchiveT>
		void serialize (ArchiveT & ar, unsigned const version)
		{
			ar & boost::serialization::make_nvp("tag", m_tag);
			ar & boost::serialization::make_nvp("label", m_label);
			ar & boost::serialization::make_nvp("strtimeunits", m_strtimeunits);
			ar & boost::serialization::make_nvp("sync_group", m_sync_group);
			ar & boost::serialization::make_nvp("fgcol", m_fgcolor);
			ar & boost::serialization::make_nvp("bgcol", m_bgcolor);
			ar & boost::serialization::make_nvp("symcol", m_symbolcolor);
			ar & boost::serialization::make_nvp("symsize", m_symbolsize);
			ar & boost::serialization::make_nvp("fontsize", m_fontsize);
			ar & boost::serialization::make_nvp("timeunits", m_timeunits);
			ar & boost::serialization::make_nvp("scale", m_scale);
			ar & boost::serialization::make_nvp("show", m_show);
			ar & boost::serialization::make_nvp("autoscroll", m_auto_scroll);
			ar & boost::serialization::make_nvp("autocolor", m_auto_color);
		}

		void setTimeUnits (QString const & unit)
		{
			if (unit == "ms")
				m_timeunits = 0.001f;
			if (unit == "us")
				m_timeunits = 0.000001f;
			if (unit == "s")
				m_timeunits = 1.0f;
			if (unit == "m")
				m_timeunits = 60.0f;
			else
				m_timeunits = 0.001f;
		}

	};

	struct GanttConfig
	{
		QString m_tag;
		QString m_title;
		QList<GanttViewConfig> m_gvcfg;
		int m_timer_delay_ms;
		int m_history_ln;
		bool m_show;

		GanttConfig ()
			: m_tag()
			, m_timer_delay_ms(50)
			, m_history_ln(2048)
			, m_show(true)
		{ }

		GanttConfig (QString const & tag)
			: m_tag(tag)
			, m_timer_delay_ms(50)
			, m_history_ln(2048)
			, m_show(true)
		{ }

		template <class ArchiveT>
		void serialize (ArchiveT & ar, unsigned const version)
		{
			ar & boost::serialization::make_nvp("tag", m_tag);
			ar & boost::serialization::make_nvp("title", m_title);
			ar & boost::serialization::make_nvp("gfcfg", m_gvcfg);
			ar & boost::serialization::make_nvp("timer", m_timer_delay_ms);
			ar & boost::serialization::make_nvp("length", m_history_ln);
			ar & boost::serialization::make_nvp("show", m_show);
		}

		bool findGanttViewConfig (QString const & tag, GanttViewConfig const * & ccfg)
		{
			for (int i = 0, ie = m_gvcfg.size(); i < ie; ++i)
				if (m_gvcfg.at(i).m_tag == tag)
				{
					ccfg = &m_gvcfg.at(i);
					return true;
				}
			return false;
		}
	};

	bool loadConfig (GanttConfig & config, QString const & fname);
	bool saveConfig (GanttConfig const & config, QString const & fname);
}

