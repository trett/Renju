import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 601
    height: 601
    title: qsTr("QRenju")

    Canvas {
        id: background
        anchors.fill: parent
        onPaint: {
            paintGrid(getContext("2d"));
        }
    }

    Board {}

    function paintGrid(ctx) {
        var step = 40;
        var fieldSize = 600;
        ctx.beginPath();
        ctx.clearRect(0, 0, fieldSize, fieldSize)
        ctx.fillStyle = "#f4ff68";
        ctx.fillRect(0, 0, fieldSize, fieldSize);
        ctx.strokeStyle = Qt.black;
        ctx.lineWidth = 1;
        for (var i = 0; i <= fieldSize; i += step) {
            ctx.moveTo(i, 0);
            ctx.lineTo(i, fieldSize);
            ctx.stroke();
            ctx.moveTo(0, i);
            ctx.lineTo(fieldSize, i);
            ctx.stroke();
        }
        ctx.closePath();
    }
}
