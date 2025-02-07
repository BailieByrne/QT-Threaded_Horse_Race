#include "mainwindow.h"
#include "horse.h"

#include <iostream>
#include <vector>
#include <thread>

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTimer>
#include <QLineEdit>
#include <QBoxLayout>
#include <QProgressBar>

class HorseProgressBar : public QWidget{
    public:
        HorseProgressBar(std::string horseName, int finalDistance){
            this->horseName = horseName;
            this->finalDistance = finalDistance;
            this->distance = 0;
            this->progressBar = new QProgressBar(this);
            this->progressBar->setGeometry(10, 10, 100, 30);
            this->progressBar->setMaximum(finalDistance);
            this->progressBar->setValue(0);
        }

        void display(){
            while (distance < finalDistance) {
                distance = 0;
                progressBar->setValue(distance);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

    QProgressBar *progressBar;

    private:
        std::string horseName;
        int distance;
        int finalDistance;
    };



class MyOtherWindow : public QWidget {
    public:
        MyOtherWindow() {
            resize(320, 240);
            setWindowTitle("Main Window");

            headerlabel = new QLabel("Hello, World!", this);
            headerlabel->setGeometry(10, 10, 100, 30);

            racebutton = new QPushButton("RACE", this);
            racebutton->setGeometry(10, 210, 100, 30);
            connect(racebutton, &QPushButton::clicked, this, &MyOtherWindow::test);

            horseNameInputFiled = new QLineEdit("", this);
            horseNameInputFiled->setPlaceholderText("Horse Name");
            horseNameInputFiled->setGeometry(10, 130, 100, 30);


            addhorsebutton = new QPushButton("Add Horse", this);
            addhorsebutton->setGeometry(10, 170, 100, 30);
            connect(addhorsebutton, &QPushButton::clicked, this, [this]() {
                addhorse(horseNameInputFiled->text().toStdString());
            });

            mainBox = new QVBoxLayout(this);
            mainBox->addWidget(headerlabel);
            mainBox->addWidget(racebutton);
            mainBox->addWidget(addhorsebutton);
            mainBox->addWidget(horseNameInputFiled);
        }

        void test() {

            if (winners->size() > 0) {
                winners->clear();
            }

            for (Horse& horse : *horses) {
                HorseProgressBar *progressBar = new HorseProgressBar(horse.getName(), 20);
                progressBar->progressBar->setFormat(QString::fromStdString(horse.getName()) + " %p%");
                progressBar->progressBar->setFixedHeight(25);
                progressBar->progressBar->setStyleSheet("QProgressBar::chunk { background-color: green; }");
                mainBox->addWidget(progressBar->progressBar);

                std::thread([progressBar, &horse, this]() {
                    while (!horse.HasFinished()) {
                        QMetaObject::invokeMethod(progressBar->progressBar, "setValue", Q_ARG(int, horse.getDistance()));
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                    QMetaObject::invokeMethod(progressBar->progressBar, "setValue", Q_ARG(int, 20));
                    winners->push_back(horse.getName());
                }).detach();
            }

            std::cout << "RACING" << std::endl;

            for (Horse& horse : *horses) {
                threads->emplace_back([&horse]() {
                    horse.run();
                });
            }

            std::thread([this]() {
                for (auto& thread : *threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }

                if (!winners->empty()) {
                    std::cout << "Winner: " << winners->front() << std::endl;
                    for (Horse& horse : *horses) {
                        horse.reset();
                    }
                    threads->clear();
                    
                    std::cout << "All threads killed and horses reset." << std::endl;
                }
            }).detach();
        }

        void addhorse(std::string horseName){
            horses->emplace_back(horseName, 20);
            std::cout << "{";
            for (size_t i = 0; i < horses->size(); ++i) {
                std::cout << (*horses)[i].getName();
                if (i < horses->size() - 1) {
                    std::cout << ",";
                }
            }
            std::cout << "}" << std::endl;
        }

    private:
        QVBoxLayout *mainBox;
        QProgressBar *progressBar;
        QLabel *headerlabel;
        QPushButton *racebutton;
        QPushButton *addhorsebutton;
        QLineEdit *horseNameInputFiled;
        std::vector<Horse> *horses = new std::vector<Horse>();
        std::vector<std::thread> *threads = new std::vector<std::thread>();
        std::vector<std::string> *winners = new std::vector<std::string>();
    };

class MyWindow : public QWidget {
    public:
        MyWindow() {
            resize(320, 240);
            setWindowTitle("Simple GUI");

            headerlabel = new QLabel("Hello, World!", this);
            headerlabel->setGeometry(10, 10, 100, 30);

            loginbutton = new QPushButton("Login", this);
            connect (loginbutton, &QPushButton::clicked, this, &MyWindow::submitlogin);
            loginbutton->setGeometry(10, 210, 100, 30);

            killbutton = new QPushButton("Kill", this);
            connect (killbutton, &QPushButton::clicked, this, &MyWindow::kill);
            killbutton->setGeometry(10, 170, 100, 30);

            label = new QLabel("Hello, World!", this);
            label->setGeometry(10, 90, 100, 30);

            loginfield = new QLineEdit("", this);
            loginfield->setPlaceholderText("Login");
            loginfield->setGeometry(10, 130, 100, 30);

            logincontainer = new QVBoxLayout(this);
            logincontainer->addWidget(headerlabel);
            logincontainer->addWidget(loginfield);
            logincontainer->addWidget(label);
            logincontainer->addWidget(loginbutton);
            logincontainer->addWidget(killbutton);
            setLayout(logincontainer);



            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, &MyWindow::nothing);
            timer->start(100); // check every 100 ms
        }

        void nothing() {
        }

        void kill(){
            timer->stop();
            this->close();
        }

        void submitlogin() {
            if (loginfield->text() == "admin") {
            mainwindow = new MyOtherWindow();
            mainwindow->show();
            this->close();
            } else {
            label->setText("Login failed");
            }
        }

    private:
        MyOtherWindow *mainwindow;
        QVBoxLayout *logincontainer;
        QLabel *headerlabel;
        QPushButton *killbutton;
        QPushButton *loginbutton;
        QLabel *label;
        QTimer *timer;
        QLineEdit *loginfield;
    };




int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MyWindow window; //Create Main Window
    window.show(); // Show this Window

    return app.exec();
}
