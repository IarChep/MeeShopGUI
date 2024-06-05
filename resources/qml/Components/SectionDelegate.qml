import QtQuick 1.1

Item {
    width: parent.width
    height: 40
    Text {
        id: headerLabel
        anchors.right: parent.right
        anchors.rightMargin: UiConstants.DefaultMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        text: section
        font.bold: true
        font.pointSize: 18
        color: theme.inverted ? "#4D4D4D" : "#3C3C3C";
    }
    Image {
        anchors.right: headerLabel.left
        anchors.left: parent.left
        anchors.verticalCenter: headerLabel.verticalCenter
        anchors.rightMargin: 24
        source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
    }
}
