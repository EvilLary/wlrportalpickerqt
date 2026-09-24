import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import WlrPortalPicker

ApplicationWindow {
    title: qsTr("Select Window or Screen")

    visible: true
    width: 500
    height: 400
    header: ToolBar {
        Label {
            anchors.fill: parent
            text: qsTr("Select Window or Screen")
            verticalAlignment: Qt.AlignVCenter

            elide: Label.ElideRight
            font.pointSize: 14
        }
    }

    // Connections {
    //     target: Backend.outputs
    //     function onSupportedChanged(): void {
    //         print(Backend.outputs.supported)
    //     }
    // }

    Page {
        anchors.fill: parent
        header: TabBar {
            id: tabbar
            currentIndex: 0
            TabButton {
                text: qsTr("Screens")
            }
            TabButton {
                text: qsTr("Windows")
            }
        }

        contentItem: StackLayout {
            currentIndex: tabbar.currentIndex

            View {
                model: Backend.outputs
                placeholderText: "zxdg_output isn't supported"
                placeholderVisible: !Backend.outputs.supported
                delegate: ItemDelegate {
                    required property string description
                    required property string name

                    width: ListView.view.width
                    height: implicitHeight
                    text: description
                    // text: modelData ? `${modelData.name} - ${modelData.model}` : ""

                    onClicked: Backend.selectScreen(name)
                }
            }

            View {
                model: Backend.toplevels
                delegate: ItemDelegate {
                    // FIXME: filter it out in the cpp side
                    visible: appId !== Application.name
                    required property string title
                    required property string appId
                    required property string identifier

                    width: ListView.view.width
                    height: implicitHeight
                    text: title
                    icon.name: appId

                    onClicked: Backend.selectWindow(identifier)
                }
            }

        }
    }

    component View: ListView {
        property alias placeholderText: placeholderMessage.text
        property alias placeholderVisible: placeholderMessage.visible
        boundsBehavior: Flickable.StopAtBounds
        Label {
            id: placeholderMessage
            anchors.centerIn: parent
            horizontalAlignment: Qt.AlignHCenter

            width: parent.width - 16
            font.pointSize: 16
            opacity: 0.8
            visible: false
        }
    }
}
