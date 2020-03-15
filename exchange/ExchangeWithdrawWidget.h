#ifndef EXCHANGEWITHDRAWWIDGET_H
#define EXCHANGEWITHDRAWWIDGET_H

#include <QWidget>


namespace Ui {
class ExchangeWithdrawWidget;
}

class ExchangeWithdrawWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExchangeWithdrawWidget(QWidget *parent = nullptr);
    ~ExchangeWithdrawWidget();

    void init();

private slots:
    void on_withdrawTableWidget_cellClicked(int row, int column);

    void on_hideZeroCheckBox_stateChanged(int arg1);

    void on_refreshBtn_clicked();

private:
    void showWithdrawList();
private:
    Ui::ExchangeWithdrawWidget *ui;

    void onAccountComboBoxCurrentIndexChanged(const QString &arg1);
    void paintEvent(QPaintEvent*);

};

#endif // EXCHANGEWITHDRAWWIDGET_H
