#include "include/prism/qt/core/helper/imagepainteditemqsg.h"
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

namespace prism::qt::core {
imagePaintedItemQSG::imagePaintedItemQSG(QQuickItem* parent):QQuickItem(parent)
{
    setFlag(ItemHasContents,true);

}

QImage imagePaintedItemQSG::image() const
{
    return m_image;
}

void imagePaintedItemQSG::setImage(const QImage &newImage)
{
    if (m_image == newImage)
        return;
    m_image = newImage;
    emit imageChanged();
}

QSGNode *imagePaintedItemQSG::updatePaintNode(QSGNode* oldNode, [[maybe_unused]]UpdatePaintNodeData* newNode)
{
    auto node = dynamic_cast<QSGSimpleTextureNode *>(oldNode);

    if(!node){
        node = new QSGSimpleTextureNode();
    }

    QSGTexture *m_texture = window()->createTextureFromImage(image());
    node->setOwnsTexture(true);
    node->setRect(boundingRect());
    node->markDirty(QSGNode::DirtyForceUpdate);
    node->setTexture(m_texture);

    return node;
}

}//namespace prism::qt::core
