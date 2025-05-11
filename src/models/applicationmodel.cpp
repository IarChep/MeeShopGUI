#include "applicationmodel.h"


namespace MeeShop {


void ApplicationModel::pushPageBack(const json &page)
{
    int lastRow = rowCount();
    beginInsertRows(QModelIndex(), lastRow, lastRow + page.size() - 1);
    m_jsonList.append(page);
    endInsertRows();

    if (m_jsonList.size() > 3) {
        beginRemoveRows(QModelIndex(), 0, m_jsonList.first().size() - 1);
        m_jsonList.removeFirst();
        endRemoveRows();
    }
    emit pageBackAdded();
}

void ApplicationModel::pushPageFront(const json &page)
{
    beginInsertRows(QModelIndex(), 0, page.size() - 1);
    m_jsonList.prepend(page);
    endInsertRows();

    if (m_jsonList.size() > 3) {
        int lastRow = rowCount();
        beginRemoveRows(QModelIndex(), lastRow - m_jsonList.last().size(), lastRow - 1);
        m_jsonList.removeLast();
        endRemoveRows();
    }
    emit pageFrontAdded(page.size());
}

int ApplicationModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    int count = 0;
    for (const auto &jsonDoc : m_jsonList) {
        count += jsonDoc.size();
    }
    return count;
}

QVariant ApplicationModel::data(const QModelIndex &index, int role) const {
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
                    return QString::fromStdString(jsonElem["title"].get<std::string>()).simplified();
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
}
