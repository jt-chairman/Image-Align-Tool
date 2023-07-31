#include "TestGraphicsView.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QImage>
#include <QWheelEvent>
#include <QScrollBar>

TestGraphicsView::TestGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setStyleSheet("padding: 0px; border: 0px;");//无边框
    setMouseTracking(true);//跟踪鼠标位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏水平条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖条
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);//设置以鼠标为中心点缩放
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);//设置可以拖动

    m_GraphicsScene = new QGraphicsScene(this);
    setScene(m_GraphicsScene);
    m_GraphicsScene->setBackgroundBrush(Qt::darkGray);
    setSceneRect(-300, -300, 600, 600);


}

void TestGraphicsView::setPixmapItem(const QImage &image)
{
    setPixmapItem(QSharedPointer<QImage>(new QImage(image)));
}

void TestGraphicsView::setPixmapItem(QSharedPointer<QImage> ptr)
{
    m_Image = ptr;
    setSceneRect(-m_Image->width() / 2 - 50, -m_Image->height() / 2 - 50, m_Image->width() + 100, m_Image->height() + 100);
    if(nullptr != m_PixmapItem)
    {
        m_GraphicsScene->removeItem(m_PixmapItem);
        delete m_PixmapItem;
        m_PixmapItem = nullptr;
    }
    m_PixmapItem = new QGraphicsPixmapItem();
    m_PixmapItem->setPixmap(QPixmap::fromImage(*m_Image));
    m_GraphicsScene->addItem(m_PixmapItem);

    m_PixmapItem->setPos(-m_Image->width() / 2, -m_Image->height() / 2);

    resizeView();
}

void TestGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(m_PixmapItem == nullptr) return;
    int wheelDeltaValue = event->angleDelta().y();
    if (wheelDeltaValue > 0)
    {
        //qDebug() << (double)m_Image->height() << m_ManualScale << ((double)m_Image->height() / m_ManualScale);
        //if((double)m_PCBImage->height() / m_ManualScale < 3)  return;
        this->scale(1.2, 1.2);
        m_ManualScale *= 1.2;
//        Q_EMIT sigScale(true);
        //Q_EMIT sigScrollChange(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value());
    }
    else
    {
        //if(m_ManualScale < 0.5) return;
        this->scale(1.0 / 1.2, 1.0 / 1.2);
        m_ManualScale = m_ManualScale * 1.0 / 1.2;
//        Q_EMIT sigScale(false);
//        Q_EMIT sigScrollChange(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value());
    }
//    scrollChange();
}


void TestGraphicsView::resizeView()
{
    QRectF itemRect = scene()->itemsBoundingRect();
    QPointF centerMapToScene = mapToScene(QPoint(this->rect().width() / 2, this->rect().height() / 2));

    qreal width = itemRect.width() / m_ManualScale;
    qreal height = itemRect.height() / m_ManualScale;
    QRectF currentRect = QRectF(centerMapToScene.x() - width / 2, centerMapToScene.y() - height / 2, width, height);

    if(m_ManualScale == 1)
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    else
        fitInView(currentRect, Qt::KeepAspectRatio);
}


