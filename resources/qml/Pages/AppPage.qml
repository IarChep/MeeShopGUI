import QtQuick 1.1
import com.nokia.meego 1.1
import "../Components"
import IarChep.MeeShop 1.0

Page {
    property int appId
    property variant appInfo: api.appInfo
    property variant gradientColors: gradienter.gradientColors
    property bool isInstalled;

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
            height: 200
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
                anchors.centerIn: parent
                spacing: 15
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
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: appInfo.title
                    color: "white"
                    font.pixelSize: 25
                    font.bold: true
                }
            }
        }
        Rectangle {
            id: appRect
            width: parent.width
            height: 170
            y: appPreviewRect.height
            color: "#e0e1e2"
            z: 1
            Column {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10
                Row {
                    anchors {
                        right: parent.right
                        left: parent.left
                        leftMargin: 15
                    }
                    spacing: 15
                    Item {
                        width: 64
                        height: width
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
                    //                Text {
                    //                    id: actionText
                    //                    visible: true
                    //                    Connections {
                    //                        target: packageManager
                    //                        onActionChanged: {
                    //                            actionText.color = "black"
                    //                            actionText.text = action;
                    //                            console.log("New qml action", action)
                    //                        }
                    //                        onAptErrorOrWarning: {
                    //                            if (type === "Error") {
                    //                                actionText.color = "red"
                    //                            } else if (type === "Warning") {
                    //                                actionText.color = "yellow"
                    //                            }
                    //                            actionText.text = message
                    //                        }
                    //                    }
                    //                }


                    Column {
                        Text {
                            text: appInfo.title
                            color: "black"
                            font.pixelSize: 25
                        }
                        Text {
                            text: "By: " + appInfo.user.name
                            color: "black"
                            font.pixelSize: 20
                        }
                    }
                }
                ButtonRow {
                    property alias deleteButtonVisible: deleteButton.visible
                    property alias updateButtonVisible: updateButton.visible
                    property alias installButtonVisible: installButton.visible

                    anchors.horizontalCenter: parent.horizontalCenter
                    exclusive: false
                    Button {
                        id: deleteButton
                        text: "Delete"
                    }
                    Button {
                        id: updateButton
                        text: "Update"
                    }
                    Button {
                        id: installButton
                        text: "Install"
                    }
                }
            }

            //            Button {
            //                id: installButton
            //                platformStyle: ButtonStyle {
            //                    fontPixelSize: 21
            //                    buttonWidth: 130
            //                    buttonHeight: 45
            //                }
            //                anchors {
            //                    right: parent.right
            //                    verticalCenter: parent.verticalCenter
            //                    rightMargin: 15
            //                }
            //                text: isInstalled ? "Installed" : "Install"
            //                enabled: !isInstalled
            //                onClicked: {
            //                    packageManager.install_package(appInfo.packages.harmattan.name ? appInfo.packages.harmattan.name : appInfo.package.name);
            //                    enabled = false;
            //                    appRect.appIconSize = 40;
            //                    appRect.indicatorVisible = true;
            //                }
            //            }
            Connections {
                target: packageManager
                onAptFinished: {
                    installButton.text = "Installed"
                    appRect.appIconSize = 64
                    appRect.indicatorVisible = false
                    actionText.visible = false
                }
            }

            property int appIconSize: 64
            property bool indicatorVisible: false
        }

        Column {
            id: infoColumn
            width: parent.width
            y: appPreviewRect.height + appRect.height
            SectionHeader {
                text: "Information"
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
            isInstalled = packageManager.is_installed(appInfo.packages.harmattan.name)
            waiter.hide();
        }
    }
}
