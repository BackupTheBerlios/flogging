#pragma once
/*#include <QWidget>
#include "baseplot.h"
#include "sessionstate.h"

QT_FORWARD_DECLARE_CLASS(QMainWindow)
class QwtPlotCurve;

namespace stats {

	enum { e_history_ln = 60 };

	class StatsPlot : public plot::BasePlot
	{
		Q_OBJECT
	public:

		enum E_StatsData
		{
			e_ReadBytes,
			e_max_statsdata_enum_value
		};
	
		StatsPlot (QObject * oparent, QWidget * wparent, SessionState & s, plot::PlotConfig & pcfg, QString const & fname);
		~StatsPlot ();
		virtual void update ();
		//QwtPlotCurve const * getStatsCurve (E_StatsData const id) const { return m_curves[id].m_curve; }

	private:
		SessionState & m_state;
	};


	class StatsWindow : public QObject
	{
		Q_OBJECT
	public:
		StatsWindow (QObject * parent, SessionState & state);
		~StatsWindow ();

		void stopUpdate ();
	
	public slots:

	private:
	
		plot::PlotConfig m_config;
		QMainWindow	* m_window;
		StatsPlot * m_plot;
		SessionState & m_state;
	};

}*/

