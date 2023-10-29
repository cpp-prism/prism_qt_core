#include "include/prism/qt/core/helper/imagePaintedItem.h"
#include <QPainter>

namespace prism::qt::core{
imagePaintedItem::imagePaintedItem(QQuickItem* parent):QQuickPaintedItem(parent)
{

}

QImage imagePaintedItem::image() const
{
    return m_image;
}

void imagePaintedItem::setImage(const QImage &newImage)
{
    if (m_image == newImage)
        return;
    m_image = newImage;
    emit imageChanged();
}

void imagePaintedItem::paint(QPainter *painter)
{
    painter->drawImage(this->boundingRect(),this->image());
}
}// namespace prism::qt::core
