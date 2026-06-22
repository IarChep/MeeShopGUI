#include "openreposapi.h"

#include <algorithm>
#include <vector>
#include <set>
#include <map>

namespace {
// Сколько страниц /apps тянуть для ранжирования и сколько «лучших» оставить.
const int TOP_PAGES = 8;
const int TOP_COUNT = 12;

// Дробное поле. ВАЖНО: рейтинг приходит строкой и БЫВАЕТ дробным ("53.3333"),
// поэтому только toDouble (QString::toInt на "53.3333" вернул бы 0).
double jdouble(const nlohmann::json &j, const char *key) {
    auto it = j.find(key);
    if (it == j.end()) return 0.0;
    if (it->is_number())  return it->get<double>();
    if (it->is_string())  return QString::fromStdString(it->get<std::string>()).toDouble();
    return 0.0;
}
// Целое поле (count/comments — целые строки "3"); тоже через toDouble на всякий случай.
int jint(const nlohmann::json &j, const char *key) {
    return static_cast<int>(jdouble(j, key));
}
double ratingScore(const nlohmann::json &a) {
    if (!a.is_object() || !a.contains("rating") || !a["rating"].is_object()) return 0.0;
    return jdouble(a["rating"], "rating"); // 0..100, может быть дробным
}
int ratingVotes(const nlohmann::json &a) {
    if (!a.is_object() || !a.contains("rating") || !a["rating"].is_object()) return 0;
    return jint(a["rating"], "count");
}
int commentsCnt(const nlohmann::json &a) {
    return a.is_object() ? jint(a, "comments_count") : 0;
}

// Обход дерева комментариев в глубину: выводит детей parentCid с проставленной
// глубиной depth, рекурсивно — детей каждого. visited страхует от циклов.
void appendThread(int parentCid, int depth,
                  const std::map<int, std::vector<const nlohmann::json*> > &children,
                  std::set<int> &visited, nlohmann::json &out) {
    std::map<int, std::vector<const nlohmann::json*> >::const_iterator it = children.find(parentCid);
    if (it == children.end())
        return;
    for (std::size_t i = 0; i < it->second.size(); ++i) {
        const nlohmann::json *c = it->second[i];
        const int cid = jint(*c, "cid");
        if (visited.count(cid))
            continue;
        visited.insert(cid);
        nlohmann::json node = *c;
        node["depth"] = depth;
        out.push_back(node);
        appendThread(cid, depth + 1, children, visited, out);
    }
}
}

namespace MeeShop {

void OpenReposApi::getCategories() {
    QString currentRoute = "/categories"; // Set the current route for categories
    QNetworkRequest request(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_categories()));
}

void OpenReposApi::getCategoryApps(int cat_id) {
    m_categoryId = cat_id;
    m_firstPage = 0;
    m_lastPage = 0;
    m_pageCount = 0;
    appModel->reset();
    setNextPageAvailable(false);
    setPrevPageAvailable(false);
    fetchPage(0);
}

void OpenReposApi::loadNextPage() {
    if (m_loading || !m_nextPageAvailable) return;
    fetchPage(m_lastPage + 1);
}

void OpenReposApi::loadPrevPage() {
    if (m_loading || !m_prevPageAvailable) return;
    fetchPage(m_firstPage - 1);
}

void OpenReposApi::fetchPage(int page) {
    if (page < 0) return;
    m_loading = true;
    m_requestedPage = page;
    QNetworkRequest request = createRequest(QUrl(baseUrl + QString("/categories/%1/apps?page=%2").arg(m_categoryId).arg(page)));
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(process_page()));
}
void OpenReposApi::search(QString query) {
    m_categoryId = -1; // поиск глобальный, не привязан к категории
    QString route = QString("/search/apps?keys=%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(query)));
    QNetworkRequest request = createRequest(QUrl(baseUrl + route));
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(process_search()));
}

void OpenReposApi::getApplication(int app_id) {
    QString currentRoute = "/apps/" + QString::number(app_id); // Set route for app info
    QNetworkRequest request = this->createRequest(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_app()));
}

void OpenReposApi::getAppComments(int app_id) {
    m_comments.clear();
    emit commentsChanged(); // сбрасываем комментарии предыдущего приложения
    QString currentRoute = "/apps/" + QString::number(app_id) + "/comments"; // Set route for app comments
    QNetworkRequest request = this->createRequest(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(process_comments()));
}

