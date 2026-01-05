import QtQuick
import QtQuick.Window
import QtQuick.Controls as QQC
import QtQuick.Effects
import QtQuick.Layouts

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

        MultiEffect {
            anchors.fill: splashImage
            source: splashImage
            shadowEnabled: true
            shadowHorizontalOffset: 3
            shadowVerticalOffset: 3
            shadowBlur: 0.5
            shadowColor: "black"
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
        onStartGame: (color) => {
            menu.visible = false
            board.offset = window.offset
            board.rowSize = window.rowSize
            board.visible = true
            board.init(color)
        }
    }
}
