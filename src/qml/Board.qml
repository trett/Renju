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

    signal mouseClicked(var obj)

    Dot {
        id: dot
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.OpenHandCursor
        onPressed: {
            cursorShape = Qt.ClosedHandCursor
        }
        onClicked: {
            console.log(mouseX + "  " + mouseY);
            dot.x = Math.round((mouseX - boardOffset) / rowSize);
            dot.y = Math.round((mouseY - boardOffset) / rowSize);
            board.mouseClicked(dot);
        }
        onReleased: {
            cursorShape = Qt.OpenHandCursor
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
        var component = Qt.createComponent("Dot.qml");
        var dotItem = component.createObject(board,
                                             {
                                                 x: dot.x * rowSize,
                                                 y: dot.y * rowSize,
                                                 width: rowSize,
                                                 height: rowSize,
                                                 radius: rowSize / 2,
                                                 "dotColor" : dot.color,
                                             });
        if (!dotItem) {
            console.log("Error creating object");
        }
    }

    function showWin(color) {
        winnerDialog.winner = color === -1 ? "Black" : "White";
        winnerDialog.open();
    }
}

