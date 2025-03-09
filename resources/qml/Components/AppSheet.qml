import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0

Sheet {
    id: appSheet
    property int appId
    property variant appInfo

    rejectButtonText: "Go back"

    content: Item {
        anchors.fill: parent

        // Превью приложения
        Rectangle {
            id: appPreviewRect
            width: parent.width
            height: 200
            gradient: IconGradient {
                iconColors: gradienter.get_gradient_colors(appInfo.icon)
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
                        source: appInfo.icon
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

        Flickable {
            id: flickable
            y: appPreviewRect.height + appRect.height
            width: parent.width
            height: parent.height - appRect.height - appPreviewRect.height
            contentHeight: appContent.height
            boundsBehavior: Flickable.StopAtBounds

            Rectangle {
                id: appContent
                color: "red"
                width: parent.width
                height: 800
                Column {
                    id: __column
                    anchors.fill: parent
                    spacing: 15
                    Text {
                        id: __header
                        text: "Description:"
                        font.pixelSize: 22
                        font.bold: true
                    }
                    TextCollapsible {
                        id: __description
                        anchors {
                            right: parent.right
                            left: parent.left
                        }
                        font.pixelSize: 20
                        wrapMode: Text.WordWrap
                        text: appInfo.description ? appInfo.description + "<br>" : ""
                    }
                }
            }

            onContentYChanged: {
                appPreviewRect.y -= (contentY - flickable.oldContentY) * 0.5;
                if (contentY < appPreviewRect.height) {
                    appRect.y -= (contentY - flickable.oldContentY);
                    appRect.y = Math.min(200, appRect.y);
                } else {
                    appRect.y = 0;
                }
                flickable.oldContentY = contentY;
            }
            property int oldContentY: 0
        }

        Rectangle {
            id: appRect
            width: parent.width
            height: 100
            y: 200
            color: "#e0e1e2"
            Row {
                x: 15
                anchors.verticalCenter: parent.verticalCenter
                spacing: 15
                Column {
                    Item {
                        width: 64
                        height: width
                        ProgressIndicator {
                            id: actionIndicator
                            size: "medium"
                            progress: 0
                            visible: appRect.indicatorVisible
                            anchors.centerIn: parent
                            Connections {
                                target: packageManager
                                onActionProgressChanged: {
                                    actionIndicator.progress = progress;
                                }
                            }
                        }
                        Image {
                            anchors.centerIn: parent
                            width: appRect.appIconSize
                            height: width
                            source: appInfo.icon
                            Behavior on width {
                                PropertyAnimation {
                                    duration: 350
                                    easing.type: Easing.InOutQuad
                                }
                            }
                        }
                    }
                    Text {
                        id: actionText
                        visible: true
                        Connections {
                            target: packageManager
                            onActionChanged: {
                                actionText.color = "black"
                                actionText.text = action;
                            }
                            onAptErrorOrWarning: {
                                if (type === "Error") {
                                    actionText.color = "red"
                                } else if (type === "Warning") {
                                    actionText.color = "yellow"
                                }
                                actionText.text = message
                            }
                        }
                    }
                }

                Column {
                    Text {
                        text: appInfo.title
                        color: "black"
                        font.pixelSize: 25
                    }
                    Text {
                        text: "By: " + appInfo.publisher
                        color: "black"
                        font.pixelSize: 20
                    }
                }
                Text {
                    id: statusText
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 80
                    font.pixelSize: 23
                }
            }

            Button {
                platformStyle: ButtonStyle {
                    fontPixelSize: 21
                    buttonWidth: 130
                    buttonHeight: 45
                }
                id: installButt
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 15
                text: "Install"
                onClicked: {
                    packageManager.install_package(appInfo.package);
                    enabled = false;
                    appRect.appIconSize = 40;
                    appRect.indicatorVisible = true;
                }
            }
            property int appIconSize: 64
            property bool indicatorVisible: false
        }

        Waiter { id: waiter }

        Connections {
            target: appSheet
            onStatusChanged: {
                if (status === 0) {
                    console.log("opening");
                    waiter.show();
                } else if (status === 1) {
                    console.log("opened");
                    api.fetchAppInfo(appId);
                    appSheet.appInfo = api.appInfo;
                    console.log("info changed");
                    waiter.hide();
                }
            }
        }
    }
}
