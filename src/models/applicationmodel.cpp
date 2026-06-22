#include "applicationmodel.h"


namespace MeeShop {

namespace {
// Безопасно достаёт строковое поле: пустая строка, если ключа нет или это не строка.
QString jsonStr(const json &j, const char *key) {
    auto it = j.find(key);
    if (it != j.end() && it->is_string())
        return QString::fromStdString(it->get<std::string>());
    return QString();
}
// Целое поле: в API openrepos числа часто приходят строками ("0"), поэтому
// принимаем и строку, и число. 0, если ключа нет/не разобрать.
int jsonInt(const json &j, const char *key) {
    auto it = j.find(key);
    if (it == j.end())
        return 0;
    if (it->is_number_integer())  return it->get<int>();
    if (it->is_number_unsigned()) return static_cast<int>(it->get<unsigned>());
    if (it->is_number_float())    return static_cast<int>(it->get<double>());
    // Строкой приходит и дробный рейтинг ("53.3333") — toInt вернул бы 0, берём toDouble.
    if (it->is_string())          return static_cast<int>(QString::fromStdString(it->get<std::string>()).toDouble());
    return 0;
}
}


void ApplicationModel::pushPageBack(const json &page)
{
    if (page.empty()) return; // пустая страница ломает диапазон beginInsertRows
    int lastRow = rowCount();
    beginInsertRows(QModelIndex(), lastRow, lastRow + page.size() - 1);
    m_jsonList.append(page);
    endInsertRows();

    if (m_jsonList.size() > MaxPages) {
        beginRemoveRows(QModelIndex(), 0, m_jsonList.first().size() - 1);
        m_jsonList.removeFirst();
        endRemoveRows();
    }
    emit pageBackAdded();
    emit countChanged();
}

void ApplicationModel::pushPageFront(const json &page)
{
    if (page.empty()) return;
    beginInsertRows(QModelIndex(), 0, page.size() - 1);
    m_jsonList.prepend(page);
    endInsertRows();

    if (m_jsonList.size() > MaxPages) {
        int lastRow = rowCount();
        beginRemoveRows(QModelIndex(), lastRow - m_jsonList.last().size(), lastRow - 1);
        m_jsonList.removeLast();
        endRemoveRows();
    }
    emit pageFrontAdded(page.size());
    emit countChanged();
}

void ApplicationModel::reset()
{
    beginResetModel();
    m_jsonList.clear();
    endResetModel();
    emit countChanged();
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
                return jsonStr(jsonElem, "title").simplified();
            case AppVerRole:
                return jsonStr(jsonElem, "version");
            case AppDevRole:
                if (jsonElem.contains("user"))
                    return jsonStr(jsonElem.at("user"), "name");
                break;
            case AppIdRole:
                if (jsonElem.contains("appid")) {
                    const json &id = jsonElem.at("appid");
                    if (id.is_number_integer())
                        return id.get<int>();
                    if (id.is_string())
                        return QString::fromStdString(id.get<std::string>()).toInt();
                }
                break;
            case AppIconRole:
                if (jsonElem.contains("icon"))
                    return jsonStr(jsonElem.at("icon"), "url");
                break;
            case AppRatingRole:
                if (jsonElem.contains("rating"))
                    return jsonInt(jsonElem.at("rating"), "rating"); // 0..100
                break;
            case AppRatingCountRole:
                if (jsonElem.contains("rating"))
                    return jsonInt(jsonElem.at("rating"), "count");
                break;
            case AppCommentsRole:
                return jsonInt(jsonElem, "comments_count");
            }
            break; // После нахождения элемента прерываем цикл
        } else {
            currentRow -= jsonDoc.size(); // Уменьшаем текущий индекс строки
        }
    }
    return QVariant();
}
}
