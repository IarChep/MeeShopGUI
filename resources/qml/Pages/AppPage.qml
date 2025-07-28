import QtQuick 1.1
import com.nokia.meego 1.1
import "../Components"
import "../Components/UIConstants.js" as Ui
import IarChep.MeeShop 1.0

Page {
    id: page
    property int appId
    property variant appInfo: api.appInfo
    property variant gradientColors: gradienter.gradientColors

    orientationLock: PageOrientation.LockPortrait
    tools: ToolBarLayout {
        visible: true
        ToolIcon {
            platformIconId: "toolbar-back"
            anchors.left: (parent === undefined) ? undefined : parent.left
            onClicked: appWindow.pageStack.replace(appsPage)
        }
    }


    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: infoColumn.height + appRect.height + appPreviewRect.height
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Rectangle {
            id: appPreviewRect
            width: parent.width
            height: previewColumn.implicitHeight + 2*Ui.PADDING_XXLARGE
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: gradientColors[0]
                }
                GradientStop {
                    position: 1.0
                    color: gradientColors[1]
                }
            }

            Column {
                id: previewColumn
                width: parent.width - 2*Ui.PADDING_XLARGE
                anchors.centerIn: parent
                spacing: Ui.PADDING_XLARGE
                NokiaShape {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 80
                    height: 80
                    Image {
                        id: icon
                        width: 64
                        height: 64
                        source: appInfo.icon.url
                        anchors.centerIn: parent
                    }
                }
                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: appInfo.title.trim()
                    color: "white"
                    font.pixelSize: 25
                    font.bold: true
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
                            Connections {
                                target: packageManager
                                onActionChanged: {
                                    if (action.indexOf("Downloading") !== -1) {
                                        actionIndicator.type = "progress"
                                    } else {
                                        actionIndicator.type = "busy"
                                    }
                                }

                                onActionProgressChanged: {
                                    actionIndicator.progress = progress;
                                }
                            }
                        }
                        Image {
                            anchors.centerIn: parent
                            width: appRect.appIconSize
                            height: width
                            source: appInfo.icon.url
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
                            text: appInfo.title.trim()
                            font.family: Ui.FONT_FAMILY
                            font.pixelSize: Ui.FONT_DEFAULT
                            wrapMode: Text.WordWrap
                            color: "black"
                        }
                        Text {
                            text: "By: " + appInfo.user.name.trim()
                            font.family: Ui.FONT_FAMILY_LIGHT
                            color: "black"
                            font.pixelSize: Ui.FONT_SMALL
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
                        id: repositoryButton
                        text: "Enable repository"
                        onClicked: {
                            enabled = false
                            installationStatus.visible = true
                            packageManager.enableRepository(appInfo.user.name)
                            appRect.indicatorVisible = true
                            appRect.appIconSize = 40;
                            packageManager.updateRepositories()
                        }
                    }
                    Button {
                        id: removeButton
                        text: "Remove"
                    }
                    Button {
                        id: updateButton
                        text: "Update"
                        onClicked: {
                            enabled = false;
                            installationStatus.visible = true;
                            actionIndicator.visible = true;
                            packageManager.installPackage(appInfo.packages.harmattan.name ? appInfo.packages.harmattan.name : appInfo.package.name)
                        }
                    }
                    Button {
                        id: launchButton
                        text: "Launch"
                    }

                    Button {
                        id: installButton
                        text: "Install"
                        onClicked: {
                            enabled = false;
                            installationStatus.visible = true;
                            appRect.indicatorVisible = true;
                            appRect.appIconSize = 40;
                            packageManager.installPackage(appInfo.packages.harmattan.name ? appInfo.packages.harmattan.name : appInfo.package.name);
                        }
                    }

                    Connections {
                        target: packageManager
                        onUpdateFinished: {
                            if (code === 0) {
                                if(packageManager.isRepositoryEnabled(appInfo.user.name)) {
                                    var stat = packageManager.isInstalled(appInfo.packages.harmattan.name, appInfo.user.name)
                                    repositoryButton.visible = false;
                                    if (stat == PackageManager.Installed) {
                                        installButton.visible = false;
                                        updateButton.visible = false;
                                    } else if (stat == PackageManager.Updatable) {
                                        installButton.visible = false;
                                    } else if (stat == PackageManager.NotInstalled) {
                                        deleteButton.visible = false;
                                        updateButton.visible = false;
                                        launchButton.visible = false;
                                    }
                                } else {
                                    launchButton.visible = false;
                                    removeButton.visible = false;
                                    updateButton.visible = false;
                                    installButton.visible = false;
                                }
                                appRect.appIconSize = 64
                                appRect.indicatorVisible = false
                            }
                        }
                        onInstallationFinished: {
                            packageManager.cacheInstalledPackages()
                            installButton.visible = false;
                            updateButton.visible = false;
                            removeButton.visible = true;
                            launchButton.visible = true;


                            appRect.appIconSize = 64
                            appRect.indicatorVisible = false
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
                    Connections {
                        target: packageManager
                        onActionChanged: {
                            aptText.color = "black"
                            aptText.text = action;
                            console.log("New qml action", action)
                        }
                        onAptErrorOrWarning: {
                            if (type === "Error") {
                                aptText.color = "red"
                            } else if (type === "Warning") {
                                aptText.color = "orange"
                            }
                            aptText.text = message
                        }
                    }
                }
            }


            SectionHeader {
                text: "Stats"
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
                        text: appInfo.rating.count
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
                        model: appInfo.screenshots.length
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
                                onClicked: {
                                    Qt.openUrlExternally(appInfo.screenshots[index].url)
                                }
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

    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            waiter.show();
            api.getApplication(appId);
        }
    }
    Connections {
        target: api
        onAppInfoChanged: {
            gradienter.getGradientColors(appInfo.icon.url ? appInfo.icon.url : "image://theme/icon-m-content-ovi-store-inverse")
            console.log("app info changed");
        }
    }
    Connections {
        target: gradienter
        onGradientChanged: {
            if(packageManager.isRepositoryEnabled(appInfo.user.name)) {
                var stat = packageManager.isInstalled(appInfo.packages.harmattan.name, appInfo.user.name)
                repositoryButton.visible = false;
                if (stat == PackageManager.Installed) {
                    installButton.visible = false;
                    updateButton.visible = false;
                } else if (stat == PackageManager.Updatable) {
                    installButton.visible = false;
                } else if (stat == PackageManager.NotInstalled) {
                    removeButton.visible = false;
                    updateButton.visible = false;
                    launchButton.visible = false;
                }
            } else {
                launchButton.visible = false;
                removeButton.visible = false;
                updateButton.visible = false;
                installButton.visible = false;
            }

            waiter.hide();
        }
    }
}
