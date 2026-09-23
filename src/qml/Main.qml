import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import WlrPortalPicker

ApplicationWindow {
    title: qsTr("Select Window or Screen")

    visible: true
    header: ToolBar {
        Label {
            anchors.fill: parent
            text: qsTr("Select Window or Screen")
            verticalAlignment: Qt.AlignVCenter

            elide: Label.ElideRight
            font.pointSize: 14
        }
    }

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

            View {
                model: Application.screens
                delegate: ItemDelegate {
                    required property var modelData
                    required property var model

                    width: ListView.view.width
                    height: implicitHeight
                    text: modelData ? `${modelData.name} - ${modelData.model}` : ""

                    onClicked: Backend.selectScreen(modelData.name)
                }
            }
        }
    }

    component View: ScrollView {
        property alias model: listView.model
        property alias delegate: listView.delegate
        property alias placeholderText: placeholderMessage.text
        property alias placeholderVisible: placeholderMessage.visible

        contentItem: ListView {
            id: listView
        }

        Label {
            id: placeholderMessage
            anchors.centerIn: parent
            width: parent.width - 16
            font.pointSize: 16
            opacity: 0.8
            visible: false
        }
    }
}
