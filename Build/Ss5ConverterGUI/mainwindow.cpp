#include "mainwindow.h"
#include "ui_mainwindow.h"

QString execPathStr;    //実行しているコンバータGUIのパス
QString cnvOutputStr;   //コンバート結果
bool convert_exec = false;  //コンバート中か
bool convert_error = false;  //コンバートエラーが発生したか
int convet_index = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //フォームの部品にアクセスする場合はuiのメンバを経由する
    ui->setupUi(this);

    //ドラッグ＆ドロップを有効にする
    setAcceptDrops(true);

    cnvProcess = new QProcess(this);
    // プロセスが終了した時に finished シグナル発信
    connect(cnvProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus )));
    // プロセスからエラー出力があって読み込み可能になったら readyReadStandardError シグナル発信
    connect(cnvProcess, SIGNAL(readyReadStandardError()), this, SLOT(processErrOutput()));

    //ウィンドウのタイトルをつける
    setWindowTitle("Ss5ConverterGUI");

    //初期化
    convert_exec = false;
    cnvOutputStr.clear();
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
        int i;
        for ( i = 1; i < list.length(); i++ )
        {
            QString dragFilePath;
            dragFilePath = list[i];
            if ( ( dragFilePath.endsWith(".sspj")) || ( dragFilePath.endsWith(".SSPJ")) )
            {
                ui->listWidget->addItem(dragFilePath);
            }
        }
    }
}

void MainWindow::on_pushButton_exit_clicked()
{
    //アプリケーションの終了
    exit(0);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->hasUrls())
    {
        QList<QUrl> urlList = e->mimeData()->urls();
        for(int i = 0; i < urlList.size(); i++)
        {
            //ドラッグしたファイルをリストに追加
            //.sspj以外は弾く
            QString dragFilePath;
            dragFilePath = urlList[i].toLocalFile();
            if ( ( dragFilePath.endsWith(".sspj")) || ( dragFilePath.endsWith(".SSPJ")) )
            {
                //同じ名前がリストにある場合は弾く
                bool addname = true;
                int j = 0;
                for ( j = 0; j < ui->listWidget->count(); j++ )
                {
                    QString fileName = ui->listWidget->item(j)->text();
                    if ( fileName == dragFilePath )
                    {
                        addname = false;
                        break;
                    }

                }
                if ( addname == true )
                {
                    ui->listWidget->addItem(dragFilePath);
                }
            }
        }
    }
}

void MainWindow::on_pushButton_listclear_clicked()
{
    //リストクリア
    ui->listWidget->clear();
}

void MainWindow::on_pushButton_convert_clicked()
{
    //コンバート

    if (( ui->listWidget->count() > 0 ) && (convert_exec == false))
    {
        convert_error = false;
        convert_exec = false;  //コンバート中か
        convet_index = 0;
        QString st = QString("Exec %1/%2").arg(convet_index+1).arg(ui->listWidget->count());
        ui->textBrowser_status->setText(st);     //ステータス
        ui->textBrowser_err->setText(tr(""));           //エラー
        cnvOutputStr = "";

        {
            QString fileName = ui->listWidget->item(convet_index)->text();
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
                str = execstr + " \"" + fileName + "\"";
                cnvProcess->start(str); //パスと引数

                convert_exec = true;  //コンバート中か
                convet_index++;
            }
        }
    }
}

void MainWindow::processErrOutput()
{
    // 出力を全て取得
    QByteArray output = cnvProcess->readAllStandardError();
    cnvOutputStr = cnvOutputStr + QString::fromLocal8Bit( output );
    ui->textBrowser_err->setText(cnvOutputStr);
}
void MainWindow::processFinished( int exitCode, QProcess::ExitStatus exitStatus)
{
    if ( exitStatus == QProcess::CrashExit )
    {
//        QMessageBox::warning( this, tr("Error"), tr("Crashed") );
        cnvOutputStr = cnvOutputStr + "Error:" + ui->listWidget->item(convet_index-1)->text();
        convert_error = true;
    }
    else if ( exitCode != 0 )
    {
//        QMessageBox::warning( this, tr("Error"), tr("Failed") );
        cnvOutputStr = cnvOutputStr + "Error:" + ui->listWidget->item(convet_index-1)->text();
        convert_error = true;
    }
    else
    {
        // 正常終了時の処理
//        ui->textBrowser_status->setText(tr("Convert Success!"));
//    QMessageBox::information(this, tr("Ss5Converter"), tr("Convert success"));
    }

    if (( ui->listWidget->count() > convet_index ))
    {
        QString st = QString("Exec %1/%2").arg(convet_index+1).arg(ui->listWidget->count());
        ui->textBrowser_status->setText(st);     //ステータス
        {
            QString fileName = ui->listWidget->item(convet_index)->text();
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
                str = execstr + " \"" + fileName + "\"";
                cnvProcess->start(str); //パスと引数

                convet_index++;
            }
        }
    }
    else
    {
        convert_exec = false;  //コンバート中か
        if ( convert_error == false )
        {
            ui->textBrowser_status->setText(tr("Convert Success!"));
        }
        else
        {
            ui->textBrowser_status->setText(tr("Error"));   //ステータス
            ui->textBrowser_err->setText(cnvOutputStr);
        }
    }

}

//リストの読み込み
void MainWindow::on_pushButton_listload_clicked()
{
    QFileDialog::Options options;
    QString strSelectedFilter;
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("select list File"), ".", tr("text(*.txt)"), &strSelectedFilter, options);

    if ( fileName != "" )
    {
        //リストクリア
        ui->listWidget->clear();

        //読み込んだファイルをリストに設定
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly))//読込のみでオープンできたかチェック
        {
            return;
        }

        QTextStream in(&file);
        while ( !in.atEnd() ) {
            QString str = in.readLine();//1行読込
            ui->listWidget->addItem(str);
        }
    }

}

//リストの保存
void MainWindow::on_pushButton_listsave_clicked()
{
    QFileDialog::Options options;
    QString strSelectedFilter;
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("save list File"), ".", tr("text(*.txt)"), &strSelectedFilter, options);

    if ( fileName != "" )
    {
        //読み込んだファイルをリストに設定
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly))//読込のみでオープンできたかチェック
        {
            return;
        }

        QTextStream out(&file);
        int i;
        for ( i = 0; i < ui->listWidget->count(); i++ )
        {
            QString str = ui->listWidget->item(i)->text();
            out << str << endl; //書込み
        }
    }
}
