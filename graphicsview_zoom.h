#ifndef GRAPHICSVIEW_ZOOM_H
#define GRAPHICSVIEW_ZOOM_H

#include <QGraphicsView>
#include <QWheelEvent>

class GraphicsViewZoom : public QGraphicsView {

public:
    explicit GraphicsViewZoom(QWidget *parent = nullptr) : QGraphicsView(parent) {}

protected:
    void wheelEvent(QWheelEvent *event) override {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }
};

#endif // GRAPHICSVIEW_ZOOM_H
