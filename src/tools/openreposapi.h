#ifndef MEESHOP_OPENREPOSAPI_H
#define MEESHOP_OPENREPOSAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QCoreApplication>
#include <nlohmann/json.hpp>
#include "../models/applicationmodel.h"
#include "../models/categoriesmodel.h"
#include <QVariantHash>

namespace MeeShop {

class OpenReposApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::ApplicationModel* appModel READ getAppModel NOTIFY appModelChanged)
    Q_PROPERTY(MeeShop::ApplicationModel* recentModel READ getRecentModel CONSTANT)
    Q_PROPERTY(MeeShop::ApplicationModel* topModel READ getTopModel CONSTANT)
    Q_PROPERTY(MeeShop::ApplicationModel* userModel READ getUserModel CONSTANT)
    Q_PROPERTY(MeeShop::CategoriesModel* categoryModel READ getCategoryModel NOTIFY categoryModelChanged)
    Q_PROPERTY(QVariantMap appInfo READ getAppInfo NOTIFY appInfoChanged)
    Q_PROPERTY(QVariantMap userProfile READ userProfile NOTIFY userProfileChanged)
    Q_PROPERTY(QVariantList comments READ getComments NOTIFY commentsChanged)
    Q_PROPERTY(bool isNextPageAvailable READ nextPageAvailable NOTIFY nextPageAvailableChanged)
    Q_PROPERTY(bool isPrevPageAvailable READ prevPageAvailable NOTIFY prevPageAvailableChanged)
    Q_PROPERTY(bool recentHasMore READ recentHasMore NOTIFY recentHasMoreChanged)
    Q_PROPERTY(bool userHasMore READ userHasMore NOTIFY userHasMoreChanged)
public:
    explicit OpenReposApi(QObject *parent = nullptr) : QObject{parent},
        appModel(new MeeShop::ApplicationModel(this)),
        recentModel(new MeeShop::ApplicationModel(this)),
        topModel(new MeeShop::ApplicationModel(this)),
        userModel(new MeeShop::ApplicationModel(this)),
        categoryModel(new MeeShop::CategoriesModel(this)),
        baseUrl("http://openrepos.wunderwungiel.pl/api/v1")
    {
    }

    MeeShop::ApplicationModel* getAppModel() {return appModel;}
    MeeShop::ApplicationModel* getRecentModel() {return recentModel;}
    MeeShop::ApplicationModel* getTopModel() {return topModel;}
    MeeShop::ApplicationModel* getUserModel() {return userModel;}
    MeeShop::CategoriesModel* getCategoryModel() {return categoryModel;}
    QVariantMap getAppInfo() {return appInfo;}
    QVariantMap userProfile() const {return m_userProfile;}
    QVariantList getComments() {return m_comments;}
    bool nextPageAvailable() {return m_nextPageAvailable;}
    bool prevPageAvailable() {return m_prevPageAvailable;}
    bool recentHasMore() {return m_recentHasMore;}
    bool userHasMore() {return m_userHasMore;}


    Q_INVOKABLE void getCategories();
    Q_INVOKABLE void getCategoryApps(int cat_id);
    Q_INVOKABLE void loadNextPage();
    Q_INVOKABLE void loadPrevPage();
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void getApplication(int app_id);
    Q_INVOKABLE void getAppComments(int app_id);
    // Лента «недавно обновлённые» (главная страница): GET /apps?page=N.
    Q_INVOKABLE void getRecentApps();
    Q_INVOKABLE void loadMoreRecent();
    // «Лучшие» приложения: у API нет эндпоинта топа, поэтому тянем несколько страниц
    // /apps и ранжируем по рейтингу на клиенте (топ-N в topModel).
    Q_INVOKABLE void getTopApps();
    // Приложения конкретного разработчика: GET /users/{uid}/apps?page=N.
    Q_INVOKABLE void getUserApps(int user_id);
    Q_INVOKABLE void loadMoreUser();
    // Профиль разработчика: GET /users/{uid} (имя, реальное имя, фото, подпись, роли).
    Q_INVOKABLE void getUserProfile(int user_id);

public slots:
    void process_page();
    void process_search();
    void process_categories();
    void process_app();
    void process_recent();
    void process_top();
    void process_user();
    void process_user_profile();
    void process_comments();
signals:
    void finished(bool sucsess);
    void appModelChanged();
    void categoryModelChanged();
    void appInfoChanged();
    void userProfileChanged();
    void commentsChanged();
    void nextPageAvailableChanged();
    void prevPageAvailableChanged();
    void recentHasMoreChanged();
    void userHasMoreChanged();
    void recentFinished(bool sucsess);
    void topFinished(bool sucsess);
    void userFinished(bool sucsess);
    // Загрузка комментариев завершилась (успех или ошибка) — чтобы убрать индикатор.
    void commentsFinished(bool sucsess);
    // Любая неудачная сетевая загрузка (любая страница/запрос) — для общего диалога.
    void networkError();

private:
    nlohmann::json parseJson(const QByteArray& data);
    QVariant jsonToVariant(const nlohmann::json& json);
    QNetworkRequest createRequest(QUrl url);

    void fetchPage(int page);
    void setNextPageAvailable(bool value);
    void setPrevPageAvailable(bool value);
    void setRecentHasMore(bool value);
    void setUserHasMore(bool value);
    void finalizeTop(); // ранжирование собранных страниц по рейтингу

    QString baseUrl;
    QNetworkAccessManager manager;

    MeeShop::ApplicationModel* appModel;
    MeeShop::ApplicationModel* recentModel;
    MeeShop::ApplicationModel* topModel;
    MeeShop::ApplicationModel* userModel;
    MeeShop::CategoriesModel* categoryModel;
    QVariantMap appInfo;
    QVariantMap m_userProfile;
    int m_userProfileGen = 0;
    QVariantList m_comments;

    // Лента «недавних» (главная) — простой постраничный догруз вперёд.
    int m_recentPage = 0;
    int m_recentRequested = 0;
    bool m_recentLoading = false;
    bool m_recentHasMore = false;
    int m_recentGen = 0;   // поколение запроса: сбрасывает «протухшие» ответы

    // «Лучшие»: собираем несколько страниц /apps и ранжируем по рейтингу.
    nlohmann::json m_topAccum;   // накопитель приложений со всех запрошенных страниц
    int m_topPending = 0;        // сколько страниц ещё не пришло
    int m_topGen = 0;
    bool m_topHadError = false;  // была ли ошибка хотя бы на одной из страниц топа

    // Лента приложений разработчика.
    int m_userId = -1;
    int m_userPage = 0;
    int m_userRequested = 0;
    bool m_userLoading = false;
    bool m_userHasMore = false;
    int m_userGen = 0;

    // Скользящее окно загруженных страниц — единственный источник правды.
    int m_categoryId = -1;
    int m_firstPage = 0;   // номер самой старой загруженной страницы окна
    int m_lastPage = 0;    // номер самой новой загруженной страницы окна
    int m_pageCount = 0;   // сколько страниц сейчас в окне (0..MaxPages)
    int m_requestedPage = 0;
    bool m_loading = false;
    bool m_nextPageAvailable = false;
    bool m_prevPageAvailable = false;
};

} // namespace MeeShop

#endif // MEESHOP_OPENREPOSAPI_H
