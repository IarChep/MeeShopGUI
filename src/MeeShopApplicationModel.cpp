#include "MeeShopApplicationModel.h"

MeeShop::MeeShopApplicationModel::MeeShopApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[AppNameRole] = "appName";
    roles[AppVerRole] = "appVer";
    roles[AppDevRole] = "appDev";
    roles[AppIdRole] = "appId";
    roles[AppIconRole] = "appIcon";
    setRoleNames(roles);
}

void MeeShop::MeeShopApplicationModel::pushPageBack(const json &jsonDoc)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + jsonDoc.size() - 1);
    m_jsonList.append(jsonDoc);
    endInsertRows();
    if (m_jsonList.size() > 3) {
        beginRemoveRows(QModelIndex(), 0, m_jsonList.first().size() - 1);
        qDebug() << "Model: removing first json";
        m_jsonList.removeFirst();
        endRemoveRows();
    }
    emit pageAdded();

}

void MeeShop::MeeShopApplicationModel::pushPageFront(const json &jsonDoc)
{
    beginInsertRows(QModelIndex(), 0, jsonDoc.size() - 1);
    m_jsonList.prepend(jsonDoc);
    if (m_jsonList.size() > 3) {
        beginRemoveRows(QModelIndex(), rowCount() - m_jsonList.last().size(), rowCount() - 1);
        m_jsonList.removeLast();
        endRemoveRows();
    }
    endInsertRows();
}

int MeeShop::MeeShopApplicationModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    int count = 0;
    for (const auto &jsonDoc : m_jsonList) {
        count += jsonDoc.size();
    }
    return count;
}

QVariant MeeShop::MeeShopApplicationModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= rowCount()) {
        return QVariant();
    }

    int currentRow = index.row();
    for (const auto &jsonDoc : m_jsonList) {
        if (currentRow < jsonDoc.size()) {
            const json &jsonElem = jsonDoc.at(currentRow);

            switch (role) {
            case AppNameRole:
                if (jsonElem.contains("title"))
                    return QString::fromStdString(jsonElem["title"].get<std::string>());
                break;
            case AppVerRole:
                if (jsonElem.contains("version"))
                    return QString::fromStdString(jsonElem["version"].get<std::string>());
            case AppDevRole:
                if (jsonElem.contains("user"))
                    return QString::fromStdString(jsonElem["user"]["name"].get<std::string>());
                break;
            case AppIdRole:
                if (jsonElem.contains("appid"))
                    return jsonElem["appid"].get<int>();
                break;
            case AppIconRole:
                if (jsonElem.contains("icon")) {
                    return QString::fromStdString(jsonElem["icon"]["url"].get<std::string>());
                }
                break;
            }
            break; // После нахождения элемента прерываем цикл
        } else {
            currentRow -= jsonDoc.size(); // Уменьшаем текущий индекс строки
        }
    }
    return QVariant();
}
