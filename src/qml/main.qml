import QtQuick 2.9
import QtQuick.Window 2.3

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

    Board {}

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
