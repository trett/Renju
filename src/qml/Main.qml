import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

Window {
    id: window
    visible: true
    minimumWidth: 640
    minimumHeight: 640
    maximumWidth: 640
    maximumHeight: 640
    title: qsTr("QRenju")
    color: "#966F33"

    property int boardOffset: 20
    property int rowSize: 40

    Canvas {
        id: boardCanvas
        anchors.fill: parent
        visible: false
        onPaint: paintGrid(getContext("2d"))
    }

    Rectangle {
        id: splash
        anchors.fill: parent
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

    Board {
        id: board
        onShowWinText: function(color) {
            menu.winColor = color
            menu.state = "winner"
            menu.visible = true
        }
    }

    Menu {
        id: menu
        onStartGame: function(color) {
            menu.visible = false
            boardCanvas.visible = true
            board.visible = true
            board.choosenColor = color
        }
    }

    function paintGrid(ctx) {
        var fieldSize = width - boardOffset
        ctx.beginPath()
        ctx.clearRect(0, 0, fieldSize, fieldSize)
        ctx.strokeStyle = "black"
        ctx.lineWidth = 1
        for (var i = boardOffset; i <= fieldSize; i += rowSize) {
            ctx.moveTo(i, boardOffset)
            ctx.lineTo(i, fieldSize)
            ctx.stroke()
            ctx.moveTo(boardOffset, i)
            ctx.lineTo(fieldSize, i)
            ctx.stroke()
        }
        ctx.closePath()
    }
}
