#pragma once
#include <QtGui/qwidget.h>
#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"
#include "plotconfig.h"

class QwtPlotCurve;

namespace plot {

	struct Data {
		std::vector<double> m_data_x;
		std::vector<double> m_data_y;

		Data (size_t ln) { reserve(ln); }

		void clear ()
		{
			m_data_x.clear();
			m_data_y.clear();
		}

		void reserve (size_t const n)
		{
			m_data_x.reserve(n);
			m_data_y.reserve(n);
		}

		void push_back (double x, double y)
		{
			m_data_x.push_back(x);
			m_data_y.push_back(y);
		}
	};

	struct Curve {
		QwtPlotCurve * m_curve;
		Data * m_data;
		CurveConfig & m_config;

		Curve (CurveConfig & curve)
			: m_curve(0)
			, m_data(0)
			, m_config(curve)
		{
			qDebug("%s this=0x%08x", __FUNCTION__, this);
		}

		QwtPlotCurve const * getCurve () const { return m_curve; }
		QwtPlotCurve * getCurve () { return m_curve; }
		
		~Curve ()
		{
			qDebug("%s this=0x%08x", __FUNCTION__, this);
			if (m_curve) {
				m_curve->detach();
				delete m_curve;
				m_curve = 0;
			}
			if (m_data) {
				delete m_data;
				m_data = 0;
			}
		}
	};
}

