#include "MainWindow.h"
#include "EntryDialog.h"
#include "PasswordViewDialog.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QTimer>
#include <QEvent>
#include <QApplication>

MainWindow::MainWindow(const std::string& password,QWidget* parent):QMainWindow(parent),masterPassword(password){
    setWindowTitle("Password Manager");
    setAttribute(Qt::WA_DeleteOnClose);
    resize(1150,600);
    if(!manager.loadFromFile("vault.dat",masterPassword)){
        QMessageBox::critical(this,"Vault Error","The encrypted vault could not be loaded.");
        QTimer::singleShot(0,this,&MainWindow::close);
        return;
    }
    QWidget* central=new QWidget(this);
    QVBoxLayout* mainLayout=new QVBoxLayout(central);
    QHBoxLayout* toolbar=new QHBoxLayout();
    searchBox=new QLineEdit(this);
    searchBox->setPlaceholderText("Search website, username or category...");
    QPushButton* addButton=new QPushButton("Add Entry",this);
    QPushButton* editButton=new QPushButton("Edit Entry",this);
    QPushButton* viewButton=new QPushButton("View Password",this);
    QPushButton* deleteButton=new QPushButton("Delete",this);
    QPushButton* lockButton=new QPushButton("Lock Vault",this);
    toolbar->addWidget(searchBox);
    toolbar->addWidget(addButton);
    toolbar->addWidget(editButton);
    toolbar->addWidget(viewButton);
    toolbar->addWidget(deleteButton);
    toolbar->addWidget(lockButton);
    table=new QTableWidget(this);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"ID","Website","Username","Category","Notes"});
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setStretchLastSection(true);
    mainLayout->addLayout(toolbar);
    mainLayout->addWidget(table);
    setCentralWidget(central);
    connect(searchBox,&QLineEdit::textChanged,this,&MainWindow::searchEntries);
    connect(addButton,&QPushButton::clicked,this,&MainWindow::addEntry);
    connect(editButton,&QPushButton::clicked,this,&MainWindow::editEntry);
    connect(viewButton,&QPushButton::clicked,this,&MainWindow::viewPassword);
    connect(deleteButton,&QPushButton::clicked,this,&MainWindow::deleteEntry);
    connect(lockButton,&QPushButton::clicked,this,&MainWindow::lockVault);
    inactivityTimer=new QTimer(this);
    inactivityTimer->setSingleShot(true);
    inactivityTimer->setInterval(120000);
    connect(inactivityTimer,&QTimer::timeout,this,&MainWindow::lockVault);
    qApp->installEventFilter(this);
    resetInactivityTimer();
    loadEntries();
}

MainWindow::~MainWindow(){
    clearSensitiveData();
    qApp->removeEventFilter(this);
}

void MainWindow::clearSensitiveData(){
    if(!masterPassword.empty()){
        volatile char* data=masterPassword.data();
        for(size_t i=0;i<masterPassword.size();i++)
            data[i]='\0';
        masterPassword.clear();
        masterPassword.shrink_to_fit();
    }
}

void MainWindow::loadEntries(){
    table->setRowCount(0);
    const auto& entries=manager.getEntries();
    for(const auto& entry:entries){
        int row=table->rowCount();
        table->insertRow(row);
        table->setItem(row,0,new QTableWidgetItem(QString::number(entry.getId())));
        table->setItem(row,1,new QTableWidgetItem(QString::fromStdString(entry.getWebsite())));
        table->setItem(row,2,new QTableWidgetItem(QString::fromStdString(entry.getUsername())));
        table->setItem(row,3,new QTableWidgetItem(QString::fromStdString(entry.getCategory())));
        table->setItem(row,4,new QTableWidgetItem(QString::fromStdString(entry.getNotes())));
    }
    table->resizeColumnsToContents();
}

