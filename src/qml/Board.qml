import QtQuick 2.12
import renju.core.dot 1.0
import renju.core.controller 1.0
import QtQuick.Controls 2.14

Item {
    id: board
    objectName: "board"
    anchors.fill: parent
    layer.enabled: true
    visible: false

    property int boardOffset: 20
    property int rowSize: 40
    property int dotSize: 17
    property variant dots: []

    signal mouseClicked(var obj)
    signal showWinText(var obj)

    Dot {
        id: dot
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.OpenHandCursor
        onPressed:cursorShape = Qt.ClosedHandCursor
        onReleased: cursorShape = Qt.OpenHandCursor
        onClicked: {
            console.log(mouseX + "  " + mouseY)
            dot.x = Math.round((mouseX - boardOffset) / rowSize)
            dot.y = Math.round((mouseY - boardOffset) / rowSize)
            board.mouseClicked(dot)
        }
    }

    Rectangle {
        id: indicator
        anchors.centerIn: parent
        width: rowSize * 1.5
        height: rowSize * 1.5
        color: "white"
        opacity: 0.8
        visible: false
        radius: 10
        z: 100
        BusyIndicator {
            id: busyIndicator
            anchors.centerIn: parent
            running: false
        }
        onVisibleChanged: {
            busyIndicator.running = visible
            mouseArea.enabled = !visible
        }
    }

    Connections {
        target: Controller
        onNextMoveChanged: {
            indicator.visible = Controller.state === Controller.AI
            var nextMove = Controller.nextMove
            paintDot(nextMove)
            if (Controller.checkWin(nextMove)) {
                showWin(nextMove.color)
                return
            }
            if (Controller.state === Controller.AI) {
                indicator.visible = true
                Controller.getNextMove()
            }
        }
    }

    function paintDot(dot) {
        var component = Qt.createComponent("Dot.qml")
        var dotItem = component.createObject(board,
                                             {
                                                 x: dot.x * rowSize,
                                                 y: dot.y * rowSize,
                                                 width: rowSize,
                                                 height: rowSize,
                                                 radius: rowSize / 2,
                                                 "dotColor" : dot.color,
                                             })
        if (!dotItem) {
            console.log("Error creating object")
        }
        dots.push(dotItem)
    }

    function init(color) {
        Controller.initGame(color)
    }

    function showWin(color) {
        board.showWinText(color === -1 ? "Black" : "White")
    }

    function endGame() {
        Controller.end()
        for (var i = 0; i < dots.length; i++) {
            dots[i].destroy();
        }
        dots = [];
    }
}


