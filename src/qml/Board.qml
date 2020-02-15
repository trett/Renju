import QtQuick 2.9
import renju.core.dot 1.0

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
    signal colorChoosed(var obj)
    signal showWinText(var obj)

    Dot {
        id: dot
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.OpenHandCursor
        onPressed:cursorShape = Qt.ClosedHandCursor
        onReleased: cursorShape = Qt.OpenHandCursor
        onClicked: {
            console.log(mouseX + "  " + mouseY);
            dot.x = Math.round((mouseX - boardOffset) / rowSize);
            dot.y = Math.round((mouseY - boardOffset) / rowSize);
            board.mouseClicked(dot);
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
        board.showWinText(color === -1 ? "Black" : "White");
    }
}

