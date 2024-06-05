// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id: im
    signal installationFinished()
    signal updatingRepositoresFinished()
    signal statusChanged()
    signal repeatCountChanged()

    property variant list: ["apple", "banana", "cherry"]
    property string installation_status: ""
    property int repeatCount: 0

    function getRandomElement(list) {
        var index = Math.floor(Math.random() * list.length);
        return list[index];
    }

    function setOnTriggered(timer, func) {
        timer.triggered.connect(func);
    }

    Timer {
        id: mainTimer
        interval: 800
        repeat: true
    }
    function test_install() {
        setOnTriggered(mainTimer, function() {
                           repeatCount += 1
                           im.repeatCountChanged()
                           if (repeatCount === 8) {
                               im.updatingRepositoresFinished()
                               setOnTriggered(mainTimer, function() {
                                                  if (repeatCount === 16) {
                                                      mainTimer.running = false
                                                      im.installationFinished()
                                                  }
                                              });
                           }
                       });
        mainTimer.running = true
    }
}
