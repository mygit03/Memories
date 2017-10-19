#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QSqlTableModel>
#include <QComboBox>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void createDB();

private slots:
    void slot_add();            //添加
    void slot_del();            //删除
    void slot_ok();             //提交
    void slot_cancel();         //撤销
    void slot_select();         //查询
    void slot_return();         //返回全表
    void slot_asc();            //按ID升序排序
    void slot_desc();           //按ID降序排序

private:
    Ui::MainWindow *ui;

    QSqlDatabase m_db;
    QTableView *tableView;
    QSqlTableModel *model;
    QComboBox *cBox;
    QLineEdit *lineEdit;
};

#endif // MAINWINDOW_H
