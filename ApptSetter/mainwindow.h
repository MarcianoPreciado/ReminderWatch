#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();

    void on_nextButton_clicked();

    void on_previousButton_clicked();

    void on_finishButton_clicked();

    void on_timeEdit_editingFinished();

    void on_dateEdit_editingFinished();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
