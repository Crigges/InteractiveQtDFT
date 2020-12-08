#pragma once

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class CVLabel : public QLabel
{
    Q_OBJECT
public:
    std::function<void(int, int)> mouseMoveCallback = [](int,int) {};
    void setCallback(std::function<void(int, int)>&&);
    QPixmap pix;

    explicit CVLabel(QWidget* parent = 0);
    virtual int heightForWidth(int width) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;
public slots:
    void setPixmap(const QPixmap&);
    void resizeEvent(QResizeEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
};
