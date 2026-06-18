import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

Page {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Search bar area
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: Material.dialogColor

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 12

                // Search input
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: "Search for music..."
                    text: searchVm.query
                    onTextChanged: searchVm.query = text

                    // Clear button
                    ToolButton {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        visible: searchField.text !== ""
                        icon.name: "edit-clear"
                        icon.width: 16
                        icon.height: 16
                        onClicked: searchField.clear()
                    }
                }

                // Platform filter
                ComboBox {
                    id: platformCombo
                    Layout.preferredWidth: 120
                    model: searchVm.availablePlatforms
                    onActivated: searchVm.selectPlatformByName(currentText)
                }
            }
        }

        // Content area
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Initial hint — no query
            Label {
                anchors.centerIn: parent
                visible: searchVm.query === "" && searchVm.results.count === 0
                text: "Search for music across platforms"
                font.pixelSize: 16
                color: Material.hintTextColor
            }

            // No results
            Label {
                anchors.centerIn: parent
                visible: searchVm.query !== "" && !searchVm.isLoading && searchVm.results.count === 0
                text: "No results found"
                font.pixelSize: 16
                color: Material.hintTextColor
            }

            // Results list
            ListView {
                id: resultsList
                anchors.fill: parent
                clip: true
                model: searchVm.results
                delegate: SongDelegate {
                    onDoubleClicked: searchVm.playSong(index)
                }

                // Infinite scroll
                onAtYEndChanged: {
                    if (atYEnd && searchVm.hasMore && !searchVm.isLoading) {
                        searchVm.loadMore()
                    }
                }

                // Loading footer
                footer: Item {
                    width: resultsList.width
                    height: visible ? 48 : 0
                    visible: searchVm.isLoading && searchVm.hasMore

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: visible
                    }
                }

                // Scrollbar
                ScrollBar.vertical: ScrollBar {}
            }

            // Initial loading indicator (no results yet)
            BusyIndicator {
                anchors.centerIn: parent
                visible: searchVm.isLoading && searchVm.results.count === 0
                running: visible
            }
        }
    }
}
