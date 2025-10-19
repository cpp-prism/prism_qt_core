#include "include/prism/qt/core/helper/sortfilterproxymodel.h"
#include <QtDebug>
#include <QtQml>

// Qt5 / Qt6 API兼容宏
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #define PRISM_FILTER_TYPE QRegularExpression
    #define PRISM_FILTER_MATCH(key, rx) key.contains(rx)
#else
    #define PRISM_FILTER_TYPE QRegExp
    #define PRISM_FILTER_MATCH(key, rx) key.contains(rx)
#endif

namespace prism::qt::core {

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), m_complete(false)
{
    connect(this, &QSortFilterProxyModel::rowsInserted, this, &SortFilterProxyModel::countChanged);
    connect(this, &QSortFilterProxyModel::rowsRemoved, this, &SortFilterProxyModel::countChanged);
}

int SortFilterProxyModel::count() const
{
    return rowCount();
}

QObject *SortFilterProxyModel::source() const
{
    return sourceModel();
}

void SortFilterProxyModel::setSource(QObject *source)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(source));
}

QByteArray SortFilterProxyModel::sortRole() const
{
    return m_sortRole;
}

void SortFilterProxyModel::setSortRole(const QByteArray &role)
{
    if (m_sortRole != role) {
        m_sortRole = role;
        if (m_complete)
            QSortFilterProxyModel::setSortRole(roleKey(role));
    }
}

void SortFilterProxyModel::setSortOrder(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}

QByteArray SortFilterProxyModel::filterRole() const
{
    return m_filterRole;
}

void SortFilterProxyModel::setFilterRole(const QByteArray &role)
{
    if (m_filterRole != role) {
        m_filterRole = role;
        if (m_complete)
            QSortFilterProxyModel::setFilterRole(roleKey(role));
    }
}

QString SortFilterProxyModel::filterString() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return filterRegularExpression().pattern();
#else
    return filterRegExp().pattern();
#endif
}

void SortFilterProxyModel::setFilterString(const QString &filter)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
    QRegularExpression rx(filter, options);
    setFilterRegularExpression(rx);
#else
    setFilterRegExp(QRegExp(filter, filterCaseSensitivity(),
                            static_cast<QRegExp::PatternSyntax>(filterSyntax())));
#endif
}

SortFilterProxyModel::FilterSyntax SortFilterProxyModel::filterSyntax() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return RegExp; // Qt6 仅支持 QRegularExpression
#else
    return static_cast<FilterSyntax>(filterRegExp().patternSyntax());
#endif
}

void SortFilterProxyModel::setFilterSyntax(SortFilterProxyModel::FilterSyntax syntax)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Q_UNUSED(syntax);
    // Qt6 使用 QRegularExpression，不支持多种匹配模式
#else
    setFilterRegExp(QRegExp(filterString(), filterCaseSensitivity(),
                            static_cast<QRegExp::PatternSyntax>(syntax)));
#endif
}

QJSValue SortFilterProxyModel::get(int idx) const
{
    QJSEngine *engine = qmlEngine(this);
    QJSValue value = engine->newObject();
    if (idx >= 0 && idx < count()) {
        QHash<int, QByteArray> roles = roleNames();
        for (auto it = roles.cbegin(), end = roles.cend(); it != end; ++it)
            value.setProperty(QString::fromUtf8(it.value()),
                              data(index(idx, 0), it.key()).toString());
    }
    return value;
}

void SortFilterProxyModel::classBegin() {}

void SortFilterProxyModel::componentComplete()
{
    m_complete = true;
    if (!m_sortRole.isEmpty())
        QSortFilterProxyModel::setSortRole(roleKey(m_sortRole));
    if (!m_filterRole.isEmpty())
        QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
}

void SortFilterProxyModel::sortEx()
{
    this->sort(0, Qt::AscendingOrder);
    this->invalidateFilter();
    this->invalidate();
}

int SortFilterProxyModel::roleKey(const QByteArray &role) const
{
    return roleNames().key(role, -1);
}

QHash<int, QByteArray> SortFilterProxyModel::roleNames() const
{
    if (QAbstractItemModel *source = sourceModel())
        return source->roleNames();
    return QHash<int, QByteArray>();
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression rx = filterRegularExpression();
    if (!rx.isValid() || rx.pattern().isEmpty())
        return true;
#else
    QRegExp rx = filterRegExp();
    if (rx.isEmpty())
        return true;
#endif

    QAbstractItemModel *model = sourceModel();
    if (!model)
        return true;

    if (filterRole().isEmpty()) {
        QHash<int, QByteArray> roles = roleNames();
        for (auto it = roles.cbegin(), end = roles.cend(); it != end; ++it) {
            QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
            QString key = model->data(sourceIndex, it.key()).toString();
            if (PRISM_FILTER_MATCH(key, rx))
                return true;
        }
        return false;
    }

    QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
    if (!sourceIndex.isValid())
        return true;

    QString key = model->data(sourceIndex, roleKey(filterRole())).toString();
    return PRISM_FILTER_MATCH(key, rx);
}

} // namespace prism::qt::core
