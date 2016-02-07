#include "picture_holder.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QAction>
#include <QFileDialog>
#include <iostream>
#include <cstdio>

picture_holder::picture_holder(QWidget *parent) : QLabel(parent)
{
    selectionStarted=false;
}

void picture_holder::paintEvent(QPaintEvent *e)
{

    QLabel::paintEvent(e);
    QPainter painter;
    painter.begin(this);
    painter.setPen(QPen(Qt::black, 1));
    if(selectionStarted){
        painter.drawRect(selectionRect);
    }
}

void picture_holder::mousePressEvent(QMouseEvent *e)
{
    if (e->button()==Qt::RightButton) {

    } else {
        selectionRect.setTopLeft(e->pos());
        selectionRect.setBottomRight(e->pos());
    }

}

void picture_holder::mouseMoveEvent(QMouseEvent *e)
{
    if (selectionStarted){
        selectionRect.setBottomRight(e->pos());
        this->repaint();
    }
}

void picture_holder::mouseReleaseEvent(QMouseEvent *e)
{
    if(selectionStarted){
        this->setPixmap( this->pixmap()->copy(selectionRect));
        adjustSize();
        original = *pixmap();
        scaleFactor = 1 ;
        setCursor(Qt::ArrowCursor);
    }
    selectionStarted=false;

}





