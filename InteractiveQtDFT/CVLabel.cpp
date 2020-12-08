#include "CVLabel.h"
//#include <QDebug>

CVLabel::CVLabel(QWidget* parent) :
    QLabel(parent)
{
    this->setMinimumSize(1, 1);
    setScaledContents(false);
}

void CVLabel::setPixmap(const QPixmap& p)
{
    pix = p;
    QLabel::setPixmap(scaledPixmap());
}

int CVLabel::heightForWidth(int width) const
{
    return pix.isNull() ? this->height() : ((qreal)pix.height() * width) / pix.width();
}

QSize CVLabel::sizeHint() const
{
    int w = this->width();
    return QSize(w, heightForWidth(w));
}

QPixmap CVLabel::scaledPixmap() const
{
    return pix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void CVLabel::resizeEvent(QResizeEvent* e)
{
    if (!pix.isNull())
        QLabel::setPixmap(scaledPixmap());
}

void CVLabel::mousePressEvent(QMouseEvent* e) {
    if (e->buttons() == Qt::LeftButton) {
        QPoint pos = e->pos();
        mouseMoveCallback(pos.x(), pos.y());
    }
}

void CVLabel::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() == Qt::LeftButton) {
        QPoint pos = e->pos();
        mouseMoveCallback(pos.x(), pos.y());
    } 
}

void CVLabel::setCallback(std::function<void(int, int)>&& callback) {
    mouseMoveCallback = callback;
}
