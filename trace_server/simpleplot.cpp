#include "simpleplot.h"
#include <QtGui>
#include <QMainWindow>
#include <QColor>
#include "qwt/qwt_plot_curve.h"
#include "qwt/qwt_plot_layout.h"
#include "qwt/qwt_scale_draw.h"
#include "qwt/qwt_scale_widget.h"
#include "qwt/qwt_legend.h"
#include "qwt/qwt_legend_item.h"
#include "qwt/qwt_plot_canvas.h"

namespace plot {

SimplePlot::SimplePlot (QWidget * parent)
	: BasePlot(parent, PlotConfig())
{
}

SimplePlot::~SimplePlot () { qDebug("%s", __FUNCTION__); }

}
