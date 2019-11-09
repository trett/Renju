import QtQuick 2.9
import renju.core.dot 1.0
import QtQuick.Dialogs 1.2

Item {
    id: board
    objectName: "board"
    anchors.fill: parent
    layer.enabled: true

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
                ctx.fillStyle = parent.color;
                ctx.beginPath();
                ctx.arc(parent.xPos, parent.yPos, 17, 0, 2 * Math.PI, false);
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
                dot.x = Math.round(mouseX / 40)
                dot.y = Math.round(mouseY / 40)
                board.needPaint = true
                board.mouseClicked(dot)
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
        board.xPos = dot.x * 40
        board.yPos = dot.y * 40
        board.color = dot.color === -1 ? "#494646" : "#f6f8fc"
        boardCanvas.requestPaint()
    }

    function showWin(color) {
        winnerDialog.winner = color === -1 ? "Black" : "White"
        winnerDialog.open()
    }
}

