import QtQuick 1.1
import com.nokia.meego 1.0

Sheet {
    property variant argList

    InstallationManager {
        id: im
    }

    rejectButtonText: "Go back"

    content: Column {
        anchors.fill: parent
        Rectangle{
            width: parent.width
            height:200
            gradient: IconGradient {
                iconColors: gradienter.get_gradient_colors(argList[4])
            }
            Row {
                anchors.centerIn: parent
                spacing: 15
                Image {
                    id: bounding
                    source: "IconBounding.svg"
                    width: 80
                    height: 80
                    Image {
                        id: icon
                        source: argList[4]
                        anchors.centerIn: parent
                    }
                }
                Text {
                    text: argList[0]
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
                    source: argList[4]
                }
                Column {
                    Text {
                        text: argList[0]
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
                    im.test_install()
                    installRect.installation_started()
                }
            }

            //installation information
            InstallationRectangle {id: installRect; height: 0}
        }
    }
    onStatusChanged: {
        if (status === DialogStatus.Opening) {
            installButt.enabled = true
        }
    }
}
