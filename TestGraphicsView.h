#ifndef TESTGRAPHICSVIEW_H
#define TESTGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>

class TestGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    TestGraphicsView(QWidget *parent = nullptr);
    void setPixmapItem(const QImage &image);
    void setPixmapItem(QSharedPointer<QImage> ptr);

    void stackRef(const QImage &image);
    void stackRef(QSharedPointer<QImage> ptr);



signals:
    void mouseMoved(const QPointF &pos);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void scrollChange();
    void generateGrid();
public:
    QGraphicsPixmapItem*& TemplateItem(){
        return m_TemplateItem;
    }
    QGraphicsPixmapItem*& RefItem(){
        return m_RefItem;
    }

private:
    QPoint m_RefLastPos;
    QPoint m_MousePressPos;

private:
    QGraphicsPixmapItem *m_TemplateItem = nullptr;
    QGraphicsPixmapItem *m_RefItem = nullptr;
    QGraphicsScene m_GraphicsScene = nullptr;
    QGraphicsPathItem *m_GridItem;
    qreal m_ManualScale = 1;
    double m_AddGridMultiple = 0.0;
};

#endif // TESTGRAPHICSVIEW_H
