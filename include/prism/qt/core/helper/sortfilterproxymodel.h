/**
 *  // qml example
    SortFilterProxyModel {
        id: proxyModel
        source: personModel

        sortRole: "age"
        filterRole: "name"
        filterString: filterField.text
        sortOrder: Qt.AscendingOrder
    }
 */

#ifndef PRISM_QT_COER_HELPER_SORTFILTERPROXYMODEL_H
#define PRISM_QT_COER_HELPER_SORTFILTERPROXYMODEL_H

#include "../hpp/prismModelListProxy.hpp"
#include "../prismQt_core_global.h"

#include <QtCore/qsortfilterproxymodel.h>
#include <QtQml/qjsvalue.h>
#include <QtQml/qqmlparserstatus.h>

// Qt5 和 Qt6 的兼容处理
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QtCore/QRegularExpression>
#else
    #include <QtCore/QRegExp>
#endif

namespace prism::qt::core {

class PRISMQT_CORE_EXPORT SortFilterProxyModel : public QSortFilterProxyModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QObject *source READ source WRITE setSource)

    Q_PROPERTY(QByteArray sortRole READ sortRole WRITE setSortRole)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)

    Q_PROPERTY(QByteArray filterRole READ filterRole WRITE setFilterRole)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString)
    Q_PROPERTY(FilterSyntax filterSyntax READ filterSyntax WRITE setFilterSyntax)

    Q_ENUMS(FilterSyntax)

public:
    explicit SortFilterProxyModel(QObject *parent = nullptr);

    Q_INVOKABLE QVariant getRowData(int index)
    {
        return QVariant::fromValue(
            *reinterpret_cast<prismModelListProxyBase**>(
                this->index(index, 0).internalPointer()));
    }

    Q_INVOKABLE QVariant getRowData(QModelIndex index)
    {
        return QVariant::fromValue(
            *reinterpret_cast<prismModelListProxyBase**>(
                this->index(index.row(), 0).internalPointer()));
    }

    Q_INVOKABLE int transIndex2sourceIndex(int index)
    {
        return this->mapToSource(this->index(index, 0)).row();
    }

    QObject *source() const;
    void setSource(QObject *source);

    QByteArray sortRole() const;
    void setSortRole(const QByteArray &role);

    void setSortOrder(Qt::SortOrder order);

    QByteArray filterRole() const;
    void setFilterRole(const QByteArray &role);

    QString filterString() const;
    void setFilterString(const QString &filter);

    enum FilterSyntax {
        RegExp,
        Wildcard,
        FixedString
    };

    FilterSyntax filterSyntax() const;
    void setFilterSyntax(FilterSyntax syntax);

    int count() const;
    Q_INVOKABLE QJSValue get(int index) const;

    void classBegin() override;
    void componentComplete() override;

signals:
    void countChanged();

public slots:
    void sortEx();

protected:
    int roleKey(const QByteArray &role) const;
    QHash<int, QByteArray> roleNames() const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    bool m_complete;
    QByteArray m_sortRole;
    QByteArray m_filterRole;
};

} // namespace prism::qt::core

#endif // PRISM_QT_COER_HELPER_SORTFILTERPROXYMODEL_H
