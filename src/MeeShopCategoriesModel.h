#ifndef MEESHOPCATMODEL_H
#define MEESHOPCATMODEL_H
#include <QAbstractListModel>
#include <nlohmann/json.hpp>
#include <QList>
#include <algorithm>
#include <QDebug>

using json = nlohmann::json;

namespace MeeShop {
class MeeShopCategoriesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)
public:
    enum EntryRoles {
        CategoryAmountRole,
        CategoryDirRole,
        CategoryIdRole,
        CategoryNameRole
    };

    MeeShopCategoriesModel(QObject *parent = 0);

    int getCount() {return m_json.size();}

    void setJson(const json &jsonDoc);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QString getCatName(int index) {return QString::fromStdString(m_json.at(index)["name"].get<std::string>());}

signals:
    void countChanged();
private:
    json m_json;
};
}
#endif // MEESHOPMODEL_H


