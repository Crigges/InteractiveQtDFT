#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DFTMainWindow.h"
#include "DFTEditor.h"

class DFTMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    DFTEditor* dftEditor;
    DFTMainWindow(QWidget *parent = Q_NULLPTR);
private:
    void updateImages();
    Ui::DFTMainWindowClass ui;
};
