import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.12

Window {
    visible: true
    width: 640
    height: 640
    title: qsTr("QRenju")
    color: "#966F33"
    property int boardOffset: 20
    property int rowSize: 40

    Canvas {
        id: background
        anchors.fill: parent
        onPaint: {
            paintGrid(getContext("2d"));
        }
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
                splashImage.visible = false;
                board.visible = true;
            }
        }
        Component.onCompleted: visible = true
    }

    Board {
        id: board
    }

    function paintGrid(ctx) {
        var fieldSize = width - boardOffset;
        ctx.beginPath();
        ctx.clearRect(0, 0, fieldSize, fieldSize);
        ctx.strokeStyle = "black";
        ctx.lineWidth = 1;
        for (var i = boardOffset; i <= fieldSize; i += rowSize) {
            ctx.moveTo(i, boardOffset);
            ctx.lineTo(i, fieldSize);
            ctx.stroke();
            ctx.moveTo(boardOffset, i);
            ctx.lineTo(fieldSize, i);
            ctx.stroke();
        }
        ctx.closePath();
    }
}