void MainWindow::searchEntries(){
    QString query=searchBox->text();
    if(query.isEmpty()){
        loadEntries();
        return;
    }
    auto results=manager.search(query.toStdString());
    table->setRowCount(0);
    for(const auto& entry:results){
        int row=table->rowCount();
        table->insertRow(row);
        table->setItem(row,0,new QTableWidgetItem(QString::number(entry.getId())));
        table->setItem(row,1,new QTableWidgetItem(QString::fromStdString(entry.getWebsite())));
        table->setItem(row,2,new QTableWidgetItem(QString::fromStdString(entry.getUsername())));
        table->setItem(row,3,new QTableWidgetItem(QString::fromStdString(entry.getCategory())));
        table->setItem(row,4,new QTableWidgetItem(QString::fromStdString(entry.getNotes())));
    }
    table->resizeColumnsToContents();
}

void MainWindow::addEntry(){
    EntryDialog dialog(this);
    if(dialog.exec()!=QDialog::Accepted)
        return;
    manager.addEntry(dialog.getWebsite(),dialog.getUsername(),dialog.getPassword(),dialog.getCategory(),dialog.getNotes());
    if(!manager.saveToFile("vault.dat",masterPassword)){
        QMessageBox::warning(this,"Save Error","Failed to save the encrypted vault.");
        return;
    }
    loadEntries();
}

void MainWindow::editEntry(){
    int row=table->currentRow();
    if(row<0){
        QMessageBox::information(this,"Edit Entry","Please select an entry first.");
        return;
    }
    QTableWidgetItem* idItem=table->item(row,0);
    if(idItem==nullptr)
        return;
    int id=idItem->text().toInt();
    PasswordEntry* entry=manager.findById(id);
    if(entry==nullptr){
        QMessageBox::warning(this,"Edit Error","The selected entry could not be found.");
        return;
    }
    EntryDialog dialog(this);
    dialog.setEntry(entry->getWebsite(),entry->getUsername(),entry->getPassword(),entry->getCategory(),entry->getNotes());
    if(dialog.exec()!=QDialog::Accepted)
        return;
    if(!manager.updateEntry(id,dialog.getWebsite(),dialog.getUsername(),dialog.getPassword(),dialog.getCategory(),dialog.getNotes())){
        QMessageBox::warning(this,"Update Error","Failed to update the selected entry.");
        return;
    }
    if(!manager.saveToFile("vault.dat",masterPassword)){
        QMessageBox::warning(this,"Save Error","Entry was updated, but the vault could not be saved.");
        return;
    }
    loadEntries();
}

void MainWindow::viewPassword(){
    int row=table->currentRow();
    if(row<0){
        QMessageBox::information(this,"View Password","Please select an entry first.");
        return;
    }
    QTableWidgetItem* idItem=table->item(row,0);
    if(idItem==nullptr)
        return;
    int id=idItem->text().toInt();
    PasswordEntry* entry=manager.findById(id);
    if(entry==nullptr){
        QMessageBox::warning(this,"Error","The selected entry could not be found.");
        return;
    }
    PasswordViewDialog dialog(entry->getWebsite(),entry->getUsername(),entry->getPassword(),this);
    dialog.exec();
}

void MainWindow::deleteEntry(){
    int row=table->currentRow();
    if(row<0){
        QMessageBox::information(this,"Delete Entry","Please select an entry first.");
        return;
    }
    QTableWidgetItem* idItem=table->item(row,0);
    if(idItem==nullptr)
        return;
    int id=idItem->text().toInt();
    if(QMessageBox::question(this,"Delete Entry","Delete selected entry?")!=QMessageBox::Yes)
        return;
    if(manager.deleteEntry(id)){
        if(!manager.saveToFile("vault.dat",masterPassword)){
            QMessageBox::warning(this,"Save Error","Entry was deleted, but the vault could not be saved.");
            return;
        }
        loadEntries();
    }
}

void MainWindow::resetInactivityTimer(){
    if(inactivityTimer)
        inactivityTimer->start();
}

bool MainWindow::eventFilter(QObject* watched,QEvent* event){
    if(watched!=this&&isActiveWindow()){
        switch(event->type()){
            case QEvent::MouseMove:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
            case QEvent::Wheel:
                resetInactivityTimer();
                break;
            default:
                break;
        }
    }
    return QMainWindow::eventFilter(watched,event);
}

void MainWindow::lockVault(){
    if(inactivityTimer)
        inactivityTimer->stop();
    qApp->removeEventFilter(this);
    clearSensitiveData();
    emit vaultLocked();
    close();
}