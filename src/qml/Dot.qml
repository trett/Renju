import QtQuick 2.0

Rectangle {
    property int dotColor

    color: dotColor === 1 ? "#f6f8fc" : "#131212"
    scale: 0.9
    gradient: Gradient {
        GradientStop { position: 0.0; color: dotColor === 1 ? "#f6f8fc" : "#131212" }
        GradientStop { position: 1.0; color: dotColor === 1 ? "#c8c7c7" : "#4b4141" }
    }
}
