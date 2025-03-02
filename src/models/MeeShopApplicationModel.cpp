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

void MeeShop::MeeShopApplicationModel::pushPageBack(const json &page)
{
    int diff = 0;
    beginResetModel();
    m_jsonList.append(page);
    if (m_jsonList.size() > 3) {
        diff = m_jsonList.first().size();
        m_jsonList.removeFirst();
    }
    endResetModel();
    emit pageBackAdded(diff);

}

void MeeShop::MeeShopApplicationModel::pushPageFront(const json &page)
{
    beginResetModel();
    m_jsonList.prepend(page);
    if (m_jsonList.size() > 3) {
        m_jsonList.removeLast();
    }
    endResetModel();
    emit pageFrontAdded(page.size());
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
