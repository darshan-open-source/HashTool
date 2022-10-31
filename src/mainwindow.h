#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QBoxLayout>
#include<QRadioButton>
#include<QComboBox>
#include<thread>
#include<QLabel>
#include<qlineedit.h>
#include<thread>

#include<QPushButton>
#include<openssl/evp.h>
#include<qfiledialog.h>
#include<QTextBrowser>
#define tb QBoxLayout::TopToBottom
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     void init();
     void end();
     void addhash();
     std::string Filename;
    QBoxLayout *mainlayut,*hashselector,*type,*sublayout,*browselayout,* buttonlayout;
    QLabel *hashalgo,*typelabel,*name;
    QComboBox *hashcombobox;
    QRadioButton *text,*r1;
    QTextBrowser *textbrowser;
    QPushButton *Browse,*calculate,*copytoclipboard;
    QLineEdit *edit;
    QFileDialog *diag;

private:
    Ui::MainWindow *ui;
    static void threadfunction(MainWindow*,QString);

 const   QStringList *hashlist;
     void signalattach();
     void thread2(QString s);

public slots:
 void checked(bool);
 void calculateclicked();
 void copytoclipboardclicked();
     void updatehash(std::string);
 void combochanged(int);
 void browseclicked();
 void itemselected(QString);

signals:
 void m(std::string);
};
#endif // MAINWINDOW_H
