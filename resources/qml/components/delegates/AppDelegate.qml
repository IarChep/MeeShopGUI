import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import "../ui"


Rectangle {
    id: appdelegate

    property bool higlightLetters: false
    property alias iconImage: icon
    signal clicked()

    width: parent.width
    height: 90
    color: mouseArea.pressed === true ? "#bbbcbe" : "#e0e1e2"

    Row {
        x: UiConstants.DefaultMargin + 15
        anchors.verticalCenter: parent.verticalCenter
        spacing: 15
        FallbackImage {
            id: icon
            width: 65
            height: 65
            anchors.verticalCenter: parent.verticalCenter
            // При ошибке декодирования (напр. .jpeg, который не тянет симулятор) или
            // отсутствии иконки — стандартная иконка магазина.
            source: appIcon === undefined ? "" : appIcon
            fallbackSource: "image://theme/icon-m-content-ovi-store-inverse"
        }
        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 1
            Text {
                text: appName
                color: "black"
                font.pixelSize: 25
            }
            Text {
                text:  "By: " + appDev
                color: "black"
                font.pixelSize: 20
            }
            // Рейтинг (звёзды) + число оценок + число комментариев.
            Row {
                spacing: 6
                property int rated: (appRatingCount === undefined) ? 0 : appRatingCount
                property int filled: (appRating === undefined) ? 0 : Math.round(appRating / 20)
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    visible: parent.rated > 0
                    font.pixelSize: 18
                    color: "#f0a000"
                    text: {
                        var s = "";
                        for (var i = 0; i < 5; i++) s += (i < parent.filled) ? "★" : "☆";
                        return s;
                    }
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 16
                    color: "#888888"
                    text: parent.rated > 0 ? "(" + parent.rated + ")" : "Not rated yet"
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    visible: (appComments !== undefined && appComments > 0)
                    font.pixelSize: 16
                    color: "#888888"
                    text: "·  " + appComments + (appComments === 1 ? " comment" : " comments")
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: appdelegate.clicked()
    }

}
