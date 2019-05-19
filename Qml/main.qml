import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import TaoItem 1.0
ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    menuBar: MenuBar {
        Menu {
            title: "File"
            Action {
                text: qsTr("&Open...")
                onTriggered: {
                    openFileDialog.open()
                }
            }
        }

    }
    TaoItem {
        id: taoItem
        anchors.fill: parent
    }
    FileDialog {
        id: openFileDialog
        title: "Please choose a Video file"

        nameFilters: [ "MP4 files (*.mp4)", "YUV files(*.yuv)" ]
        onAccepted: {
            taoItem.loadFile(fileUrl)
        }
    }
    footer: ToolBar {
        Row{
            anchors.fill: parent
            ToolButton {
                text: "<"

            }
        }
    }
}
