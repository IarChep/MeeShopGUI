#ifndef MEESHOPPAGMODEL_H
#define MEESHOPPAGMODEL_H
#include <QAbstractListModel>
#include <QDebug>

namespace MeeShop {
class MeeShopPagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)
public:
    enum EntryRoles {
        PagesRole
    };

    MeeShopPagesModel(QObject *parent = 0);

    int getCount() {return m_pages;}

    Q_INVOKABLE void setPages(const int &pages);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:
    void countChanged();
private:
    int m_pages;
};
}

Q_DECLARE_METATYPE(MeeShop::MeeShopPagesModel*)
#endif // MEESHOPMODEL_H


