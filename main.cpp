#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QComboBox>
#include <QTextToSpeech>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <iostream>

class TalkingClock : public QMainWindow {
    Q_OBJECT

public:
    TalkingClock(QWidget *parent = nullptr)
        : QMainWindow(parent), tts(new QTextToSpeech(this)), trayIcon(new QSystemTrayIcon(this)) {
        // Setup main window
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        timeLabel = new QLabel(this);
        timeLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(timeLabel);

        voiceSelector = new QComboBox(this);
       // for (const QTextToSpeech::Voice &voice : tts->availableVoices()) {
       //     voiceSelector->addItem(voice.name());
       // }
      //  connect(voiceSelector, &QComboBox::currentIndexChanged, this, &TalkingClock::changeVoice);
        layout->addWidget(voiceSelector);

        setCentralWidget(centralWidget);
        setWindowTitle("Talking Clock");
        resize(300, 150);

        // Initialize the timer
        connect(&timer, &QTimer::timeout, this, &TalkingClock::updateTime);
        timer.start(1000); // Update every second
        scheduleNextAnnouncement();

        // Setup tray icon and menu
        QMenu *trayMenu = new QMenu();
        QAction *quitAction = new QAction("Quit", this);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
        trayMenu->addAction(quitAction);

        trayIcon->setIcon(QIcon("icon.png")); // Replace with an actual icon path
        trayIcon->setContextMenu(trayMenu);
        trayIcon->setToolTip("Talking Clock");
        trayIcon->show();

        connect(trayIcon, &QSystemTrayIcon::activated, this, &TalkingClock::onTrayIconActivated);
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        if (trayIcon->isVisible()) {
            hide();
            event->ignore();
        }
    }

private slots:
    void updateTime() {
        QTime currentTime = QTime::currentTime();
        timeLabel->setText(currentTime.toString("h:mm:ss AP"));
    }

    void announceTime() {
        // Get the current time
        QTime currentTime = QTime::currentTime();

        // Generate the announcement text
        QString timeString = currentTime.toString("h:mm AP");
        QString announcement = QString("The time is now %1.").arg(timeString);

        // Speak the time
        tts->say(announcement);
        std::cout << "[Announcement]: " << announcement.toStdString() << std::endl;

        // Schedule the next announcement
        scheduleNextAnnouncement();
    }

    void changeVoice(int index) {
        auto voices = tts->availableVoices();
        if (index >= 0 && index < voices.size()) {
            tts->setVoice(voices.at(index));
        }
    }

    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            showNormal();
            activateWindow();
        }
    }

private:
    void scheduleNextAnnouncement() {
        // Get the current time
        QTime currentTime = QTime::currentTime();

        // Calculate the next announcement time (hour or half hour)
        int minutesToNextHalfHour = (30 - (currentTime.minute() % 30)) % 30;
        QTime nextAnnouncement = currentTime.addSecs(minutesToNextHalfHour * 60).addSecs(-currentTime.second());

        // Calculate the time interval to the next announcement
        int interval = currentTime.secsTo(nextAnnouncement) * 1000; // milliseconds

        // Restart the timer
        QTimer::singleShot(interval, this, &TalkingClock::announceTime);

        std::cout << "[Next Announcement Scheduled]: " << nextAnnouncement.toString("h:mm AP").toStdString() << std::endl;
    }

    QTimer timer;
    QLabel *timeLabel;
    QComboBox *voiceSelector;
    QTextToSpeech *tts;
    QSystemTrayIcon *trayIcon;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TalkingClock clock;
    clock.show();

    return app.exec();
}

#include "main.moc"
