#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QFontDialog>
#include <QLabel>
#include <QLayout>
#include <QToolBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QProcess>

MainWindow::MainWindow(QString progName,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->TextChanged = false;

    QMenu* File = new QMenu("File"),*Edit = new QMenu("Edit"),
            *View = new QMenu("View"),*Help = new QMenu("Help");
    QAction* New = new QAction("New"),*Open = new QAction("Open"),
            *SetFont = new QAction("Font"),*About = new QAction("About");
    QAction* Exit = new QAction("Exit");
    QAction* Save = new QAction(QIcon("./16x16/save.png"),"Save");
    QAction* SaveAs = new QAction(QIcon("./16x16/saveas.png"),"Save As");

    New->setIcon(QIcon("./16x16/insert.png"));
    New->setShortcut(tr("Ctrl+N"));
    Open->setIcon(QIcon("./16x16/open2.png"));
    Open->setShortcut(tr("Ctrl+O"));
    Exit->setIcon(QIcon("./16x16/error.png"));
    SetFont->setIcon(QIcon("./16x16/edit.png"));
    About->setIcon(QIcon("./16x16/startup.png"));

    QObject::connect(New,SIGNAL(triggered()),this,SLOT(NewFile()));
    QObject::connect(Open,SIGNAL(triggered()),this,SLOT(OpenFile()));
    QObject::connect(SetFont,SIGNAL(triggered()),this,SLOT(SetFontTriggered()));
    QObject::connect(About,SIGNAL(triggered()),this,SLOT(about()));
    QObject::connect(Exit,SIGNAL(triggered()),this,SLOT(exitTriggered()));
    QObject::connect(Save,SIGNAL(triggered()),this,SLOT(SaveFile()));
    QObject::connect(SaveAs,SIGNAL(triggered()),this,SLOT(SaveAsFile()));

    File->addAction(New);
    File->addAction(Open);
    File->addAction(Save);
    File->addAction(SaveAs);
    File->addSeparator();
    File->addAction(Exit);
    View->addAction(SetFont);
    Help->addAction(About);
    this->menuBar()->addMenu(File);
    this->menuBar()->addMenu(Edit);
    this->menuBar()->addMenu(View);
    this->menuBar()->addMenu(Help);

    this->te = new QTextEdit;
    this->setCentralWidget(te);
    QObject::connect(this->te,SIGNAL(textChanged()),this,SLOT(textChanged()));

    QToolBar* tb = this->addToolBar(tr("New"));
    tb->addAction(New);
    tb->addAction(Open);
    tb->addAction(Save);
    tb->addAction(SaveAs);
    tb->addSeparator();
    tb->addAction(About);

    this->f = nullptr;
    this->progName = progName;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::NewFile()
{
    QProcess* p = new QProcess(nullptr);
    p->setProgram(this->progName);
    p->start();
}

void MainWindow::OpenFile()
{
    //opening file
    if(this->f && !this->isSafeToClose())
        return;
    if(this->f)
        this->f->close();
    QString name = QFileDialog::getOpenFileName(this,"Get Open File Name");
    if(name=="")
        return;
    delete this->f;
    this->f = new QFile(name);
    if(!this->f->open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"ERROR","There was an error while opening file");
        return;
    }
    this->setWindowTitle(this->f->fileName());
    QString c;
    QTextStream ts(this->f);
    ts>>c;
    this->te->setText(c);
    this->TextChanged = false;
    this->f->close();
}

void MainWindow::SaveFile()
{
    //save file
    if(this->TextChanged){
        if(this->f->open(QIODevice::WriteOnly)){
            QTextStream ts(this->f);
            ts<<this->te->document()->toPlainText();
            QMessageBox::information(this,"Saved","Saved Successfully");
            this->f->close();
        }else{
            QString name = QFileDialog::getSaveFileName(this,"Get Save File Name");
            if(name=="")
                return;
            if(!name.endsWith(".txt"))
                name+=".txt";
            this->f = new QFile(name);
            this->f->open(QIODevice::WriteOnly);
            QTextStream ts(this->f);
            ts<<this->te->document()->toPlainText();
            QMessageBox::information(this,"Saved","Saved Successfully");
            this->f->close();
        }
        this->TextChanged = false;
    }
}

void MainWindow::SaveAsFile()
{
    QString name = QFileDialog::getSaveFileName(this,"Save As");
    if(name=="")
        return;
    if(QMessageBox::question(this,"Save As","Do you want to overwrite the file?",
                             QMessageBox::No|QMessageBox::Yes,QMessageBox::No)
            == QMessageBox::Yes){
        if(this->f->isOpen())
            this->f->close();
        if(!this->f->open(QIODevice::WriteOnly)){
            QMessageBox::warning(this,"ERROR","There was an error while opening file");
            return;
        }
        QTextStream ts(this->f);
        ts<<this->te->document()->toPlainText();
        this->setWindowTitle(name);
        this->f->close();
    }
}

void MainWindow::SetFontTriggered()
{
    bool ok=false;
    QFont f = QFontDialog::getFont(&ok);
    if(ok){
        this->te->setFont(f);
    }
}

void MainWindow::about()
{
    QDialog* d = new QDialog(this);
    QVBoxLayout* vbl = new QVBoxLayout(d);
    QLabel* l = new QLabel(d);
    QString text = "NotePad With Qt FrameWork\n";
    text += "Programmer : AloneMaster7\n\n";
    l->setText(text);
    vbl->addWidget(l);
    text = "";
    text += "this program is like windows notepad which has written with C++ ";
    text += "and Qt5 framework. Enjoy it :)";
    l = new QLabel(d);
    l->setText(text);
    l->setWordWrap(true);
    vbl->addWidget(l);
    d->resize(200,100);
    d->show();
}

void MainWindow::exitTriggered()
{
    this->close();
}

void MainWindow::textChanged()
{
    if(!this->TextChanged && !this->windowTitle().endsWith("*")){
        this->setWindowTitle(this->windowTitle() + "*");
    }
    this->TextChanged = true;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(!this->TextChanged || this->isSafeToClose()){
        event->accept();
    }else
        event->ignore();
}

bool MainWindow::isSafeToClose()
{
    switch(QMessageBox::question(this,"Really...?!","Do you want to close?",
                          QMessageBox::Yes|QMessageBox::No,QMessageBox::No)){
    case QMessageBox::Yes:
        return true;
    case QMessageBox::No:
        return false;
    default:
        return false;
    }
}
