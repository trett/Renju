import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12

Window {
    id: window
    visible: true
    title: qsTr("QRenju")
    color: "#966F33"
    minimumWidth: Screen.orientation === Qt.PortraitOrientation ? 720 : 640
    minimumHeight: Screen.orientation === Qt.PortraitOrientation ? 1280 : 780
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    property double offset: width / 32
    property double rowSize: width / 15

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width
        height: parent.height

        Board {
            id: board
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: width
        }
    }

    Rectangle {
        id: splash
        anchors.centerIn: parent
        color: "#966F33"
        width: splashImage.width
        height: splashImage.height

        Image {
            id: splashImage
            source: "qrenju.png"
            anchors.centerIn: parent
        }

        DropShadow {
            anchors.fill: splashImage
            horizontalOffset: 3
            verticalOffset: 3
            radius: 8.0
            samples: 17
            color: "black"
            source: splashImage
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                splash.destroy()
                splashImage.visible = false
                menu.visible = true
            }
        }
        Component.onCompleted: visible = true
    }

    Menu {
        id: menu
        onStartGame: function(color) {
            menu.visible = false
            board.offset = window.offset
            board.rowSize = window.rowSize
            board.visible = true
            board.init(color)
        }
    }
}
