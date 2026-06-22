#ifndef APTPARSER_H
#define APTPARSER_H

#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QRegExp>
#include <QVector>
#include "aptlogmodel.h"

// Универсальный разбор вывода apt/aegis-apt-get/dpkg на РЕГУЛЯРНЫХ ВЫРАЖЕНИЯХ.
// Вместо фиксированного switch/contains — упорядоченная таблица правил (первое
// совпадение побеждает) с обобщающим «catch-all» хвостом: ЛЮБАЯ непустая строка
// либо попадает в статус/лог, либо явно помечена как шум перерисовки. Добавить
// поддержку нового вида строки = добавить ОДНУ строку в таблицу (buildRules).
//
// Чистая функция QString -> AptLineResult: ни QObject, ни ввода-вывода — легко
// проверяется прогоном тест-векторов.

// Итог разбора ОДНОЙ строки apt.
struct AptLineResult {
    bool    drop          = true;  // true -> чистый шум, ничего не делать
    bool    updatesAction = false; // обновлять ли живой статус (actionChanged)
    bool    hasPercent    = false; // нести ли процент в progress()
    bool    loggable      = false; // писать ли в лог
    bool    determinate   = false; // true -> прогресс-бар; false -> busy-спиннер
    int     percent       = -1;    // 0..100, действителен при hasPercent
    QString action;                // готовая метка для статуса/лога
    AptLogModel::Level level = AptLogModel::Info;
};

// Одна строка таблицы правил. Порядок в таблице = приоритет.
struct AptRule {
    QRegExp            rx;                  // предкомпилирован (RegExp2), обычно привязан ^
    QString            label;              // шаблон: %1 = cap(cap1), %2 = cap(cap2)
    int                cap1 = 0;           // группа для %1 (0 = нет)
    int                cap2 = 0;           // группа для %2 (0 = нет)
    int                pctCap = 0;         // группа с процентом (когда не "(\\d+)%", напр. pmstatus)
    bool               useWholeLine = false; // метка = вся строка (для ошибок)
    AptLogModel::Level level = AptLogModel::Info;
    bool               carriesPercent = false;
    bool               loggable = true;
    bool               updatesAction = true;
    bool               determinate = false;
    bool               doneTo100 = false;  // фаза без цифры, но со словом Done -> percent=100
    bool               drop = false;       // правило-шум: бросить строку и выйти
    bool               debName = false;    // cap1 — имя .deb-файла: оставить имя пакета
};

class AptLineParser {
public:
    AptLineParser() { buildRules(); }

