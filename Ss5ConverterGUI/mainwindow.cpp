#include "mainwindow.h"
#include "ui_mainwindow.h"

QString fileName;       //コンバートするsspjファイル
QString cnvOutputStr;   //コンバート結果
QString sspjPathStr;    //引数で渡されるSSPJのパス
QString OpenPathStr;    //ダイアログで開くパス
QString execPathStr;    //実行しているコンバータGUIのパス

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //フォームの部品にアクセスする場合はuiのメンバを経由する
    ui->setupUi(this);

    //コンバートファイル名の初期化
    fileName.clear();

    cnvProcess = new QProcess(this);
    // プロセスが終了した時に finished シグナル発信
    connect(cnvProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus )));
    // プロセスからエラー出力があって読み込み可能になったら readyReadStandardError シグナル発信
    connect(cnvProcess, SIGNAL(readyReadStandardError()), this, SLOT(processErrOutput()));

    //ウィンドウのタイトルをつける
    setWindowTitle("Ss5Converter GUI");

    sspjPathStr.clear();
    OpenPathStr.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setText_to_List(QStringList list)
{
    //実行ファイルのパスを保存
    execPathStr = list[0];

    if ( list.length() > 1 )
    {
        sspjPathStr = list[1];
        fileName = sspjPathStr;
        if (fileName.isEmpty())
        {
            //ファイル名なし
        }
        else
        {
            ui->textBrowser_2->setText(fileName);
        }
    }
    if ( list.length() > 2 )
    {
        OpenPathStr = list[2];
    }
}


void MainWindow::on_pushButton_clicked()
{
    QString SelectfileName = QFileDialog::getOpenFileName(this,
            tr("Open SpriteStudio5 Project"),   //タイトル
            OpenPathStr, //ディレクトリの指定
            tr("Project files (*.sspj)"));  //フィルタ

    if (SelectfileName.isEmpty())
    {
        //ファイル名なし
    }
    else
    {
        fileName = SelectfileName;
        ui->textBrowser_2->setText(fileName);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    //コンバータの起動
    if (fileName.isEmpty())
    {
        //ファイル名なし
    }
    else
    {
        QString str;
        QString execstr;

#ifdef Q_OS_WIN32
        // Windows
        execstr = "Ss5Converter.exe";
#else
        // Mac
        QDir dir = QDir(execPathStr);
        dir.cd("..");
        dir.cd("..");
        dir.cd("..");
        dir.cd("..");
        QString str_current_path = dir.path();
        execstr = str_current_path + "/Ss5Converter";
#endif
        if ( ui->checkBox->checkState() == Qt::Checked )
        {
            //チェックされているとき
            str = execstr + " -p " + ui->lineEdit->text() + " " + fileName;
        }
        else
        {
            str = execstr + " " + fileName;
        }
        cnvProcess->start(str); //パスと引数

        ui->textBrowser_3->setText(tr("Exec"));     //ステータス
        ui->textBrowser->setText(tr(""));           //エラー
        cnvOutputStr = "";
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    //アプリケーションの終了
    exit(0);
}

void MainWindow::processErrOutput()
{
    // 出力を全て取得
    QByteArray output = cnvProcess->readAllStandardError();
    cnvOutputStr = cnvOutputStr + QString::fromLocal8Bit( output );
    ui->textBrowser->setText(cnvOutputStr);
}
void MainWindow::processFinished( int exitCode, QProcess::ExitStatus exitStatus)
{
    if ( exitStatus == QProcess::CrashExit )
    {
//        QMessageBox::warning( this, tr("Error"), tr("Crashed") );
        ui->textBrowser_3->setText(tr("Error"));
    }
    else if ( exitCode != 0 )
    {
//        QMessageBox::warning( this, tr("Error"), tr("Failed") );
        ui->textBrowser_3->setText(tr("Error"));   //ステータス
    }
    else
    {
        // 正常終了時の処理
        ui->textBrowser_3->setText(tr("Convert Success!"));
//    QMessageBox::information(this, tr("Ss5Converter"), tr("Convert success"));
    }
}



//チェックボックスの状態が変化した
void MainWindow::on_checkBox_toggled(bool checked)
{
    ui->lineEdit->setEnabled(checked);
}
