#ifndef APTLOGMODEL_H
#define APTLOGMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QByteArray>

// Структурированный лог apt: каждая запись имеет время, уровень и текст.
// Отдаётся в QML как модель — для цветного моноширинного просмотра.
class AptLogModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Level { Info, Warning, Error };
    enum Roles {
        TimestampRole = Qt::UserRole + 1,
        LevelRole,    // "info" | "warning" | "error"
        MessageRole,  // исходный текст строки
        LineRole      // готовая строка лога: "[hh:mm:ss] ERROR  текст"
    };

    explicit AptLogModel(QObject *parent = 0) : QAbstractListModel(parent) {
        QHash<int, QByteArray> roles;
        roles[TimestampRole] = "timestamp";
        roles[LevelRole]     = "level";
        roles[MessageRole]   = "message";
        roles[LineRole]      = "line";
        setRoleNames(roles);
    }

    int count() const { return m_entries.size(); }

    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        return parent.isValid() ? 0 : m_entries.size();
    }

    QVariant data(const QModelIndex &index, int role) const {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
            return QVariant();
        const Entry &e = m_entries.at(index.row());
        switch (role) {
        case TimestampRole: return e.timestamp;
        case LevelRole:     return levelName(e.level);
        case MessageRole:   return e.message;
        case LineRole:      return formatLine(e);
        }
        return QVariant();
    }

    void append(Level level, const QString &message) {
        Entry e;
        e.timestamp = QDateTime::currentDateTime().toString(QString::fromLatin1("hh:mm:ss"));
        e.level = level;
        e.message = message;
        beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
        m_entries.append(e);
        endInsertRows();
        emit countChanged();
    }

    void clear() {
        if (m_entries.isEmpty())
            return;
        beginResetModel();
        m_entries.clear();
        endResetModel();
        emit countChanged();
    }

    // Были ли в логе предупреждения или ошибки (для решения, показывать ли блок
    // "Installation status" после успешного завершения).
    Q_INVOKABLE bool hasProblems() const {
        for (int i = 0; i < m_entries.size(); ++i)
            if (m_entries.at(i).level == Warning || m_entries.at(i).level == Error)
                return true;
        return false;
    }

    // Весь лог как обычный текст — для доступа/копирования извне.
    Q_INVOKABLE QString toPlainText() const {
        QStringList lines;
        for (int i = 0; i < m_entries.size(); ++i)
            lines << formatLine(m_entries.at(i));
        return lines.join(QString::fromLatin1("\n"));
    }

signals:
    void countChanged();

private:
    struct Entry {
        QString timestamp;
        Level level;
        QString message;
    };

    static QString levelName(Level level) {
        switch (level) {
        case Warning: return QString::fromLatin1("warning");
        case Error:   return QString::fromLatin1("error");
        default:      return QString::fromLatin1("info");
        }
    }
    static QString levelLabel(Level level) {
        switch (level) {
        case Warning: return QString::fromLatin1("WARN ");
        case Error:   return QString::fromLatin1("ERROR");
        default:      return QString::fromLatin1("INFO ");
        }
    }
    static QString formatLine(const Entry &e) {
        return QString::fromLatin1("[%1] %2  %3")
            .arg(e.timestamp, levelLabel(e.level), e.message);
    }

    QList<Entry> m_entries;
};

#endif // APTLOGMODEL_H
