#ifndef PRISM_QT_CORE_HELPER_IMAGEPAINTEDITEMQSG_H
#define PRISM_QT_CORE_HELPER_IMAGEPAINTEDITEMQSG_H

#include <QQuickItem>
#include <QImage>

namespace prism::qt::core {
class imagePaintedItemQSG : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
public:
    imagePaintedItemQSG(QQuickItem* parent = nullptr);

    QImage image() const;
    void setImage(const QImage &newImage);

signals:

    void imageChanged();
private:
    QImage m_image;

    // QQuickItem interface
protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
};

}// namespace prism::qt::core
#endif // PRISM_QT_CORE_HELPER_IMAGEPAINTEDITEMQSG_H
