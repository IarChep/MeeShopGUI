import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0

Sheet {
    property int appId;
    property variant appInfo;
    id: appSheet

    rejectButtonText: "Go back"

    content: Item {
        anchors.fill: parent
        Column {
            anchors.fill: parent
            Rectangle{
                id: appRect
                width: parent.width
                height:200
                gradient: IconGradient {
                    iconColors: gradienter.get_gradient_colors(appInfo.icon)
                }
                Row {
                    anchors.centerIn: parent
                    spacing: 15
                    NokiaShape {
                        id: bounding
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
                        text: appInfo.title
                        color: "white"
                        font.pixelSize: 25
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
            Rectangle {
                width: parent.width
                height: 100
                color: "#e0e1e2"
                Row {
                    x: 15
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 15
                    Image {
                        width: 64
                        height: width
                        source: appInfo.icon
                    }
                    Column {
                        Text {
                            text: appInfo.title
                            color: "black"
                            font.pixelSize: 25
                        }
                        Text {
                            text: "Size: " + argList[2]
                            color: "black"
                            font.pixelSize: 20
                        }
                    }
                    Text{
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
                    onClicked:  {
                        enabled = false
                        installRect.start_installation()
                    }
                }

                //installation information
                InstallationRectangle {id: installRect; height: 0}
            }
            Waiter {id: waiter}
            Connections {
                target: appSheet
                onStatusChanged: {
                    if (status == 0) {
                        console.log("opening")
                        waiter.show()
                    } else if (status === 1) {
                        console.log("opened")
                        api.fetchAppInfo(appId)
                        appSheet.appInfo = api.appInfo
                        console.log("info changed")
                        waiter.hide();
                    }
                }
            }
        }
    }
}
