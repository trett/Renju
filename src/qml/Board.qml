pragma ComponentBehavior: Bound
import QtQuick
import renju.core.dot 1.0
import renju.core.controller 1.0
import QtQuick.Controls

Item {
    id: board
    objectName: "board"
    layer.enabled: true
    visible: false

    property int offset
    property double rowSize
    property double dotSize: rowSize * 0.42
    property variant dots: []
    property string winner: ""
    property bool showingWin: false

    signal mouseClicked(var obj)
    signal gameEnded()

    Dot {
        id: dot
    }

    Image {
        id: grid
        width: parent.width - board.offset * 2
        height: width
        source: "grid.svg"
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        mipmap: true
        sourceSize: Qt.size( img.sourceSize.width * 2, img.sourceSize.height * 2 )
        // properly scale SVG
        Image {
            id: img
            source: parent.source
            width: 0
            height: 0
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.OpenHandCursor
        onPressed: cursorShape = Qt.ClosedHandCursor
        onReleased: cursorShape = Qt.OpenHandCursor
        onClicked: {
            dot.x = Math.round((mouseX - board.offset) / board.rowSize)
            dot.y = Math.round((mouseY - board.offset) / board.rowSize)
            board.mouseClicked(dot)
        }
    }

    Rectangle {
        id: aiThinkingBadge
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: board.offset * 0.35
        height: board.rowSize * 1.1
        width: badgeContent.implicitWidth + board.rowSize * 0.7
        radius: height / 2
        color: "#222222"
        opacity: 0.95
        visible: false

        Row {
            id: badgeContent
            anchors.centerIn: parent
            spacing: board.rowSize * 0.2

            Text {
                text: qsTr("AI is thinking")
                font.pointSize: Math.max(1, board.rowSize * 0.34)
                font.bold: true
                color: "#fff"
                anchors.verticalCenter: parent.verticalCenter
            }

            Row {
                spacing: board.rowSize * 0.1
                anchors.verticalCenter: parent.verticalCenter

                Repeater {
                    model: 3
                    delegate: Rectangle {
                        id: pulseDot
                        required property int index
                        width: board.rowSize * 0.16
                        height: width
                        radius: width / 2
                        color: "#fff"
                        opacity: 0.3

                        SequentialAnimation {
                            running: aiThinkingBadge.visible
                            loops: Animation.Infinite
                            PauseAnimation { duration: pulseDot.index * 150 }
                            NumberAnimation { target: pulseDot; property: "opacity"; to: 1.0; duration: 350; easing.type: Easing.InOutQuad }
                            NumberAnimation { target: pulseDot; property: "opacity"; to: 0.3; duration: 350; easing.type: Easing.InOutQuad }
                            PauseAnimation { duration: (2 - pulseDot.index) * 150 }
                        }
                    }
                }
            }
        }

        onVisibleChanged: {
            mouseArea.enabled = !visible
        }
    }

    // The winner banner itself is rendered in Main.qml, above the board
    // (bound to board.winner / board.showingWin), since this Item clips
    // its children (layer.enabled: true) and there's no room to show it
    // without covering the stones. This overlay is just the "tap anywhere
    // to continue" click-catcher for after a win.
    Item {
        id: winOverlay
        anchors.fill: parent
        visible: false
        z: 100

        MouseArea {
            anchors.fill: parent
            onClicked: {
                board.visible = false
                board.endGame()
                winOverlay.visible = false
                board.showingWin = false
                board.gameEnded()
            }
        }
    }

    Connections {
        target: Controller
        function onNextMoveChanged() {
            aiThinkingBadge.visible = Controller.state === Controller.AI
            var nextMove = Controller.nextMove
            board.paintDot(nextMove)
            if (Controller.checkWin(nextMove)) {
                board.showWin(nextMove.color)
                return
            }
            if (Controller.state === Controller.AI) {
                aiThinkingBadge.visible = true
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
        mouseArea.visible = true
        Controller.initGame(color)
    }

    function showWin(color) {
        aiThinkingBadge.visible = false
        mouseArea.visible = false
        winner = color === -1 ? "Black" : "White"
        showingWin = true
        winOverlay.visible = true
    }

    function endGame() {
        Controller.end()
        for (var i = 0; i < dots.length; i++) {
            dots[i].destroy();
        }
        dots = [];
    }
}