void OpenReposApi::process_comments() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit commentsFinished(false); // убрать индикатор загрузки
        return; // оставляем m_comments пустым
    }
    nlohmann::json arr = parseJson(reply->readAll());

    // Превращаем плоский список в дерево ответов: pid=0 (или несуществующий) — корень,
    // иначе ответ на комментарий с cid==pid. Раскладываем в порядке обхода в глубину,
    // каждому добавляем "depth" (для отступа/линий в QML).
    nlohmann::json threaded = nlohmann::json::array();
    if (arr.is_array()) {
        std::set<int> cids;
        for (const auto &c : arr)
            cids.insert(jint(c, "cid"));
        std::map<int, std::vector<const nlohmann::json*> > children;
        for (const auto &c : arr) {
            const int pid = jint(c, "pid");
            const int key = (pid != 0 && cids.count(pid)) ? pid : 0; // сирота -> корень
            children[key].push_back(&c);
        }
        std::set<int> visited;
        appendThread(0, 0, children, visited, threaded);
        // Подстраховка: всё, что не обошли (странные данные/циклы) — добавим как корни.
        for (const auto &c : arr) {
            if (!visited.count(jint(c, "cid"))) {
                nlohmann::json node = c;
                node["depth"] = 0;
                threaded.push_back(node);
            }
        }
    }
    m_comments = jsonToVariant(threaded).toList();
    emit commentsChanged();
    emit commentsFinished(true);
}

// ----------------------------------------------------- лента «недавно обновлённые»

void OpenReposApi::getRecentApps() {
    const int gen = ++m_recentGen; // аннулируем все ответы прежних запросов
    recentModel->reset();
    m_recentPage = 0;
    m_recentRequested = 0;
    m_recentLoading = true;
    setRecentHasMore(false);
    QNetworkRequest request = createRequest(QUrl(baseUrl + "/apps?page=0"));
    QNetworkReply *reply = manager.get(request);
    reply->setProperty("gen", gen);
    connect(reply, SIGNAL(finished()), this, SLOT(process_recent()));
}

void OpenReposApi::loadMoreRecent() {
    if (m_recentLoading || !m_recentHasMore) return;
    m_recentLoading = true;
    m_recentRequested = m_recentPage + 1;
    QNetworkRequest request = createRequest(QUrl(baseUrl + QString("/apps?page=%1").arg(m_recentRequested)));
    QNetworkReply *reply = manager.get(request);
    reply->setProperty("gen", m_recentGen);
    connect(reply, SIGNAL(finished()), this, SLOT(process_recent()));
}

void OpenReposApi::process_recent() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    // Протухший ответ (после reset/refresh) игнорируем — иначе он бы дописал
    // страницу в уже сброшенную модель и сломал нумерацию.
    if (reply->property("gen").toInt() != m_recentGen)
        return;
    m_recentLoading = false;
    if (reply->error() != QNetworkReply::NoError) {
        emit recentFinished(false);
        emit networkError();
        return;
    }
    nlohmann::json page = parseJson(reply->readAll());
    if (page.empty()) {
        setRecentHasMore(false); // конец ленты
        emit recentFinished(true);
        return;
    }
    recentModel->pushPageBack(page);
    m_recentPage = m_recentRequested;
    setRecentHasMore(true);
    emit recentFinished(true);
}

// --------------------------------------------------------- «лучшие» (топ по рейтингу)

void OpenReposApi::getTopApps() {
    const int gen = ++m_topGen; // аннулируем ответы прошлого запроса
    topModel->reset();
    m_topAccum = nlohmann::json::array();
    m_topPending = TOP_PAGES;
    m_topHadError = false;
    // Тянем первые TOP_PAGES страниц /apps параллельно; ранжируем по приходу всех.
    for (int p = 0; p < TOP_PAGES; ++p) {
        QNetworkRequest request = createRequest(QUrl(baseUrl + QString("/apps?page=%1").arg(p)));
        QNetworkReply *reply = manager.get(request);
        reply->setProperty("gen", gen);
        connect(reply, SIGNAL(finished()), this, SLOT(process_top()));
    }
}

void OpenReposApi::process_top() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->property("gen").toInt() != m_topGen)
        return; // протухший ответ (был refresh) — не учитываем
    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json page = parseJson(reply->readAll());
        if (page.is_array())
            for (nlohmann::json::iterator it = page.begin(); it != page.end(); ++it)
                m_topAccum.push_back(*it);
    } else {
        m_topHadError = true;
    }
    // Ошибочные страницы тоже уменьшают счётчик — иначе финал не наступит.
    if (--m_topPending > 0)
        return;
    finalizeTop();
}

