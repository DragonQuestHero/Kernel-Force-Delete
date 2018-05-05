/********************************************************************************
** Form generated from reading UI file 'Kernel_Force_Delete_R3.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KERNEL_FORCE_DELETE_R3_H
#define UI_KERNEL_FORCE_DELETE_R3_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Kernel_Force_Delete_R3Class
{
public:
    QWidget *centralWidget;
    QPushButton *Delete_pushButton;
    QPushButton *Unlock_pushButton;
    QTableView *tableView;

    void setupUi(QMainWindow *Kernel_Force_Delete_R3Class)
    {
        if (Kernel_Force_Delete_R3Class->objectName().isEmpty())
            Kernel_Force_Delete_R3Class->setObjectName(QStringLiteral("Kernel_Force_Delete_R3Class"));
        Kernel_Force_Delete_R3Class->resize(400, 413);
        centralWidget = new QWidget(Kernel_Force_Delete_R3Class);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        Delete_pushButton = new QPushButton(centralWidget);
        Delete_pushButton->setObjectName(QStringLiteral("Delete_pushButton"));
        Delete_pushButton->setGeometry(QRect(21, 371, 121, 31));
        Unlock_pushButton = new QPushButton(centralWidget);
        Unlock_pushButton->setObjectName(QStringLiteral("Unlock_pushButton"));
        Unlock_pushButton->setGeometry(QRect(250, 370, 121, 31));
        tableView = new QTableView(centralWidget);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setGeometry(QRect(10, 10, 381, 351));
        Kernel_Force_Delete_R3Class->setCentralWidget(centralWidget);

        retranslateUi(Kernel_Force_Delete_R3Class);

        QMetaObject::connectSlotsByName(Kernel_Force_Delete_R3Class);
    } // setupUi

    void retranslateUi(QMainWindow *Kernel_Force_Delete_R3Class)
    {
        Kernel_Force_Delete_R3Class->setWindowTitle(QApplication::translate("Kernel_Force_Delete_R3Class", "Kernel_Force_Delete_R3", Q_NULLPTR));
        Delete_pushButton->setText(QApplication::translate("Kernel_Force_Delete_R3Class", "Delete", Q_NULLPTR));
        Unlock_pushButton->setText(QApplication::translate("Kernel_Force_Delete_R3Class", "Unlock", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Kernel_Force_Delete_R3Class: public Ui_Kernel_Force_Delete_R3Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KERNEL_FORCE_DELETE_R3_H
