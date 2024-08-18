import QtQuick 1.0
import com.nokia.meego 1.1

SelectionDialog{
    selectedIndex: pageSwitcher.currentPage - 1
    titleText: "Choose a page:"
    model: pagesModel
    onAccepted: {
        pageSwitcher.setPage(selectedIndex+1)
    }
}
