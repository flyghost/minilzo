#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "buttonedit.h"
#include <QTextStream>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void buttonEditInit();
    ButtonEdit *buttonEdit1;
    QTextStream *filestream;
    QFile *file;

private:
    uint16_t crc16_le(uint16_t crc, const uint8_t *buf, uint32_t len);
    uint32_t crc32_le(uint32_t crc, const uint8_t *buf, uint32_t len);
    uint8_t crc8_le(uint8_t crc, uint8_t const *p, uint32_t len);
    int miniz_compress_file(int fd_in, int fd_out);
    int miniz_decompress_file(int fd_in, int fd_out);

    void compress_file();
    void decompress_file();


private slots:
    void path_lineEdit1_Clicked();
    void compressButton_Clicked();
    void decompressButton_clicked();
};
#endif // MAINWINDOW_H
