#include "TestGraphicsView.h"
#include <iostream>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QImage>
#include <QWheelEvent>
#include <QScrollBar>
#include <QMessageBox>

TestGraphicsView::TestGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setStyleSheet("padding: 0px; border: 0px;");//无边框
    setMouseTracking(true);//跟踪鼠标位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏水平条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖条
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);//设置以鼠标为中心点缩放
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);//设置可以拖动

//    m_GraphicsScene = new QGraphicsScene(this);
    m_RefItem = new QGraphicsPixmapItem();
    setScene(&m_GraphicsScene);
    m_GraphicsScene.setBackgroundBrush(Qt::darkGray);
    setSceneRect(0, 0, 600, 600);

}

void TestGraphicsView::setPixmapItem(const QImage &image)
{
    setPixmapItem(QSharedPointer<QImage>(new QImage(image)));
}

void TestGraphicsView::setPixmapItem(QSharedPointer<QImage> ptr)
{
    m_GraphicsScene.clear();
    m_RefItem = nullptr;
    m_GridItem = new QGraphicsPathItem (QPainterPath());
    m_TemplateItem = new QGraphicsPixmapItem();
    m_ManualScale = 1.0;
    m_TemplateItem->setPixmap(QPixmap::fromImage(*ptr));
    m_TemplateItem->setPos(0, 0);
    m_GraphicsScene.addItem(m_TemplateItem);

    auto pixelmapRect = scene()->itemsBoundingRect();
    setSceneRect(pixelmapRect.marginsAdded(QMargins(10,10,10,10)));
    fitInView(pixelmapRect, Qt::KeepAspectRatio);
    generateGrid();
    m_AddGridMultiple = 8 * std::max((double)pixelmapRect.width() / (double)viewport()->width(), (double)pixelmapRect.height() / (double)viewport()->height());
}

void TestGraphicsView::stackRef(const QImage &image)
{
    stackRef(QSharedPointer<QImage>(new QImage(image)));
}

void TestGraphicsView::stackRef(QSharedPointer<QImage> ptr)
{
    if(m_TemplateItem == nullptr){
        QMessageBox::warning(this, "Warning!", "Read template image first");
        return;
    }

    if(m_RefItem != nullptr){
        m_GraphicsScene.removeItem((m_RefItem));
        delete m_RefItem;
    }
    m_RefItem = new QGraphicsPixmapItem();

    m_RefItem->setPixmap(QPixmap::fromImage(*ptr));
    m_RefItem->setOpacity(0.5);
    m_RefItem->setPos(0, 0);
    m_GraphicsScene.addItem(m_RefItem);
}

void TestGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(m_TemplateItem == nullptr) return;
    int wheelDeltaValue = event->angleDelta().y();

    if (wheelDeltaValue > 0)
    {
        m_ManualScale *= 1.2;
        if(m_ManualScale > m_AddGridMultiple){
            m_GraphicsScene.addItem(m_GridItem);
        }
        this->scale(1.2, 1.2);

    }
    else
    {
        if(m_ManualScale < 1) return;
        m_ManualScale = m_ManualScale / 1.2;
        if(m_ManualScale <= m_AddGridMultiple){
            m_GraphicsScene.removeItem(m_GridItem);
        }
        this->scale(1.0 / 1.2, 1.0 / 1.2);

    }
}

void TestGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF viewPos = mapToScene(event->pos());
    emit mouseMoved(viewPos);
    if (event->buttons() & Qt::LeftButton) {
        auto nowPos = QPoint(event->pos().x(), event->pos().y());
        QPoint newPos = m_RefLastPos + (mapToScene(nowPos).toPoint() - m_MousePressPos);
        m_RefItem->setPos(newPos);
    }
    else if(event->buttons() & Qt::RightButton){
        auto newEvent = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseMoveEvent(newEvent);
    }
    else{
        QGraphicsView::mouseMoveEvent(event);
    }
}

void TestGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_RefLastPos = m_RefItem->pos().toPoint();
        m_MousePressPos = mapToScene(event->pos()).toPoint();
    }
    if(event->button() == Qt::RightButton){
        auto newEvent = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mousePressEvent(newEvent);
    }
}

void TestGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_MousePressPos = QPoint();
    }
    if(event->button() == Qt::RightButton){
        auto newEvent = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseReleaseEvent(newEvent);
    }
}

void TestGraphicsView::generateGrid()
{
    // 创建QPainterPath对象并添加多条线段
    QPainterPath path;
    auto pixelmapRect = scene()->sceneRect();
    for (int x = pixelmapRect.x(); x <= pixelmapRect.x() + pixelmapRect.width(); x += 1) {
        path.moveTo(x, 0);
        path.lineTo(x, pixelmapRect.height());
    }

    for (int y = pixelmapRect.y(); y <= pixelmapRect.y() + pixelmapRect.height(); y += 1) {
        path.moveTo(0, y);
        path.lineTo(pixelmapRect.width(), y);
    }

    m_GridItem->setPath(path);
    QPen pen(Qt::black);
    pen.setWidth(0);
    m_GridItem->setPen(pen);
}