void OpenReposApi::finalizeTop() {
    std::vector<nlohmann::json> apps(m_topAccum.begin(), m_topAccum.end());
    // Сортировка: выше рейтинг -> больше голосов -> больше комментариев.
    std::sort(apps.begin(), apps.end(), [](const nlohmann::json &a, const nlohmann::json &b) {
        const double ra = ratingScore(a), rb = ratingScore(b);
        if (ra != rb) return ra > rb;
        const int va = ratingVotes(a), vb = ratingVotes(b);
        if (va != vb) return va > vb;
        return commentsCnt(a) > commentsCnt(b);
    });
    // Берём топ-N уникальных по appid (страницы могли сдвинуться между запросами и
    // дать дубликат одного приложения).
    nlohmann::json top = nlohmann::json::array();
    std::set<int> seen;
    for (std::size_t i = 0; i < apps.size() && top.size() < static_cast<std::size_t>(TOP_COUNT); ++i) {
        const int id = jint(apps[i], "appid");
        if (id != 0 && seen.count(id))
            continue;
        seen.insert(id);
        top.push_back(apps[i]);
    }
    if (!top.empty())
        topModel->pushPageBack(top);
    // Провал только если не собрали НИЧЕГО и были ошибки (частичный успех — это успех).
    const bool ok = !(top.empty() && m_topHadError);
    if (!ok)
        emit networkError();
    emit topFinished(ok);
}

// ---------------------------------------------------- приложения разработчика

void OpenReposApi::getUserApps(int user_id) {
    const int gen = ++m_userGen;
    m_userId = user_id;
    userModel->reset();
    m_userPage = 0;
    m_userRequested = 0;
    m_userLoading = true;
    setUserHasMore(false);
    QNetworkRequest request = createRequest(QUrl(baseUrl + QString("/users/%1/apps?page=0").arg(user_id)));
    QNetworkReply *reply = manager.get(request);
    reply->setProperty("gen", gen);
    connect(reply, SIGNAL(finished()), this, SLOT(process_user()));
}

void OpenReposApi::loadMoreUser() {
    if (m_userLoading || !m_userHasMore || m_userId < 0) return;
    m_userLoading = true;
    m_userRequested = m_userPage + 1;
    QNetworkRequest request = createRequest(QUrl(baseUrl + QString("/users/%1/apps?page=%2").arg(m_userId).arg(m_userRequested)));
    QNetworkReply *reply = manager.get(request);
    reply->setProperty("gen", m_userGen);
    connect(reply, SIGNAL(finished()), this, SLOT(process_user()));
}

void OpenReposApi::process_user() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->property("gen").toInt() != m_userGen)
        return; // протухший ответ (сменился разработчик/refresh)
    m_userLoading = false;
    if (reply->error() != QNetworkReply::NoError) {
        emit userFinished(false);
        emit networkError();
        return;
    }
    nlohmann::json page = parseJson(reply->readAll());
    if (page.empty()) {
        setUserHasMore(false);
        emit userFinished(true);
        return;
    }
    userModel->pushPageBack(page);
    m_userPage = m_userRequested;
    // ВАЖНО: /users/{uid}/apps НЕ постранично — параметр page игнорируется, эндпоинт
    // отдаёт сразу ВСЕ приложения автора. Поэтому догрузки нет (иначе loadMoreUser
    // снова и снова дописывал бы тот же список). Всё загружено за один запрос.
    setUserHasMore(false);
    emit userFinished(true);
}

void OpenReposApi::getUserProfile(int user_id) {
    const int gen = ++m_userProfileGen; // протухшие ответы при смене разработчика
    m_userProfile.clear();
    emit userProfileChanged(); // сбрасываем прежний профиль (шапка покажет только ник)
    QNetworkRequest request = createRequest(QUrl(baseUrl + QString("/users/%1").arg(user_id)));
    QNetworkReply *reply = manager.get(request);
    reply->setProperty("gen", gen);
    connect(reply, SIGNAL(finished()), this, SLOT(process_user_profile()));
}

void OpenReposApi::process_user_profile() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->property("gen").toInt() != m_userProfileGen)
        return;
    // Профиль второстепенен: при ошибке оставляем пустым (без общего диалога) —
    // шапка деградирует до одного ника, а список приложений грузится отдельно.
    if (reply->error() != QNetworkReply::NoError)
        return;
    m_userProfile = jsonToVariant(parseJson(reply->readAll())).toMap();
    emit userProfileChanged();
}

