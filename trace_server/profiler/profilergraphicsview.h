#pragma once
#include <QGraphicsView>
#include <cstdio>

QT_FORWARD_DECLARE_CLASS(QGraphicsView)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QSpinBox)

namespace profiler {

// taken from
// http://www.qtcentre.org/wiki/index.php?title=QGraphicsView:_Smooth_Panning_and_Zooming#MyGraphicsView.h
class GraphicsView : public QGraphicsView
{
	Q_OBJECT;
public:
	GraphicsView (QSpinBox & fs, QWidget* parent = NULL);

	//Set the current centerpoint in the
	void SetCenter (QPointF const & centerPoint);
	void ForceCenter (QPointF const & center) { CurrentCenterPoint = center; }
 
protected:
	//Holds the current centerpoint for the view, used for panning and zooming
	QPointF CurrentCenterPoint;
	QPointF GetCenter () { return CurrentCenterPoint; }
 
	QPoint LastPanPoint; //From panning the view
 
	virtual void mousePressEvent (QMouseEvent* event);
	virtual void mouseReleaseEvent (QMouseEvent* event);
	virtual void mouseMoveEvent (QMouseEvent* event);
	virtual void wheelEvent (QWheelEvent* event);
	virtual void resizeEvent (QResizeEvent* event);

	QSpinBox & m_frameSpinBox;
};

}

