#ifndef MEESHOPCATMODEL_H
#define MEESHOPCATMODEL_H

#include <QAbstractListModel>
#include <nlohmann/json.hpp>
#include <QList>
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
        CategoryNameRole
    };

    explicit CategoriesModel(QObject *parent = 0) : QAbstractListModel(parent), m_expandedCategory("")
    {
        QHash<int, QByteArray> roles;
        roles[CategoryAmountRole] = "categoryAmount";
        roles[CategoryKidsRole] = "categoryKids";
        roles[CategoryIdRole] = "categoryId";
        roles[CategoryNameRole] = "categoryName";
        setRoleNames(roles);
    }

    int getCount() { return m_json.size(); }

    void setJson(const json &jsonDoc);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QString getCatName(int index) { return QString::fromStdString(m_json.at(index)["name"].get<std::string>()); }

    Q_INVOKABLE void toggleKids(const QString &categoryName);

signals:
    void countChanged();

private:
    json m_json;
    QString m_expandedCategory; // Пустая строка означает, что ни одна категория не открыта

    void collapseCategory(const QString &categoryName);
    void expandCategory(const QString &categoryName);

    QVariant getCategoryData(const json &category, int role) const;
    json getCategoryByName(const QString &name) const;
    bool isChildCategory(const json &category) const;
};
}
#endif // MEESHOPCATMODEL_H
