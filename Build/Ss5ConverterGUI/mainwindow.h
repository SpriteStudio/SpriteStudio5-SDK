#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDropEvent>
#include <QProcess>
#include <QUrl>
#include <QMimeData>
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
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_pushButton_exit_clicked();

    void on_pushButton_listclear_clicked();

    void on_pushButton_convert_clicked();

private:
    Ui::MainWindow *ui;
    QProcess *cnvProcess;

private slots:
    void processErrOutput();
    void processFinished( int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // MAINWINDOW_H
