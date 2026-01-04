import QtQuick
import renju.core.dot 1.0
import renju.core.controller 1.0
import QtQuick.Controls
import QtQuick.Effects

Item {
    id: board
    objectName: "board"
    layer.enabled: true
    visible: false

    property int offset
    property double rowSize
    property double dotSize: rowSize * 0.42
    property variant dots: []

    signal mouseClicked(var obj)

    Dot {
        id: dot
    }

    Image {
        id: grid
        width: parent.width - offset * 2
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
            dot.x = Math.round((mouseX - offset) / rowSize)
            dot.y = Math.round((mouseY - offset) / rowSize)
            board.mouseClicked(dot)
        }
    }

    Rectangle {
        id: indicator
        anchors.centerIn: parent
        width: rowSize * 1.5
        height: rowSize * 1.5
        color: "white"
        opacity: 0.6
        visible: false
        radius: 10
        z: 100
        
        Rectangle {
            id: spinner
            width: parent.width * 0.5
            height: parent.height * 0.5
            anchors.centerIn: parent
            color: "transparent"
            
            Rectangle {
                width: parent.width * 0.2
                height: parent.height * 0.2
                radius: width / 2
                color: "#333"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
            }
            
            RotationAnimator {
                target: spinner
                from: 0
                to: 360
                duration: 1000
                loops: Animation.Infinite
                running: indicator.visible
            }
        }
        
        onVisibleChanged: {
            mouseArea.enabled = !visible
        }
    }

    Rectangle {
        id: winnerText
        color: "transparent"
        anchors.fill: parent
        anchors.centerIn: parent
        visible: false
        z: 100

        property string winner

        Rectangle {
            anchors.centerIn: parent
            color: "grey"
            width: 270
            height: 70
            opacity: 0.7
            radius: 10

            Text {
                id: winText
                anchors.centerIn: parent
                text: qsTr(winnerText.winner + " is win!")
                font.pointSize: 30
                color: "#fff"
                visible: false
            }

            MultiEffect {
                anchors.fill: winText
                source: winText
                shadowEnabled: true
                shadowVerticalOffset: 2
                shadowColor: "#333"
                shadowBlur: 0.1
                blurEnabled: true
                blur: 0.5
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                board.visible = false
                board.endGame()
                winnerText.visible = false
                menu.state = "start"
                menu.visible = true
            }
        }
    }

    Connections {
        target: Controller
        function onNextMoveChanged() {
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
        mouseArea.visible = true
        Controller.initGame(color)
    }

    function showWin(color) {
        indicator.visible = false
        mouseArea.visible = false
        winnerText.winner = color === -1 ? "Black" : "White"
        winnerText.visible = true
    }

    function endGame() {
        Controller.end()
        for (var i = 0; i < dots.length; i++) {
            dots[i].destroy();
        }
        dots = [];
    }
}


