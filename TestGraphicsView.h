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

protected:
    void wheelEvent(QWheelEvent *event) override;


private:
    void resizeView();
    void scrollChange();

private:
    QGraphicsPixmapItem *m_PixmapItem = nullptr;
    QSharedPointer<QImage> m_Image = nullptr;
    QGraphicsScene *m_GraphicsScene = nullptr;
    qreal m_ManualScale = 1;
};

#endif // TESTGRAPHICSVIEW_H
