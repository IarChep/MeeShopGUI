import QtQuick 1.1


Gradient {
    property variant iconColors
    GradientStop{
        position: 0.0
        color: iconColors[0]
    }
    GradientStop {
        position: 1.0
        color: iconColors[1]
    }
}

