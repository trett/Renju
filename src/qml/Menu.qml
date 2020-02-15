import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

Item {
    id: menu
    visible: false
    anchors.fill: parent
    state: "start"

    signal startGame(int color)
    property string winColor

    ColumnLayout {
        id: column
        antialiasing: false
        spacing: 40
        anchors.centerIn: parent

        RoundButton {
            id: newGame
            text: qsTr("New Game")
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 270
            Layout.preferredHeight: 70
            font.pointSize: 28
            onClicked: menu.state = "colorChoice"
        }

        RoundButton {
            id: quit
            text: qsTr("Quit")
            Layout.preferredWidth: 270
            Layout.preferredHeight: 70
            font.pointSize: 28
            onClicked: Qt.quit()
        }

        RoundButton {
            id: black
            text: qsTr("Black")
            Layout.preferredWidth: 270
            Layout.preferredHeight: 70
            font.pointSize: 28
            onClicked: menu.startGame(-1)
        }

        RoundButton {
            id: white
            text: qsTr("White")
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 270
            Layout.preferredHeight: 70
            font.pointSize: 28
            onClicked: menu.startGame(1)
        }

        Text {
            id: winnerText
            text: qsTr(menu.winColor + " is win!")
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 270
            Layout.preferredHeight: 70
            font.pointSize: 30
            color: "#fff"
            layer.enabled: true
            layer.effect: DropShadow {
                verticalOffset: 2
                color: "#333"
                radius: 1
                samples: 3
            }
            MouseArea {
                anchors.fill: parent
                onClicked: menu.state = "start"
            }
        }
    }
    states: [
        State {
            name: "start"
            PropertyChanges {
                target: newGame
                visible: true
            }
            PropertyChanges {
                target: quit
                visible: true
            }
            PropertyChanges {
                target: black
                visible: false
            }
            PropertyChanges {
                target: white
                visible: false
            }
            PropertyChanges {
                target: winnerText
                visible: false
            }
        },
        State {
            name: "colorChoice"
            PropertyChanges {
                target: newGame
                visible: false
            }
            PropertyChanges {
                target: quit
                visible: false
            }
            PropertyChanges {
                target: black
                visible: true
            }
            PropertyChanges {
                target: white
                visible: true
            }
            PropertyChanges {
                target: winnerText
                visible: false
            }
        },
        State {
            name: "winner"
            PropertyChanges {
                target: newGame
                visible: false
            }
            PropertyChanges {
                target: quit
                visible: false
            }
            PropertyChanges {
                target: black
                visible: false
            }
            PropertyChanges {
                target: white
                visible: false
            }
            PropertyChanges {
                target: winnerText
                visible: true
            }
        }
    ]
}