    AptLineResult parse(const QString &line) const {
        // (1) Жёсткие префиксы apt: уровень фиксируем ДО таблицы (точность раскраски
        //     лога и hasProblems()). apt печатает E:/W:/Err/Warning только в начале
        //     строки (а sanitize() уже обрезал пробелы), поэтому startsWith — не
        //     contains: иначе подстрока "E: "/"W: " в описании/скрипте ложно повышала
        //     бы уровень и портила hasProblems() на успешной установке.
        if (line.startsWith(QLatin1String("E: ")) || line.startsWith(QLatin1String("Err"))) {
            AptLineResult r;
            r.drop = false; r.updatesAction = true; r.loggable = true;
            r.level = AptLogModel::Error; r.action = line;
            return r;
        }
        if (line.startsWith(QLatin1String("W: ")) || line.startsWith(QLatin1String("Warning"))) {
            AptLineResult r;
            r.drop = false; r.updatesAction = true; r.loggable = true;
            r.level = AptLogModel::Warning; r.action = line;
            return r;
        }

        // (2) Таблица: первое совпадение побеждает.
        for (int i = 0; i < m_rules.size(); ++i) {
            const AptRule &rule = m_rules.at(i);
            if (rule.rx.indexIn(line) == -1)
                continue;
            if (rule.drop)
                return AptLineResult(); // drop=true по умолчанию

            AptLineResult r;
            r.drop          = false;
            r.updatesAction = rule.updatesAction;
            r.loggable      = rule.loggable;
            r.level         = rule.level;
            r.determinate   = rule.determinate;

            // Метка.
            if (rule.useWholeLine) {
                r.action = line;
            } else {
                r.action = rule.label;
                if (rule.cap1 > 0) {
                    QString c1 = rule.rx.cap(rule.cap1).trimmed();
                    if (rule.debName) {            // "pkg_ver_arch.deb" -> "pkg"
                        if (c1.endsWith(QLatin1String(".deb")))
                            c1.chop(4);
                        const int us = c1.indexOf(QLatin1Char('_'));
                        if (us > 0)
                            c1 = c1.left(us);     // имя пакета (без '_' по политике Debian)
                    }
                    r.action = r.action.arg(c1);
                }
                if (rule.cap2 > 0) r.action = r.action.arg(rule.rx.cap(rule.cap2).trimmed());
            }

            // Процент.
            if (rule.carriesPercent) {
                bool ok = false;
                int p = -1;
                if (rule.pctCap > 0) {
                    p = rule.rx.cap(rule.pctCap).toInt(&ok);
                } else if (m_pct.indexIn(line) != -1) {
                    p = m_pct.cap(1).toInt(&ok);
                }
                if (ok) {
                    r.hasPercent = true;
                    r.percent = clamp(p);
                } else if (rule.doneTo100 && line.contains(QLatin1String("Done"))) {
                    r.hasPercent = true; // фаза завершена -> добиваем бар до 100
                    r.percent = 100;
                }
            }
            return r;
        }

        // (3) Catch-all: строка не распознана ни одним правилом. Гарантия
        //     универсальности — ничего не теряем: в статус и в лог. Уровень
        //     поднимаем по ключевым словам ТОЛЬКО здесь (не трогая распознанные
        //     строки). Процент НЕ несём — неизвестная строка не двигает бар.
        AptLineResult r;
        r.drop = false; r.updatesAction = true; r.loggable = true;
        r.determinate = false; r.action = line; r.level = AptLogModel::Info;
        if (m_kwErr.indexIn(line) != -1)
            r.level = AptLogModel::Error;
        else if (m_kwWarn.indexIn(line) != -1)
            r.level = AptLogModel::Warning;
        return r;
    }

private:
    QVector<AptRule> m_rules; // собирается один раз в ctor
    QRegExp m_pct;            // общий извлекатель процента "(\\d+)%"
    QRegExp m_kwErr;          // эскалация уровня ТОЛЬКО для catch-all
    QRegExp m_kwWarn;

    static QRegExp rx2(const char *p) {
        return QRegExp(QString::fromLatin1(p), Qt::CaseSensitive, QRegExp::RegExp2);
    }
    static QRegExp rx2i(const char *p) {
        return QRegExp(QString::fromLatin1(p), Qt::CaseInsensitive, QRegExp::RegExp2);
    }
    static int clamp(int p) { return p < 0 ? 0 : (p > 100 ? 100 : p); }

    void buildRules() {
        m_pct   = rx2("(\\d+)%");
        // Эскалация уровня для НЕРАСПОЗНАННЫХ строк (catch-all) — только высокосигнальные
        // слова, иначе обычные строки (списки пакетов, "Note: ... no such ...",
        // debconf/maintainer-вывод) ложно делали бы успешную установку «с проблемами».
        // Настоящие ошибки уже ловятся префиксом E:/Err и правилами dpkg/aegis.
        m_kwErr = rx2i("\\b(failed|corrupt|fatal)\\b");
        m_kwWarn = rx2i("\\b(insecure|unsigned|untrusted)\\b");

        // --- Download / Acquire (самые частые) ---
        // Структурный Get: "Get:N URI suite/comp arch pkg ver [size]". Токены pkg/ver
        // не должны начинаться с '[' — иначе у коротких строк (релиз-файлы) сюда попал
        // бы размер "[12.3 kB]". Тогда такие строки уходят в короткое правило ниже.
        m_rules.append(AptRule{ .rx = rx2("^Get:\\d+\\s+\\S+\\s+\\S+\\s+\\S+\\s+([^\\[\\s]\\S*)\\s+([^\\[\\s]\\S*)(?:\\s+\\[.*\\])?$"),
                                .label = "Downloading %1 %2", .cap1 = 1, .cap2 = 2, .determinate = true });
        // Короткий Get: (релиз-файлы и т.п.). ЖАДНЫЙ захват до '[' (ленивый "+?" перед
        // необязательной группой/$ в QRegExp RegExp2 НЕ матчится — известный баг Qt4).
        m_rules.append(AptRule{ .rx = rx2("^Get:\\d+\\s+\\S+\\s+([^\\[]+)(?:\\[.*\\])?$"),
                                .label = "Downloading %1", .cap1 = 1, .determinate = true });
        // Кадр перерисовки прогресса загрузки: только двигает бар.
        m_rules.append(AptRule{ .rx = rx2("^(\\d+)%\\s+\\[.*\\]$"),
                                .label = "Downloading...", .carriesPercent = true,
                                .loggable = false, .updatesAction = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^Fetched\\s+([\\d.,]+\\s*[kKMG]?B)\\b.*$"),
                                .label = "Downloaded %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^Hit:?\\d*\\s+\\S+\\s+(.+)$"),
                                .label = "Up to date: %1", .cap1 = 1, .loggable = false });
        m_rules.append(AptRule{ .rx = rx2("^Ign:?\\d*\\s+\\S+\\s+(.+)$"),
                                .label = "Skipped: %1", .cap1 = 1 });

