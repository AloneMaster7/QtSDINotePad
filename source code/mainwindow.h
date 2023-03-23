#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTextEdit* te;
    bool TextChanged;
    QFile* f;
    QString progName;
public:
    MainWindow(QString progName,QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *);
    bool isSafeToClose();
protected slots:
    void NewFile();
    void OpenFile();
    void SaveFile();
    void SaveAsFile();
    void SetFontTriggered();
    void about();
    void exitTriggered();
    void textChanged();
};
#endif // MAINWINDOW_H
