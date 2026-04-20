#pragma once
#include <QDialog>
#include <QPainter>
#include "watermarkwidget.h"

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* e) override;
};
