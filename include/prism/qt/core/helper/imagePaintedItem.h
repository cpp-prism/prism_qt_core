#ifndef PRISM_QT_CORE_IMAGEPAINTEDITEM_H
#define PRISM_QT_CORE_IMAGEPAINTEDITEM_H

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPixelFormat>
#include <QPixmap>

namespace prism::qt::core {

class imagePaintedItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
public:
    imagePaintedItem(QQuickItem* parent = nullptr);

    QImage image() const;
    void setImage(const QImage &newImage);

signals:

    void imageChanged();
private:
    QImage m_image;

    // QQuickPaintedItem interface
public:
    void paint(QPainter *painter) override;
};

}// namespace prism::qt::core

#endif // PRISM_QT_CORE_IMAGEPAINTEDITEM_H
