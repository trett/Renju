import QtQuick 2.0
import QtQuick.Controls 2.13

Item {
    id: startScreen
    visible: false
    anchors.fill: parent
    signal startGame

    Column {
        id: column
        width: 274
        height: 180
        antialiasing: false
        transformOrigin: Item.Center
        spacing: 40
        anchors.centerIn: parent

        RoundButton {
            id: newGame
            text: qsTr("New Game")
            font.pointSize: 28
            width: parent.width
            height: 70
            onClicked: startScreen.startGame()
        }

        RoundButton {
            id: quit
            text: qsTr("Quit")
            font.pointSize: 28
            width: parent.width
            height: 70
            onClicked: Qt.quit()
        }
    }
}

