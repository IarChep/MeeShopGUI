#ifndef MEESHOPCATMODEL_H
#define MEESHOPCATMODEL_H

#include <QAbstractListModel>
#include <nlohmann/json.hpp>
#include <QList>
#include <unordered_map>
#include <algorithm>
#include <QDebug>
#include <QString>

using json = nlohmann::json;

namespace MeeShop {
class CategoriesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)
public:
    enum EntryRoles {
        CategoryAmountRole,
        CategoryKidsRole,
        CategoryIdRole,
        CategoryNameRole,
        CategoryUnformattedNameRole
    };

    explicit CategoriesModel(QObject *parent = 0) : QAbstractListModel(parent), m_expandedCategory("")
    {
        QHash<int, QByteArray> roles;
        roles[CategoryAmountRole] = "categoryAmount";
        roles[CategoryKidsRole] = "categoryKids";
        roles[CategoryIdRole] = "categoryId";
        roles[CategoryNameRole] = "categoryName";
        roles[CategoryUnformattedNameRole] = "unformattedName";
        setRoleNames(roles);
    }

    int getCount() { return m_json.size(); }

    void setJson(const json &jsonDoc);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QString getCatName(int index) {
        if (index < 0 || index >= static_cast<int>(m_json.size()))
            return QString();
        const json &c = m_json.at(index);
        auto it = c.find("name");
        return (it != c.end() && it->is_string())
            ? QString::fromStdString(it->get<std::string>()) : QString();
    }
    Q_INVOKABLE QString getCatName(QString tid) {
        auto it = m_nameCache.find(tid.toStdString());
        return it != m_nameCache.end() ? it->second : QString();
    }
    Q_INVOKABLE void toggleKids(const QString &categoryName);

signals:
    void countChanged();

private:
    json m_json;
    std::unordered_map<std::string, QString> m_nameCache;
    QString m_expandedCategory; // Пустая строка означает, что ни одна категория не открыта

    void collapseCategory(const QString &categoryName);
    void expandCategory(const QString &categoryName);

    QVariant getCategoryData(const json &category, int role) const;
    bool isChildCategory(const json &category) const;
};
}
#endif // MEESHOPCATMODEL_H
