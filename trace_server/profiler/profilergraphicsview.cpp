#include "profilergraphicsview.h"

#include <QtGui>
#ifndef QT_NO_OPENGL
#	include <QtOpenGL>
#endif

#include <qmath.h>

namespace profiler {

GraphicsView::GraphicsView (QSpinBox & fs, QWidget * parent)
	: QGraphicsView(parent)
	, m_frameSpinBox(fs)
{ }

/**
  * Sets the current centerpoint.  Also updates the scene's center point.
  * Unlike centerOn, which has no way of getting the floating point center
  * back, SetCenter() stores the center point.	It also handles the special
  * sidebar case.  This function will claim the centerPoint to sceneRec ie.
  * the centerPoint must be within the sceneRec.
  */
void GraphicsView::SetCenter (QPointF const & centerPoint)
{
	// Get the rectangle of the visible area in scene coords
	QRectF visibleArea = mapToScene(rect()).boundingRect();
	// Get the scene area
	QRectF sceneBounds = sceneRect();

	double const boundX = visibleArea.width() / 2.0;
	double const boundY = visibleArea.height() / 2.0;
	double const boundWidth = sceneBounds.width() - 2.0 * boundX;
	double const boundHeight = sceneBounds.height() - 2.0 * boundY;

	//qDebug("setcenter: x=%f y=%f w=%f h=%f", boundX, boundY, boundWidth, boundHeight);
	// The max boundary that the centerPoint can be to
	QRectF bounds(boundX, boundY, boundWidth, boundHeight);
	if (bounds.contains(centerPoint))
	{
		// We are within the bounds
		CurrentCenterPoint = centerPoint;
	}
	else
	{
		// We need to clamp or use the center of the screen
		if (visibleArea.contains(sceneBounds))
		{
			// Use the center of scene ie. we can see the whole scene
			CurrentCenterPoint = centerPoint;
			//CurrentCenterPoint = sceneBounds.center();
		}
		else
		{
			CurrentCenterPoint = centerPoint;
 
			//We need to clamp the center. The centerPoint is too large
			/*if (centerPoint.x() > bounds.x() + bounds.width()) {
				CurrentCenterPoint.setX(bounds.x() + bounds.width());
			} else if (centerPoint.x() < bounds.x()) {
				CurrentCenterPoint.setX(bounds.x());
			}
 
			if (centerPoint.y() > bounds.y() + bounds.height()) {
				CurrentCenterPoint.setY(bounds.y() + bounds.height());
			} else if (centerPoint.y() < bounds.y()) {
				CurrentCenterPoint.setY(bounds.y());
			}*/
		}
	}
	// Update the scrollbars
	centerOn(CurrentCenterPoint);
}
 
void GraphicsView::mousePressEvent (QMouseEvent * event)
{
	// For panning the view
	LastPanPoint = event->pos();
	setCursor(Qt::ClosedHandCursor);
}
 
void GraphicsView::mouseReleaseEvent (QMouseEvent * event)
{
	setCursor(Qt::OpenHandCursor);
	LastPanPoint = QPoint();
}
 
void GraphicsView::mouseMoveEvent (QMouseEvent * event)
{
	if (!LastPanPoint.isNull())
	{
		//Get how much we panned
		QPointF delta = mapToScene(LastPanPoint) - mapToScene(event->pos());
		LastPanPoint = event->pos();

		QPointF cen = mapToScene(viewport()->rect()).boundingRect().center();
		SetCenter(cen + delta);
 
		//Update the center ie. do the pan
		//SetCenter(GetCenter() + delta);
		//qDebug("new center: %f %f", GetCenter().x(), GetCenter().y()); 
	}
	else
	{
		QGraphicsView::mouseMoveEvent(event);
	}
}
 
/**
  * Zoom the view in and out.
  */
void GraphicsView::wheelEvent (QWheelEvent* event)
{
	bool const shift = event->modifiers() & Qt::SHIFT;

	if (shift)
	{
		m_frameSpinBox.setValue(m_frameSpinBox.value() + event->delta());
	}
	else
	{
		//Get the position of the mouse before scaling, in scene coords
		QPointF pointBeforeScale(mapToScene(event->pos()));

		//Get the original screen centerpoint
		QPointF screenCenter = GetCenter(); //CurrentCenterPoint; //(visRect.center());

		//Scale the view ie. do the zoom
		double scaleFactor = 1.15; //How fast we zoom
		if (event->delta() > 0) {
			//Zoom in
			scale(scaleFactor, scaleFactor);
		} else {
			//Zooming out
			scale(1.0 / scaleFactor, 1.0 / scaleFactor);
		}
	 
		//Get the position after scaling, in scene coords
		QPointF pointAfterScale(mapToScene(event->pos()));
	 
		//Get the offset of how the screen moved
		QPointF offset = pointBeforeScale - pointAfterScale;
	 
		//Adjust to the new center for correct zooming
		QPointF newCenter = screenCenter + offset;
		SetCenter(newCenter);
	}
}
 
/**
  * Need to update the center so there is no jolt in the
  * interaction after resizing the widget.
  */
void GraphicsView::resizeEvent (QResizeEvent * event)
{
	//Get the rectangle of the visible area in scene coords
	QRectF visibleArea = mapToScene(rect()).boundingRect();
	SetCenter(visibleArea.center());
 
	//Call the subclass resize so the scrollbars are updated correctly
	QGraphicsView::resizeEvent(event);
}

}

