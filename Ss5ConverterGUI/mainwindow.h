#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <qglobal.h>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setText_to_List(QStringList list);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QProcess *cnvProcess;


private slots:
    void processErrOutput();
    void processFinished( int exitCode, QProcess::ExitStatus exitStatus);
    void on_checkBox_toggled(bool checked);
};

#endif // MAINWINDOW_H
