#include "categoriesmodel.h"


namespace MeeShop {

void CategoriesModel::setJson(const json &jsonDoc)
{
    beginResetModel();
    m_json = jsonDoc;
    m_expandedCategory = "";
    endResetModel();
    emit countChanged();
}

int CategoriesModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    int count = 0;
    for (const auto &category : m_json) {
        count++;
        if (QString::fromStdString(category["name"].get<std::string>()) == m_expandedCategory) {
            count += category["childrens"].size();
        }
    }
    return count;
}

QVariant CategoriesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    int row = index.row();
    for (const auto &category : m_json) {
        if (row == 0) {
            return getCategoryData(category, role);
        }
        row--;
        if (QString::fromStdString(category["name"].get<std::string>()) == m_expandedCategory) {
            for (const auto &child : category["childrens"]) {
                if (row == 0) {
                    return getCategoryData(child, role);
                }
                row--;
            }
        }
    }
    return QVariant();
}

QVariant CategoriesModel::getCategoryData(const json &category, int role) const {
    switch (role) {
    case CategoryAmountRole:
        if (category.contains("apps_count")) {
                return std::stoi(category["apps_count"].get<std::string>());
        }
        break;
    case CategoryKidsRole:
        if (category.contains("childrens")) {
            return static_cast<qulonglong>(category["childrens"].get<json>().size());
        }
        break;
    case CategoryIdRole:
        if (category.contains("tid")) {
                return std::stoi(category["tid"].get<std::string>());
        }
        break;
    case CategoryNameRole:
        if (category.contains("name")) {
            if (isChildCategory(category)) {
                return QString("    %1").arg(QString::fromStdString(category["name"].get<std::string>()));
            } else {
                return QString::fromStdString(category["name"].get<std::string>());
            }
        }
        break;
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
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void CategoriesModel::collapseCategory(const QString &categoryName) {
    int row = 0;
    for (const auto &category : m_json) {
        if (QString::fromStdString(category["name"].get<std::string>()) == categoryName) {
            beginRemoveRows(QModelIndex(), row + 1, row + category["childrens"].size());
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
        if (QString::fromStdString(category["name"].get<std::string>()) == categoryName) {
            beginInsertRows(QModelIndex(), row + 1, row + category["childrens"].size());
            m_expandedCategory = categoryName;
            endInsertRows();
            break;
        }
        row++;
    }
}

json CategoriesModel::getCategoryByName(const QString &name) const {
    for (const auto &category : m_json) {
        if (QString::fromStdString(category["name"].get<std::string>()) == name) {
            return category;
        }
    }
    return json();
}

bool CategoriesModel::isChildCategory(const json &category) const {
    if (category.contains("parents") && !category["parents"].empty()) {
        return std::stoi(category["parents"][0].get<std::string>()) > 0;
    }
    return false;
}

}
