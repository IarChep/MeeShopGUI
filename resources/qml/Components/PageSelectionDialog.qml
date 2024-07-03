import QtQuick 1.0
import com.nokia.meego 1.1

SelectionDialog{
    selectedIndex: pageSwitcher.currentPage - 1
    titleText: "Choose a page:"
    model: pagesModel
    onAccepted: {
        switch (pageSwitcher.currentPage < selectedIndex + 1) {
        case true:
            pageSwitcher.currentPage = selectedIndex + 1
            pageSwitcher.shift -= 180 * (Math.floor(pageSwitcher.currentPage / 4)) + pageSwitcher.shift
            if(pageSwitcher.currentPage > 2 && pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 < 3) {
                pageSwitcher.shift += 45 * (3 - (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45))
            } else if (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 === 0) {
                pageSwitcher.shift -= 45
            }
            break
        case false:
            pageSwitcher.currentPage = selectedIndex + 1
            pageSwitcher.shift -= pageSwitcher.shift + (180 * (Math.floor(pageSwitcher.currentPage / 4)))
            if(pageSwitcher.currentPage > 2 && pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 > 2) {
                pageSwitcher.shift -= 45 * ((pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45) - 2)
            } else if (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 === 0) {
                pageSwitcher.shift += 90
            }
            break
        }
        pageSwitcher.pageChanged()
    }
}
