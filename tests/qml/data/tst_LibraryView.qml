import QtQuick
import QtTest

Item {
    id: root
    width: 800
    height: 600

    QtObject {
        id: playlistVm
        property var localPlaylists: []
        property var neteasePlaylists: []
        property var neteaseAlbums: []
        property bool isLoading: false
        property bool hasError: false
        property var error: ({ message: "" })
        property int localOpenCount: 0
        property int remotePlaylistOpenCount: 0
        property int remoteAlbumOpenCount: 0
        property int loadPlaylistCount: 0
        property int loadAlbumCount: 0
        property int createPlaylistCount: 0
        property string lastCreatedName: ""

        function openLocalPlaylist(index) { localOpenCount += 1 }
        function openNeteasePlaylist(index) { remotePlaylistOpenCount += 1 }
        function openNeteaseAlbum(index) { remoteAlbumOpenCount += 1 }
        function loadNeteasePlaylists() { loadPlaylistCount += 1 }
        function loadNeteaseAlbums() { loadAlbumCount += 1 }
        function createLocalPlaylist(name) {
            createPlaylistCount += 1
            lastCreatedName = name
        }
    }

    TestCase {
        name: "LibraryView"
        when: windowShown

        function createView(props) {
            var component = Qt.createComponent("../../../src/qml/LibraryView.qml")
            verify(component.status === Component.Ready, component.errorString())
            var instance = component.createObject(root, props || {})
            verify(instance !== null)
            return instance
        }

        function test_component_loads() {
            var instance = createView()
            instance.destroy()
        }

        function findObject(parent, name) {
            if (!parent)
                return null
            if (parent.objectName === name)
                return parent

            var buckets = []
            if (parent.children)
                buckets.push(parent.children)
            if (parent.contentItem && parent.contentItem.children)
                buckets.push(parent.contentItem.children)
            if (parent.data)
                buckets.push(parent.data)

            for (var bucketIndex = 0; bucketIndex < buckets.length; ++bucketIndex) {
                var bucket = buckets[bucketIndex]
                for (var i = 0; i < bucket.length; ++i) {
                    var result = findObject(bucket[i], name)
                    if (result)
                        return result
                }
            }
            return null
        }

        function test_lazy_load_remote_tabs() {
            playlistVm.loadPlaylistCount = 0
            playlistVm.loadAlbumCount = 0

            var instance = createView()
            var tabBar = findObject(instance, "libraryTabBar")
            verify(tabBar !== null)
            compare(tabBar.currentIndex, 0)

            tabBar.currentIndex = 1
            compare(playlistVm.loadPlaylistCount, 1)
            tabBar.currentIndex = 2
            compare(playlistVm.loadAlbumCount, 1)
            tabBar.currentIndex = 1
            compare(playlistVm.loadPlaylistCount, 1)

            instance.destroy()
        }

        function test_create_playlist_dialog_flow() {
            playlistVm.createPlaylistCount = 0
            playlistVm.lastCreatedName = ""

            var instance = createView()
            var newPlaylistButton = findObject(instance, "newPlaylistButton")
            var createDialog = findObject(instance, "createDialog")
            verify(newPlaylistButton !== null)
            verify(createDialog !== null)

            newPlaylistButton.clicked()

            var field = findObject(createDialog, "playlistNameField")
            var createButton = findObject(createDialog, "createPlaylistButton")
            verify(field !== null)
            verify(createButton !== null)
            field.text = "Focus Mix"
            createButton.clicked()

            compare(playlistVm.createPlaylistCount, 1)
            compare(playlistVm.lastCreatedName, "Focus Mix")

            instance.destroy()
        }
    }
}
