import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

RowLayout {
    id: root

    /// The song title to display
    property string title: ""
    /// The artist name to display
    property string artist: ""
    /// The album cover URL to display
    property url coverUrl: ""

    spacing: 12

    // Album art or placeholder
    Rectangle {
        Layout.preferredWidth: 56
        Layout.preferredHeight: 56
        Layout.alignment: Qt.AlignVCenter
        color: Qt.rgba(1, 1, 1, 0.05)
        radius: 4

        Image {
            id: coverImage
            anchors.fill: parent
            anchors.margins: 2
            source: root.coverUrl
            fillMode: Image.PreserveAspectCrop
            visible: status === Image.Ready
            asynchronous: true
        }

        // Placeholder music icon
        Label {
            anchors.centerIn: parent
            visible: coverImage.status !== Image.Ready
            text: "\uD83C\uDFB5" // 🎵
            font.pixelSize: 24
            opacity: 0.5
        }
    }

    // Song info
    ColumnLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
        spacing: 2

        Label {
            Layout.fillWidth: true
            text: root.title || "No song playing"
            font.pixelSize: 13
            font.weight: Font.Medium
            elide: Text.ElideRight
            opacity: root.title ? 1.0 : 0.5
        }

        Label {
            Layout.fillWidth: true
            visible: root.artist !== ""
            text: root.artist
            font.pixelSize: 11
            color: Material.hintTextColor
            elide: Text.ElideRight
        }
    }
}