        // --- Фазы dpkg ---
        m_rules.append(AptRule{ .rx = rx2("^Unpacking\\s+([\\w.+:~-]+)\\s+\\([^)]*\\)\\s+over\\b"),
                                .label = "Upgrading %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^Unpacking\\s+([\\w.+:~-]+)\\s+\\([^)]*\\)"),
                                .label = "Unpacking %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^Setting up\\s+([\\w.+:~-]+)\\s+\\([^)]*\\)"),
                                .label = "Configuring %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^Removing\\s+([\\w.+:~-]+)\\s+\\([^)]*\\)"),
                                .label = "Removing %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^Purging configuration files for\\s+([\\w.+:~-]+)"),
                                .label = "Purging config for %1", .cap1 = 1 });
        // ЖАДНЫЙ захват (ленивый "+?" перед $ в QRegExp RegExp2 не матчится — баг Qt4).
        // Точка в конце обязательна -> жадность откатится и оставит имя без точки.
        m_rules.append(AptRule{ .rx = rx2("^Selecting previously unselected package\\s+([\\w.+:~-]+)\\.$"),
                                .label = "Preparing %1", .cap1 = 1 });
        // Захватываем имя .deb-файла жадно, имя пакета вычленяем в parse() (debName).
        m_rules.append(AptRule{ .rx = rx2("^Preparing to unpack\\s+(?:\\.\\.\\./)?(\\S+)\\s+\\.\\.\\.$"),
                                .label = "Preparing %1", .cap1 = 1, .debName = true });
        m_rules.append(AptRule{ .rx = rx2("^Processing triggers for\\s+([\\w.+:~-]+)"),
                                .label = "Updating %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^De-?configuring\\s+([\\w.+:~-]+)"),
                                .label = "Deconfiguring %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^Installing new version of config file\\s+(\\S+)"),
                                .label = "Installing config %1", .cap1 = 1 });

        // --- Чтение базы данных dpkg ---
        m_rules.append(AptRule{ .rx = rx2("^\\(Reading database \\.\\.\\.\\s*(\\d+)%"),
                                .label = "Reading database...", .carriesPercent = true,
                                .loggable = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^\\(Reading database \\.\\.\\.\\s*\\d+ files"),
                                .label = "Reading database..." });

        // --- Общий прогресс apt ---
        m_rules.append(AptRule{ .rx = rx2("^Progress:\\s*\\[\\s*(\\d+)%\\]$"),
                                .label = "Installing...", .carriesPercent = true,
                                .loggable = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^pmstatus:[^:]+:(\\d+)(?:\\.\\d+)?:"),
                                .label = "Installing...", .pctCap = 1, .carriesPercent = true,
                                .loggable = false, .determinate = true });

        // --- Фазы решателя (Done -> добить бар до 100) ---
        m_rules.append(AptRule{ .rx = rx2("^Reading package lists\\.\\.\\.\\s*\\d+%$"),
                                .label = "Reading package lists...", .carriesPercent = true,
                                .loggable = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^Reading package lists"),
                                .label = "Reading package lists...", .carriesPercent = true, .determinate = true, .doneTo100 = true });
        m_rules.append(AptRule{ .rx = rx2("^Building dependency tree\\.{0,3}\\s*\\d+%$"),
                                .label = "Building dependency tree...", .carriesPercent = true,
                                .loggable = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^Building dependency tree"),
                                .label = "Building dependency tree...", .carriesPercent = true, .determinate = true, .doneTo100 = true });
        m_rules.append(AptRule{ .rx = rx2("^Reading state information\\.\\.\\.\\s*\\d+%$"),
                                .label = "Reading state information...", .carriesPercent = true,
                                .loggable = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^Reading state information"),
                                .label = "Reading state information...", .carriesPercent = true, .determinate = true, .doneTo100 = true });
        m_rules.append(AptRule{ .rx = rx2("^Calculating upgrade"),
                                .label = "Calculating upgrade...", .carriesPercent = true, .determinate = true, .doneTo100 = true });
        m_rules.append(AptRule{ .rx = rx2("^Correcting dependencies"),
                                .label = "Correcting dependencies..." });

        // --- Итоги / размеры / план ---
        m_rules.append(AptRule{ .rx = rx2("^(\\d+) upgraded, (\\d+) newly installed,.*not upgraded\\.$"),
                                .label = "Plan: %1 upgraded, %2 new", .cap1 = 1, .cap2 = 2 });
        m_rules.append(AptRule{ .rx = rx2("^Need to get\\s+([\\d.,]+\\s*[kKMG]?B)\\b.*of archives\\.$"),
                                .label = "Need to download %1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^After this operation,\\s+([\\d.,]+\\s*[kKMG]?B)\\b.*will be (used|freed)\\.$"),
                                .label = "%1 will be %2", .cap1 = 1, .cap2 = 2 });
        m_rules.append(AptRule{ .rx = rx2("^The following [A-Za-z ]+:$"),
                                .label = "Planning changes..." });
        m_rules.append(AptRule{ .rx = rx2("^Extracting templates from packages:\\s*(\\d+)%$"),
                                .label = "Extracting templates...", .carriesPercent = true,
                                .loggable = false, .determinate = true });
        m_rules.append(AptRule{ .rx = rx2("^Do you want to continue"),
                                .label = "Waiting for confirmation...", .level = AptLogModel::Warning });

        // --- Ошибки/предупреждения dpkg/aegis (явные уровни; метка = вся строка) ---
        m_rules.append(AptRule{ .rx = rx2("^dpkg: warning:\\s*(.*)$"),
                                .label = "dpkg warning: %1", .cap1 = 1, .level = AptLogModel::Warning });
        m_rules.append(AptRule{ .rx = rx2("^dpkg:.*\\berror\\b"),
                                .useWholeLine = true, .level = AptLogModel::Error });
        m_rules.append(AptRule{ .rx = rx2("^dpkg-deb\\b.*\\berror\\b"),
                                .useWholeLine = true, .level = AptLogModel::Error });
        m_rules.append(AptRule{ .rx = rx2("\\bsubprocess returned error\\b"),
                                .useWholeLine = true, .level = AptLogModel::Error });
        m_rules.append(AptRule{ .rx = rx2("^aegis(?:-\\S+)?:.*\\b(refused|policy violation|verification failed|Bad signature)\\b"),
                                .useWholeLine = true, .level = AptLogModel::Error });
        m_rules.append(AptRule{ .rx = rx2("^aegis(?:-\\S+)?:.*\\b(untrusted|unsigned)\\b"),
                                .useWholeLine = true, .level = AptLogModel::Warning });
        m_rules.append(AptRule{ .rx = rx2("^aegis(?:-\\S+)?:\\s*(.+)$"),
                                .label = "%1", .cap1 = 1 });
        m_rules.append(AptRule{ .rx = rx2("^N:\\s*(.+)$"),
                                .label = "%1", .cap1 = 1 });
        // apt печатает «WARNING: ...» (напр. "cannot be authenticated!") — явное
        // правило, чтобы это был Warning, а не Error от catch-all по слову "cannot".
        m_rules.append(AptRule{ .rx = rx2i("^WARNING:\\s*(.+)$"),
                                .label = "%1", .cap1 = 1, .level = AptLogModel::Warning });

        // --- Шум перерисовки (бросаем; над catch-all) ---
        m_rules.append(AptRule{ .rx = rx2("^\\[(?:Working|Connecting|Waiting).*\\]$"), .drop = true });
        m_rules.append(AptRule{ .rx = rx2("^\\d+%$"), .drop = true });
        // Хвостовой catch-all реализован в parse() шагом (3).
    }
};

#endif // APTPARSER_H