void OpenReposApi::process_page() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    m_loading = false;

    if (reply->error() != QNetworkReply::NoError) {
        emit finished(false);
        emit networkError();
        return;
    }

    nlohmann::json page = parseJson(reply->readAll());
    const int requested = m_requestedPage;

    // Первичная загрузка категории: окно ещё пустое.
    if (m_pageCount == 0) {
        if (page.empty()) {
            setNextPageAvailable(false);
            setPrevPageAvailable(false);
            emit finished(false);
            return;
        }
        appModel->pushPageBack(page);
        m_firstPage = requested;
        m_lastPage = requested;
        m_pageCount = 1;
        setNextPageAvailable(true);
        setPrevPageAvailable(requested > 0);
        emit finished(true);
        return;
    }

    if (requested > m_lastPage) {
        // Листаем вперёд.
        if (page.empty()) {
            // Сервер вернул пустую страницу — текущая последняя была концом списка.
            setNextPageAvailable(false);
            emit finished(true);
            return;
        }
        appModel->pushPageBack(page);
        m_lastPage = requested;
        if (m_pageCount < ApplicationModel::MaxPages)
            m_pageCount++;
        else
            m_firstPage++;            // самая старая страница вытолкнута из окна
        setNextPageAvailable(true);
        setPrevPageAvailable(m_firstPage > 0);
    } else if (requested < m_firstPage) {
        // Листаем назад (эти страницы уже просматривались и непусты).
        if (page.empty()) {
            emit finished(true);
            return;
        }
        appModel->pushPageFront(page);
        m_firstPage = requested;
        if (m_pageCount < ApplicationModel::MaxPages)
            m_pageCount++;
        else
            m_lastPage--;             // самая новая страница вытолкнута из окна
        setNextPageAvailable(true);   // впереди снова есть страница
        setPrevPageAvailable(m_firstPage > 0);
    }

    emit finished(true);
}

void OpenReposApi::setNextPageAvailable(bool value) {
    if (m_nextPageAvailable == value) return;
    m_nextPageAvailable = value;
    emit nextPageAvailableChanged();
}

void OpenReposApi::setPrevPageAvailable(bool value) {
    if (m_prevPageAvailable == value) return;
    m_prevPageAvailable = value;
    emit prevPageAvailableChanged();
}

void OpenReposApi::setRecentHasMore(bool value) {
    if (m_recentHasMore == value) return;
    m_recentHasMore = value;
    emit recentHasMoreChanged();
}

void OpenReposApi::setUserHasMore(bool value) {
    if (m_userHasMore == value) return;
    m_userHasMore = value;
    emit userHasMoreChanged();
}

void OpenReposApi::process_search() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    m_loading = false;

    if (reply->error() != QNetworkReply::NoError) {
        emit finished(false);
        emit networkError();
        return;
    }

    nlohmann::json results = parseJson(reply->readAll());
    appModel->reset();
    m_pageCount = 0;
    setPrevPageAvailable(false);
    setNextPageAvailable(false); // результаты поиска отдаются одной страницей
    if (!results.empty()) {
        appModel->pushPageBack(results);
        m_pageCount = 1;
    }
    emit finished(true);
}

void OpenReposApi::process_categories() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json jsonObj = parseJson(reply->readAll());
        categoryModel->setJson(jsonObj);
        emit categoryModelChanged();
        emit finished(true);
    } else {
        emit finished(false);
        emit networkError();
    }
    reply->deleteLater();
}

void OpenReposApi::process_app() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "changed appInfo";
        nlohmann::json json = parseJson(reply->readAll());
        qDebug() << json.dump(4).c_str();
        QVariant var = jsonToVariant(json);
        appInfo = var.toMap();
        emit appInfoChanged();
    } else {
        qDebug() << "something is failed!";
        emit networkError();
    }
    reply->deleteLater();
}

nlohmann::json OpenReposApi::parseJson(const QByteArray& data) {
    std::string dataStr(data.constData(), data.size());
    if (nlohmann::json::accept(dataStr)) {
        return nlohmann::json::parse(dataStr);
    } else {
        return {};
    }
}
QVariant OpenReposApi::jsonToVariant(const nlohmann::json &j) {
    if (j.is_object()) {
        QVariantMap map;
        for (auto it = j.begin(); it != j.end(); ++it) {
            QString key = QString::fromStdString(it.key());
            map.insert(key, jsonToVariant(it.value()));
        }
        return map;
    } else if (j.is_array()) {
        QVariantList list;
        for (const auto &item : j) {
            list.append(jsonToVariant(item));
        }
        return list;
    } else if (j.is_string()) {
        return QString::fromStdString(j.get<std::string>());
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<unsigned int>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_null()) {
        return QVariant();
    }

    return QVariant();
}

QNetworkRequest OpenReposApi::createRequest(QUrl url) {
    QNetworkRequest request(url);
    request.setRawHeader("Accept-Language", "en");
    request.setRawHeader("Warehouse-Platform", "Harmattan");
    return request;
}
}
