#include "categoriesmodel.h"


namespace MeeShop {

namespace {
// Имя категории: пустая строка, если ключа нет или это не строка.
QString catName(const json &c) {
    auto it = c.find("name");
    if (it != c.end() && it->is_string())
        return QString::fromStdString(it->get<std::string>());
    return QString();
}

// Числовое поле, которое в API приходит строкой ("42") либо числом.
int strToInt(const json &j, const char *key, int def = 0) {
    auto it = j.find(key);
    if (it == j.end()) return def;
    try {
        if (it->is_string()) return std::stoi(it->get<std::string>());
        if (it->is_number()) return it->get<int>();
    } catch (...) {}
    return def;
}

// Подкатегории: всегда валидный массив (пустой, если ключа нет).
const json &childrenOf(const json &c) {
    static const json empty = json::array();
    auto it = c.find("childrens");
    return (it != c.end() && it->is_array()) ? *it : empty;
}
}

void CategoriesModel::setJson(const json &jsonDoc)
{
    beginResetModel();
    m_json = jsonDoc;
    m_expandedCategory = "";
    m_nameCache.clear();
    for (const auto &category : m_json) {
        auto tid = category.find("tid");
        if (tid != category.end() && tid->is_string())
            m_nameCache[tid->get<std::string>()] = catName(category);
        for (const auto &child : childrenOf(category)) {
            auto ctid = child.find("tid");
            if (ctid != child.end() && ctid->is_string())
                m_nameCache[ctid->get<std::string>()] = catName(child);
        }
    }
    endResetModel();
    emit countChanged();
}

int CategoriesModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    int count = 0;
    for (const auto &category : m_json) {
        count++;
        if (catName(category) == m_expandedCategory)
            count += childrenOf(category).size();
    }
    return count;
}

QVariant CategoriesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    int row = index.row();
    for (const auto &category : m_json) {
        if (row == 0)
            return getCategoryData(category, role);
        row--;
        if (catName(category) == m_expandedCategory) {
            for (const auto &child : childrenOf(category)) {
                if (row == 0)
                    return getCategoryData(child, role);
                row--;
            }
        }
    }
    return QVariant();
}

QVariant CategoriesModel::getCategoryData(const json &category, int role) const {
    switch (role) {
    case CategoryAmountRole:
        if (category.contains("apps_count"))
            return strToInt(category, "apps_count");
        break;
    case CategoryKidsRole:
        return static_cast<qulonglong>(childrenOf(category).size());
    case CategoryIdRole:
        if (category.contains("tid"))
            return strToInt(category, "tid");
        break;
    case CategoryNameRole: {
        const QString name = catName(category);
        if (name.isEmpty())
            break;
        return isChildCategory(category) ? QString("    %1").arg(name) : name;
    }
    case CategoryUnformattedNameRole: {
        const QString name = catName(category);
        if (!name.isEmpty())
            return name;
        break;
    }
    }
    return QVariant();
}


void CategoriesModel::toggleKids(const QString &categoryName) {
    if (m_expandedCategory == categoryName) {
        collapseCategory(categoryName);
    } else {
        if (!m_expandedCategory.isEmpty()) {
            collapseCategory(m_expandedCategory);
        }
        expandCategory(categoryName);
    }
}

void CategoriesModel::collapseCategory(const QString &categoryName) {
    int row = 0;
    for (const auto &category : m_json) {
        if (catName(category) == categoryName) {
            beginRemoveRows(QModelIndex(), row + 1, row + childrenOf(category).size());
            m_expandedCategory = "";
            endRemoveRows();
            break;
        }
        row++;
    }
}

void CategoriesModel::expandCategory(const QString &categoryName) {
    int row = 0;
    for (const auto &category : m_json) {
        if (catName(category) == categoryName) {
            beginInsertRows(QModelIndex(), row + 1, row + childrenOf(category).size());
            m_expandedCategory = categoryName;
            endInsertRows();
            break;
        }
        row++;
    }
}

bool CategoriesModel::isChildCategory(const json &category) const {
    auto it = category.find("parents");
    if (it != category.end() && it->is_array() && !it->empty()) {
        const json &parent = it->front();
        try {
            if (parent.is_string()) return std::stoi(parent.get<std::string>()) > 0;
            if (parent.is_number()) return parent.get<int>() > 0;
        } catch (...) {}
    }
    return false;
}

}
