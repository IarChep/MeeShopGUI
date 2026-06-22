import QtQuick 1.1
import com.nokia.meego 1.1
import "../components/ui"
import "../components/sheets"
import "../js/UIConstants.js" as Ui
import IarChep.MeeShop 1.0

Page {
    id: page
    property int appId
    property variant appInfo: api.appInfo
    // Иконка-источник для градиента (передаётся из AppsPage при переходе).
    property variant gradientSource
    // Открыта ли страница из очереди установок — тогда «назад» вернёт в очередь.
    property bool fromQueue: false
    // Открыта через push (со страницы разработчика / из очереди) — «назад» = pop.
    property bool pushed: false
    // Страница, на которую вернуться по «назад» через replace (главная или список).
    // Пусто -> список приложений (appsPage). Так главная и список ведут себя одинаково.
    property QtObject replaceOrigin: null
    // Комментарии грузятся по кнопке, а не автоматически.
    property bool commentsRequested: false
    // Идёт ли сейчас загрузка комментариев (для индикатора-«крутилки»).
    property bool commentsLoading: false
    // Доступен ли уже лог apt для этого приложения (после запуска установки).
    property bool aptLogAvailable: false

    // Повтор загрузки из общего диалога ошибки (main.qml).
    function retry() {
        waiter.show();
        api.getApplication(page.appId);
        if (page.commentsRequested) {
            page.commentsLoading = true;
            api.getAppComments(page.appId);
        }
    }

    // У страницы нет заголовка — единый HeaderBar сворачивается (его место
    // занимает прокручиваемая градиентная плашка-превью наверху).
    property Component pageHeader: null
    property Component pageHeaderBackground: null

    // Единая точка настройки видимости кнопок и блока статуса по состоянию очереди.
    function configureButtons() {
        if (!appInfo || !appInfo.user || appInfo.user.name === undefined)
            return;

        var st = (page.appId !== 0) ? queue.taskStatus(page.appId) : -1;
        // Лог доступен, как только задача запускалась (выполняется/завершилась).
        var ran = (st === InstallationQueue.Running || st === InstallationQueue.Done
                                                    || st === InstallationQueue.Failed);
        page.aptLogAvailable = ran;
        var log = ran ? queue.logForApp(page.appId) : null;
        var problems = log ? log.hasProblems() : false;

        var active = (st === InstallationQueue.Queued || st === InstallationQueue.Running);
        cancelButton.visible = active;
        // Отменить можно только пока приложение в очереди (ещё не пошла установка). #3
        cancelButton.enabled = (st === InstallationQueue.Queued);

        if (active) {
            installButton.visible = false;
            updateButton.visible = false;
            removeButton.visible = false;
            launchButton.visible = false;
            installationStatus.visible = true;
            appRect.indicatorVisible = (st === InstallationQueue.Running);
            // Иконка уменьшается (открывая индикатор) только когда уже выполняется —
            // важно при ОТКРЫТИИ страницы на уже идущей установке (нет нового сигнала).
            appRect.appIconSize = (st === InstallationQueue.Running) ? 40 : 64;
            if (st === InstallationQueue.Queued) {
                aptText.color = "black";
                aptText.text = "Queued";                 // #1: показываем статус ожидания
            } else if (aptText.text === "" || aptText.text === "Queued") {
                aptText.color = "black";
                aptText.text = "Working…";               // Running, действие ещё не пришло
            }
            return;
        }

        appRect.indicatorVisible = false;
        appRect.appIconSize = 64;

        // Блок "Installation status": показываем только при ошибке или предупреждениях,
        // при чистом успехе (без error/warning) — скрываем. #6
        if (st === InstallationQueue.Failed) {
            installationStatus.visible = true;
            aptText.color = "red";
            aptText.text = "Error — operation failed";
        } else if (st === InstallationQueue.Done && problems) {
            installationStatus.visible = true;
            aptText.color = "#3a9d23";
            aptText.text = "Completed (with warnings)";
        } else {
            installationStatus.visible = false;
        }

        var pkgName = (appInfo.packages && appInfo.packages.harmattan)
                ? appInfo.packages.harmattan.name : "";
        var repoEnabled = queue.isRepositoryEnabled(appInfo.user.name);
        var stat = repoEnabled ? queue.isInstalled(pkgName, appInfo.user.name)
                               : InstallationQueue.NotInstalled;
        installButton.visible = (stat === InstallationQueue.NotInstalled);
        updateButton.visible  = (stat === InstallationQueue.Updatable);
        removeButton.visible  = (stat !== InstallationQueue.NotInstalled);
        launchButton.visible  = (stat !== InstallationQueue.NotInstalled);
    }

    orientationLock: PageOrientation.LockPortrait
    tools: ToolBarLayout {
        visible: true
        ToolIcon {
            platformIconId: "toolbar-back"
            anchors.left: (parent === undefined) ? undefined : parent.left
            // Открыли из очереди (push) — возвращаемся в неё (pop); иначе из списка
            // приложений (replace) — возвращаемся в список. #2
            onClicked: {
                if (page.fromQueue || page.pushed)
                    appWindow.pageStack.pop();
                else
                    appWindow.pageStack.replace(page.replaceOrigin ? page.replaceOrigin : appsPage);
            }
        }
    }


    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: infoColumn.height + appRect.height + appPreviewRect.height
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Gradienter {
            id: appPreviewRect
            width: parent.width
            height: previewColumn.implicitHeight + 2*Ui.PADDING_XXLARGE
            source: page.gradientSource

            Column {
                id: previewColumn
                width: parent.width - 2*Ui.PADDING_XLARGE
                anchors.centerIn: parent
                spacing: Ui.PADDING_XLARGE
                NokiaShape {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 80
                    height: 80
                    FallbackImage {
                        id: icon
                        width: 64
                        height: 64
                        anchors.centerIn: parent
                        source: (appInfo.icon && appInfo.icon.url) ? appInfo.icon.url : ""
                        fallbackSource: "image://theme/icon-m-content-ovi-store-inverse"
                    }
                }
                Text {
                    id: previewTitle
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: appInfo.title ? appInfo.title.trim() : ""
                    font.pixelSize: 25
                    font.bold: true

                    // Если градиент под текстом слишком светлый — делаем текст чёрным.
                    function pickColor() {
                        var p = previewTitle.mapToItem(appPreviewRect, 0, previewTitle.height / 2);
                        return appPreviewRect.luminanceAt(p.y / appPreviewRect.height) > 0.6 ? "black" : "white";
                    }
                    color: "white"
                    Component.onCompleted: color = pickColor()
                    Connections {
                        target: appPreviewRect
                        onColorsChanged: previewTitle.color = previewTitle.pickColor()
                    }
                }
            }
        }
        Rectangle {
            id: appRect
            width: parent.width
            height: appColumn.implicitHeight + 2*Ui.PADDING_XLARGE
            y: appPreviewRect.height
            color: "#e0e1e2"
            z: 1
            Column {
                id: appColumn
                width: parent.width
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    margins: Ui.MARGIN_XLARGE
                }

                spacing: Ui.PADDING_DOUBLE
                Row {
                    width: page.width - 2 * Ui.MARGIN_XLARGE
                    spacing: Ui.PADDING_DOUBLE
                    Item {
                        width: 64
                        height: width
                        anchors.verticalCenter: parent.verticalCenter
                        ExtendedIndicator {
                            id: actionIndicator
                            type: "busy"
                            size: "medium"
                            progress: 0
                            running: true
                            visible: appRect.indicatorVisible
                            anchors.centerIn: parent
                        }
                        FallbackImage {
                            id: appIconImg
                            anchors.centerIn: parent
                            width: appRect.appIconSize
                            height: width
                            source: (appInfo.icon && appInfo.icon.url) ? appInfo.icon.url : ""
                            fallbackSource: "image://theme/icon-m-content-ovi-store-inverse"
                            Behavior on width {
                                PropertyAnimation {
                                    duration: 350
                                    easing.type: Easing.InOutQuad
                                }
                            }
                        }
                    }


                    Column {
                        width: parent.width - 64 - parent.spacing
                        Text {
                            width: parent.width
                            text: appInfo.title ? appInfo.title.trim() : ""
                            font.family: Ui.FONT_FAMILY
                            font.pixelSize: Ui.FONT_DEFAULT
                            wrapMode: Text.WordWrap
                            color: "black"
                        }
                        Text {
                            id: devText
                            // Имя разработчика кликабельно (синее) — открывает список его
                            // приложений (/users/{uid}/apps), если известен uid.
                            property bool hasDev: !!(appInfo.user && appInfo.user.uid)
                            text: "By: " + ((appInfo.user && appInfo.user.name) ? appInfo.user.name.trim() : "")
                            font.family: Ui.FONT_FAMILY_LIGHT
                            color: hasDev ? "#2a7fd0" : "black"
                            font.pixelSize: Ui.FONT_SMALL
                            MouseArea {
                                anchors.fill: parent
                                enabled: devText.hasDev
                                onClicked: {
                                    var uid = parseInt(appInfo.user.uid);
                                    if (uid > 0)
                                        appWindow.pageStack.push(developerPage, {
                                            userId: uid,
                                            developerName: appInfo.user.name ? appInfo.user.name.trim() : "" });
                                }
                            }
                        }
                    }
                }
                ButtonRow {
                    id: buttonRow
                    exclusive: false
                    height: 56
                    width: page.width - Ui.PADDING_XLARGE * 2
                    __maxButtonSize: page.width - Ui.PADDING_XLARGE * 2
                    platformStyle: ButtonStyle {
                        buttonWidth: page.width - 2*Ui.PADDING_XLARGE;
                        buttonHeight: 56
                        __colorString: "color8-"
                    }

                    Button {
                        id: removeButton
                        text: "Remove"
                        onClicked: { queue.enqueue(appInfo, InstallationQueue.Remove); page.configureButtons(); }
                    }
                    Button {
                        id: updateButton
                        text: "Update"
                        onClicked: { queue.enqueue(appInfo, InstallationQueue.Update); page.configureButtons(); }
                    }
                    Button {
                        id: launchButton
                        text: "Launch"
                    }
                    Button {
                        id: installButton
                        text: "Install"
                        onClicked: { queue.enqueue(appInfo, InstallationQueue.Install); page.configureButtons(); }
                    }
                    Button {
                        id: cancelButton
                        text: "Cancel"
                        onClicked: { queue.cancel(page.appId); page.configureButtons(); }
                    }
                    // Появляется, как только для приложения доступен лог apt. #5
                    Button {
                        id: aptLogButton
                        text: "APT Log"
                        visible: page.aptLogAvailable
                        onClicked: {
                            aptLogSheet.logModel = queue.logForApp(page.appId);
                            aptLogSheet.open();
                        }
                    }
                }
            }

            property int appIconSize: 64
            property bool indicatorVisible: false
        }

        Column {
            id: infoColumn
            width: parent.width
            y: appPreviewRect.height + appRect.height
            Column {
                id: installationStatus
                width: parent.width
                visible: false
                SectionHeader {
                    text: "Installation status"
                }
                Text {
                    id: aptText
                    font.pixelSize: 20
                    anchors {
                        left: parent.left
                        leftMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }


            SectionHeader {
                text: "Stats"
            }
            // Средний рейтинг звёздами (rating.rating — 0..100).
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 2
                visible: !!appInfo.rating
                property int filled: (appInfo.rating && appInfo.rating.rating !== undefined)
                                     ? Math.round(parseInt(appInfo.rating.rating) / 20) : 0
                Repeater {
                    model: 5
                    Text {
                        text: index < parent.filled ? "★" : "☆"
                        color: "#f0a000"
                        font.pixelSize: 30
                    }
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16
                Row {
                    spacing: 6
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        source: "image://theme/icon-s-common-like"
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 20
                        text: (appInfo.rating && appInfo.rating.count !== undefined) ? appInfo.rating.count : ""
                    }
                }
                Row {
                    spacing: 6
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        source: "image://theme/icon-s-transfer-download"
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 20
                        text: appInfo.downloads ? appInfo.downloads : ""
                    }
                }
            }


            SectionHeader {
                text: "Description"
            }
            TextCollapsible {
                id: __description
                anchors {
                    left: parent.left
                    leftMargin: 10
                    right: parent.right
                    rightMargin: 10
                }
                wrapMode:  Text.WordWrap
                font.pixelSize: 20
                text: appInfo.body !== undefined ? appInfo.body + "<br>" : ""
            }

            SectionHeader {
                text: "Screenshots"
                visible: appInfo.screenshots ? true : false
            }
            Flickable {
                boundsBehavior: Flickable.StopAtBounds
                flickableDirection: Flickable.HorizontalFlick
                height: screenshotRow.implicitHeight
                width: parent.width - 20
                anchors.horizontalCenter: parent.horizontalCenter
                contentWidth: screenshotRow.implicitWidth
                clip: true
                visible: appInfo.screenshots ? true : false
                Row {
                    id: screenshotRow
                    spacing: Ui.PADDING_SMALL
                    Repeater {
                        model: appInfo.screenshots ? appInfo.screenshots.length : 0
                        Image {
                            source: appInfo.screenshots[index].thumbs.medium
                            Rectangle {
                                anchors.fill: parent
                                color: "black"
                                opacity: imageMouseArea.pressed ? 0.3 : 0
                            }
                            MouseArea {
                                id: imageMouseArea
                                anchors.fill: parent
                                // Открываем во встроенном системном просмотрщике (с
                                // предварительным скачиванием) вместо браузера.
                                onClicked: mediaOpener.openImage(appInfo.screenshots[index].url)
                            }
                        }
                    }
                }
            }




            SectionHeader {
                text: "Changelog"
                visible: textChangelog.text.length
            }
            TextCollapsible {
                id: textChangelog
                anchors {
                    left: parent.left
                    leftMargin: 10
                    right: parent.right
                    rightMargin: 10
                }

                font.pixelSize: 20
                wrapMode: Text.WordWrap

                text: appInfo.changelog !== undefined ? appInfo.changelog+ "\n": ""
                visible: text.length
            }

            SectionHeader {
                text: "Comments"
            }
            // Кнопка загрузки — появляется только если у приложения есть комментарии
            // и они ещё не загружены (грузим по нажатию, не автоматически).
            // Прозрачный «делегат» высотой как стандартный пункт списка: текст слева,
            // стрелка вниз справа.
            Rectangle {
                id: loadCommentsItem
                width: parent.width
                height: Ui.LIST_ITEM_HEIGHT_DEFAULT
                color: loadCommentsArea.pressed ? "#d0d1d2" : "transparent"
                visible: !page.commentsRequested
                         && appInfo.comments_count !== undefined
                         && parseInt(appInfo.comments_count) > 0

                Text {
                    anchors {
                        left: parent.left; leftMargin: Ui.MARGIN_XLARGE
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Load comments (" + (appInfo.comments_count ? appInfo.comments_count : "0") + ")"
                    font.pixelSize: 24
                    color: "black"
                }
                Image {
                    anchors {
                        right: parent.right; rightMargin: Ui.MARGIN_XLARGE
                        verticalCenter: parent.verticalCenter
                    }
                    source: "image://theme/icon-m-toolbar-down"
                }
                MouseArea {
                    id: loadCommentsArea
                    anchors.fill: parent
                    onClicked: {
                        page.commentsRequested = true;
                        page.commentsLoading = true;
                        api.getAppComments(page.appId);
                    }
                }
            }
            // «Крутилка» пока грузятся комментарии — чтобы секция не была пустой.
            // Высота переключается МГНОВЕННО (без анимации): кнопка (80px) исчезает и
            // тут же заменяется крутилкой (90px), поэтому contentHeight не проседает и
            // Flickable не «отскакивает» вверх, пряча комментарии.
            LoadMoreRectangle {
                id: commentsLoader
                width: parent.width
                height: page.commentsLoading ? 90 : 0
                visible: page.commentsLoading
                Component.onCompleted: if (page.commentsLoading) startRotation()
                Connections {
                    target: page
                    onCommentsLoadingChanged: page.commentsLoading ? commentsLoader.startRotation()
                                                                   : commentsLoader.stopRotation()
                }
            }
            Column {
                width: parent.width
                visible: page.commentsRequested
                Repeater {
                    id: commentsRepeater
                    model: api.comments
                    // Комментарий в виде «пузырька»: слева — аватар автора, отступ и
                    // вертикальные линии-направляющие показывают вложенность (ответы).
                    Item {
                        id: cItem
                        width: page.width
                        property int rawDepth: modelData.depth ? modelData.depth : 0
                        property int depth: Math.min(rawDepth, 4) // ограничиваем визуальный отступ
                        property int step: 26
                        property int indent: depth * step
                        height: bubble.height + 12

                        // По одной вертикальной линии на каждый уровень вложенности.
                        Repeater {
                            model: cItem.depth
                            Rectangle {
                                x: index * cItem.step + 14
                                y: 2
                                width: 2
                                height: cItem.height - 4
                                color: "#c4c7ca"
                            }
                        }

                        // Аватар комментирующего (фото встроено в comment.user, если есть).
                        FallbackImage {
                            id: cAvatar
                            x: cItem.indent + 8
                            y: 6
                            width: 44; height: 44
                            source: (modelData.user && modelData.user.picture && modelData.user.picture.url)
                                    ? modelData.user.picture.url : ""
                            fallbackSource: "image://theme/icon-l-sharing-avatar-placeholder"
                        }

                        // Пузырёк сообщения. Ответы — чуть голубее, чтобы отличать.
                        Rectangle {
                            id: bubble
                            anchors {
                                left: cAvatar.right; leftMargin: 8
                                right: parent.right; rightMargin: 12
                                top: parent.top; topMargin: 6
                            }
                            radius: 12
                            clip: true
                            color: cItem.rawDepth > 0 ? "#dce9f7" : "#ffffff"
                            border.color: "#cfd2d5"
                            border.width: 1
                            height: bcol.height + 16

                            Column {
                                id: bcol
                                anchors { left: parent.left; right: parent.right; top: parent.top; margins: 8 }
                                spacing: 2
                                Row {
                                    width: parent.width
                                    spacing: 8
                                    Text {
                                        text: (modelData.user && modelData.user.name) ? modelData.user.name : "anonymous"
                                        font.bold: true; font.pixelSize: 17; color: "black"
                                        elide: Text.ElideRight
                                    }
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: modelData.created ? Qt.formatDateTime(new Date(parseInt(modelData.created) * 1000), "dd.MM.yyyy") : ""
                                        font.pixelSize: 14; color: "#888888"
                                    }
                                }
                                // RichText: сам подсвечивает ссылки голубым и делает их
                                // кликабельными, без «чёрного квадрата» в конце (его давал
                                // StyledText). Text.Wrap (WrapAnywhere) ломает длинные URL,
                                // чтобы текст не вылезал за границы пузыря.
                                Text {
                                    width: parent.width
                                    text: modelData.text ? modelData.text : ""
                                    textFormat: Text.RichText
                                    wrapMode: Text.Wrap
                                    font.pixelSize: 17
                                    color: "#333333"
                                    onLinkActivated: Qt.openUrlExternally(link)
                                }
                            }
                        }
                    }
                }
            }
            Text {
                visible: appInfo.comments_count !== undefined && parseInt(appInfo.comments_count) === 0
                anchors { left: parent.left; leftMargin: 10 }
                text: "No comments yet"
                color: "#888888"
                font.pixelSize: 18
            }
        }

        onContentYChanged: {
            appPreviewRect.y = contentY * 0.5;

            if (contentY > appPreviewRect.height)
                appRect.y = contentY;
            else
                appRect.y = appPreviewRect.height;
        }
    }

    Waiter { id: waiter }

    // Пока скриншот скачивается перед открытием во встроенном просмотрщике — крутилка.
    Connections {
        target: mediaOpener
        onBusyChanged: {
            if (mediaOpener.busy) {
                waiter.statusText = "Opening image…";
                waiter.show();
            } else {
                waiter.hide();
            }
        }
    }

    function reportAptFailure() {
        failDialog.open();
    }

    AptLogSheet { id: aptLogSheet }

    QueryDialog {
        id: failDialog
        titleText: "Operation failed"
        message: "apt couldn't finish the operation. Would you like to view the full log to see what went wrong?"
        acceptButtonText: "View log"
        rejectButtonText: "Not now"
        onAccepted: aptLogSheet.open()
    }

    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            waiter.show();
            api.getApplication(appId);
            // Комментарии НЕ грузим автоматически — только по кнопке (см. секцию Comments).
        }
    }
    Connections {
        target: api
        onAppInfoChanged: {
            // Градиент уже снят с иконки делегата при переходе на страницу.
            page.configureButtons();
            waiter.hide();
        }
        // Загрузка приложения не удалась — снимаем ожидание (диалог покажет main.qml).
        onNetworkError: waiter.hide()
        // Комментарии загрузились (успех/ошибка) — убираем «крутилку».
        onCommentsFinished: page.commentsLoading = false
    }

    // Живой статус/прогресс/завершение из очереди — только для этого приложения.
    Connections {
        target: queue
        onTaskActionChanged: {
            // Аргументы сигнала: (appId, action, determinate).
            if (appId !== page.appId) return;
            page.aptLogAvailable = true;
            installationStatus.visible = true;
            appRect.indicatorVisible = true;
            aptText.color = "black";
            aptText.text = action;
            actionIndicator.type = determinate ? "progress" : "busy";
        }
        onTaskProgressChanged: {
            if (appId !== page.appId) return;
            actionIndicator.type = indeterminate ? "busy" : "progress";
            actionIndicator.progress = progress;
        }
        onTaskStatusChanged: {
            if (appId !== page.appId) return;
            // Переоценить кнопки/блок статуса/иконку при любой смене статуса
            // (Queued→Running, Running→Done/Failed).
            page.configureButtons();
        }
        onTaskFinished: {
            if (appId !== page.appId) return;
            page.aptLogAvailable = true;
            // configureButtons выставит блок статуса/кнопки/иконку по итогу:
            // чистый успех — скрыть (#6), предупреждения — "Completed (with warnings)",
            // ошибка — "Error".
            page.configureButtons();
            if (code !== 0) {
                aptLogSheet.logModel = queue.logForApp(page.appId);
                page.reportAptFailure();
            }
        }
    }
}
