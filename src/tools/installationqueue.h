#ifndef INSTALLATIONQUEUE_H
#define INSTALLATIONQUEUE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QMultiHash>
#include <QHash>
#include <QList>

#include "apt/apt.h"
#include "packageutils.h"
#include "../models/installationqueuemodel.h"

namespace MeeShop {

// Заменяет PackageManager: ведёт очередь приложений с разными операциями,
// выполняет apt последовательно, и отдаёт синхронные запросы (isInstalled,
// репозитории). Сигналы несут appId, чтобы UI знал, к какому приложению они.
class InstallationQueue : public QObject {
    Q_OBJECT
    Q_ENUMS(InstallationStatus Operation TaskStatus)
    Q_PROPERTY(QObject* model READ model CONSTANT)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
public:
    // Состояние установки конкретного пакета (для кнопок AppPage).
    enum InstallationStatus { NotInstalled = 0, Installed, Updatable };
    // Тип задачи в очереди.
    enum Operation { Install = 0, Update, Remove, RepoUpdate };
    // Жизненный цикл задачи. ВАЖНО: значения совпадают с проверками в модели.
    enum TaskStatus { Queued = 0, Running, Done, Failed };

    explicit InstallationQueue(QObject *parent = 0);

    QObject *model() const { return m_model; }
    int activeCount() const { return m_model->activeCount(); }

    // --- Очередь ---
    Q_INVOKABLE void enqueue(QVariantMap app, int operation);
    // Поставить в очередь удаление установленного приложения, известного только по
    // имени пакета (у него нет appId с openrepos). Синтезируем стабильный appId из
    // хеша имени пакета в высоком диапазоне — он не пересечётся с реальными id
    // openrepos (те небольшие) и одинаков для одного пакета (повторное удаление —
    // дедуп через contains()).
    Q_INVOKABLE void enqueueRemovalByPackage(const QString &name, const QString &developer,
                                             const QString &iconUrl, const QString &packageName);
    Q_INVOKABLE bool contains(int appId) const;
    Q_INVOKABLE int taskStatus(int appId) const;   // TaskStatus или -1
    Q_INVOKABLE void cancel(int appId);            // удаляет Queued/Done/Failed
    Q_INVOKABLE void move(int from, int to);
    // Индекс первой строки со статусом Queued (граница: ниже неё — ожидающие,
    // выше — выполняемые/завершённые). Возвращает count, если очередь пуста.
    // Используется QML, чтобы не давать ставить ожидающее приложение выше границы.
    Q_INVOKABLE int firstQueuedIndex() const;
    Q_INVOKABLE QObject *logForApp(int appId) const;

    // --- Тестовые помощники (для TestQueuePage) ---
    // addPlaceholder добавляет фиктивную запись (appId < 0) с заданным статусом
    // (TaskStatus) и образцом лога, НЕ запуская apt; clearPlaceholders удаляет
    // только такие записи, не трогая реальные задачи. Только для тестов UI.
    Q_INVOKABLE void addPlaceholder(int status);
    Q_INVOKABLE void clearPlaceholders();

    // --- Синхронные запросы (перенесены из PackageManager) ---
    Q_INVOKABLE int isInstalled(QString package, QString name);
    Q_INVOKABLE bool isRepositoryEnabled(QString name);
    Q_INVOKABLE void enableRepository(QString name);
    Q_INVOKABLE void disableRepository(QString name);
    Q_INVOKABLE void cacheInstalledPackages();
    Q_INVOKABLE void cacheEnabledRepositories();

signals:
    void taskActionChanged(int appId, QString action, bool determinate);
    void taskProgressChanged(int appId, int progress, bool indeterminate);
    void taskStatusChanged(int appId, int status);
    void taskFinished(int appId, int code);
    void activeCountChanged();

private slots:
    void onAptAction(const QString &action, bool determinate);
    void onAptProgress(const QString &action, int percent);
    void onAptLogLine(int level, const QString &message);
    void onAptFinished(int code, const QString &output);

private:
    enum AptStep { StepUpdate, StepInstall, StepRemove };

    void processNext();
    void runStep();
    // Строка активного (Queued/Running) удаления данного пакета, или -1. Чтобы не
    // ставить два удаления одного пакета (с любого входа: AppPage или Installed).
    int activeRemovalRow(const QString &packageName) const;
    const QMultiHash<QString, QVariantMap> &repoPackages(const QString &name);

    AptTools apt;
    InstallationQueueModel *m_model;

    int m_activeAppId;       // -1 если ничего не выполняется
    QList<int> m_steps;      // шаги apt для активной задачи
    int m_stepIndex;

    QMultiHash<QString, QVariantMap> installedPackages;
    QVariantMap enabledRepositories;
    QHash<QString, QMultiHash<QString, QVariantMap>> m_repoPackagesCache;
};

}

#endif // INSTALLATIONQUEUE_H
