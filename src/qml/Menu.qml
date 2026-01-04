import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: menu
    visible: false
    anchors.fill: parent
    state: "start"

    signal startGame(var color)
    signal endGame();
    property string winColor

    ColumnLayout {
        id: column
        antialiasing: true
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
        }
    ]
}
