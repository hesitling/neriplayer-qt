import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: root

    // --- Properties ---
    property string message: ""
    property int autoHideMs: 4000

    // --- Signals ---
    signal dismissed()

    // --- Public API ---
    function show(msg) {
        message = msg
        hideTimer.restart()
        showAnimation.start()
    }

    function hide() {
        hideAnimation.start()
    }

    // --- Internal ---
    visible: false
    height: 48
    radius: 8
    color: Material.color(Material.Red, Material.Shade800)
    opacity: 0

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 8
        spacing: 8

        // Warning icon
        Label {
            text: "\u26A0" // ⚠
            font.pixelSize: 16
            color: "white"
        }

        // Message
        Label {
            Layout.fillWidth: true
            text: root.message
            font.pixelSize: 13
            color: "white"
            elide: Text.ElideRight
        }

        // Close button
        ToolButton {
            icon.name: "close"
            icon.width: 16
            icon.height: 16
            icon.color: "white"
            onClicked: root.hide()
        }
    }

    // Auto-hide timer
    Timer {
        id: hideTimer
        interval: root.autoHideMs
        onTriggered: root.hide()
    }

    // Show animation
    SequentialAnimation {
        id: showAnimation

        PropertyAction {
            target: root
            property: "visible"
            value: true
        }

        NumberAnimation {
            target: root
            property: "opacity"
            from: 0
            to: 1
            duration: 200
            easing.type: Easing.OutCubic
        }
    }

    // Hide animation
    SequentialAnimation {
        id: hideAnimation

        NumberAnimation {
            target: root
            property: "opacity"
            from: 1
            to: 0
            duration: 200
            easing.type: Easing.InCubic
        }

        PropertyAction {
            target: root
            property: "visible"
            value: false
        }

        ScriptAction {
            script: root.dismissed()
        }
    }
}
