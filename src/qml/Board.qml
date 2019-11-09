import QtQuick 2.9
import renju.core.dot 1.0
import QtQuick.Dialogs 1.2

Item {
    id: board
    objectName: "board"
    anchors.fill: parent
    layer.enabled: true
    visible: false
    property int boardOffset: 20
    property int rowSize: 40
    property int dotSize: 17
    property int xPos
    property int yPos
    property string color
    property bool needPaint: false

    signal mouseClicked(var obj)

    Dot {
        id: dot
    }

    Canvas {
        anchors.fill: parent
        id: boardCanvas
        onPaint: {
            if(board.needPaint) {
                var ctx = getContext("2d");
                ctx.fillStyle = color;
                ctx.beginPath();
                ctx.arc(xPos + boardOffset, yPos + boardOffset, dotSize, 0, 2 * Math.PI, false);
                ctx.fill();
                ctx.shadowOffsetX = 2;
                ctx.shadowOffsetY = 2;
                ctx.shadowBlur = 1;
                ctx.stroke();
                ctx.closePath();
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log(mouseX + "  " + mouseY);
                dot.x = Math.round((mouseX - boardOffset) / rowSize);
                dot.y = Math.round((mouseY - boardOffset) / rowSize);
                board.needPaint = true;
                board.mouseClicked(dot);
            }
        }
    }

    MessageDialog {
        id: winnerDialog
        visible: false
        title: "Renju"
        icon: StandardIcon.Information
        property string winner
        informativeText: winner + " is win!"
        onAccepted: {
            close();
        }
    }

    function paintDot(dot) {
        board.xPos = dot.x * rowSize;
        board.yPos = dot.y * rowSize;
        board.color = dot.color === -1 ? "#494646" : "#f6f8fc";
        boardCanvas.requestPaint();
    }

    function showWin(color) {
        winnerDialog.winner = color === -1 ? "Black" : "White";
        winnerDialog.open();
    }
}

