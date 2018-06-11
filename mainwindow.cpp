#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>

#include "itemdelegate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("事件记录"));

    createDB();

    tableView = new QTableView(this);
    model=new QSqlTableModel(this);
    model->setTable("info");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0,Qt::Horizontal,tr("名字"));
    model->setHeaderData(1,Qt::Horizontal,tr("日期"));
    model->setHeaderData(2,Qt::Horizontal,tr("地址"));
    model->setHeaderData(3,Qt::Horizontal,tr("注释"));

    tableView->setItemDelegateForColumn(1,new DateDelegate);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableView->setAlternatingRowColors(true);               //可以改变QableView颜色
    tableView->setSortingEnabled(true);                     //设置可以排序
    tableView->setModel(model);

    QAction *act_add = new QAction(QIcon(":/images/add.png"),tr("添加"));
    connect(act_add,SIGNAL(triggered(bool)),this,SLOT(slot_add()));
    QAction *act_del = new QAction(QIcon(":/images/del.png"),tr("删除"));
    connect(act_del,SIGNAL(triggered(bool)),this,SLOT(slot_del()));
    ui->mainToolBar->addAction(act_add);
    ui->mainToolBar->addAction(act_del);

    QPushButton *btn_ok = new QPushButton(tr("提交修改"),this);
    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(slot_ok()));
    QPushButton *btn_cancel = new QPushButton(tr("撤销修改"),this);
    connect(btn_cancel, SIGNAL(clicked(bool)), this, SLOT(slot_cancel()));
    QPushButton *btn_asc = new QPushButton(tr("按time升序排序"),this);
    connect(btn_asc, SIGNAL(clicked(bool)), this, SLOT(slot_asc()));
    QPushButton *btn_desc = new QPushButton(tr("按time降序排序"),this);
    connect(btn_desc, SIGNAL(clicked(bool)), this, SLOT(slot_desc()));
    QPushButton *btn_select = new QPushButton(tr("查询"),this);
    connect(btn_select, SIGNAL(clicked(bool)), this, SLOT(slot_select()));
    QPushButton *btn_return = new QPushButton(tr("返回全表"),this);
    connect(btn_return, SIGNAL(clicked(bool)), this, SLOT(slot_return()));

    QLabel *label = new QLabel(tr("查询条件:"));

    cBox = new QComboBox(this);
    QStringList strList;
    strList.clear();
    strList << tr("NAME") << tr("DATETIME") << tr("ADDRESS") << tr("NOTE");
    cBox->addItems(strList);

    lineEdit = new QLineEdit(this);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(btn_asc);
    vLayout->addWidget(btn_desc);
    vLayout->addWidget(btn_ok);
    vLayout->addWidget(btn_cancel);
    vLayout->addWidget(btn_return);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(label);
    hLayout->addWidget(cBox);
    hLayout->addWidget(lineEdit);
    hLayout->addWidget(btn_select);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(tableView,0,0,1,1);
    gridLayout->addLayout(vLayout,0,1,1,1);
    gridLayout->addLayout(hLayout,1,0,1,1);

    QWidget *widget = new QWidget(this);
    widget->setLayout(gridLayout);

    setCentralWidget(widget);
    setMinimumSize(750,500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createDB()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("memory.db");
    if(!m_db.open()){
        qDebug() << "m_db open failed!";
        return;
    }

    QString strSql = QString("CREATE TABLE IF NOT EXISTS INFO("
                             "NAME      VARCHAR PRIMARY KEY,"
                             "DATETIME  VARCHAR NOT NULL DEFAULT '',"
                             "ADDR      VARCHAR NOT NULL DEFAULT '',"
                             "NOTE      VARCHAR NOT NULL DEFAULT '');");

    QSqlQuery qry(m_db);
    if(!qry.exec(strSql)){
        qDebug() << qry.lastError();
    }
}

void MainWindow::slot_add()
{
    int rows = model->rowCount();
    model->insertRow(rows);

//    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QModelIndex index = model->index(rows,1);
    if(model->index(rows,1).data().isNull())
        model->setData(index,dateTime);
}

void MainWindow::slot_del()
{
    int curRow = tableView->currentIndex().row();

    int ok = QMessageBox::warning(this,tr("删除当前行!"),tr("你确定删除当前行吗？"),QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        model->revertAll(); //如果不删除，则撤销
    }
    else
    {
        model->removeRow(curRow);
        model->submitAll();
    }
}

void MainWindow::slot_ok()
{
    if(!model->submitAll()){
        qDebug() << "submit error!";
    }
    model->select();
}

void MainWindow::slot_cancel()
{
    model->revertAll();
    model->select();
}

void MainWindow::slot_select()
{
    QString condition = cBox->currentText();
    QString text = lineEdit->text();
    model->setFilter(QObject::tr("%1 like '%2%'").arg(condition).arg(text)); //根据姓名进行筛选
    model->select(); //显示结果
}

void MainWindow::slot_return()
{
    model->setTable("info");
    model->select();
}

void MainWindow::slot_asc()
{
    model->setSort(1,Qt::AscendingOrder); //id属性，即第0列，升序排列
    model->select();
}

void MainWindow::slot_desc()
{
    model->setSort(1,Qt::DescendingOrder);
    model->select();
}
