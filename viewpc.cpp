#include "viewpc.h"
#include "ui_viewpc.h"
#include <QThread>
#include <QDesktopServices>

ViewPC::ViewPC(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewPC)
{
    ui->setupUi(this);
}

ViewPC::~ViewPC()
{
    delete ui;
}

void ViewPC::on_encryptMode_clicked()
{
    // Encrypt radio button clicked
    setEncryptMode(true);
}

void ViewPC::on_decryptMode_clicked()
{
    // Decrypt radio button clicked
    setEncryptMode(false);
}
/*!
 * \brief ViewPC::on_fileButton_clicked Slot to be called, when according button is pressed.
 */
void ViewPC::on_fileButton_clicked()
{
    // Opening QFileDialog depending on isEncrypt
    if(isEncrypt)
        inputFileName = QFileDialog::getOpenFileName(this, tr("Select file"), "/untitled.txt", tr("Text files (*.txt);;All Files (*)"));
    else
        inputFileName = QFileDialog::getOpenFileName(this, tr("Select file"), "/untitled.png", tr("PNG files (*.png);;All Files (*)"));
    // Display the file name
    ui->fileLabel->setText(inputFileName.isEmpty() ? "File not chosen" : inputFileName);
}
/*!
 * \brief ViewPC::on_startButton_clicked Slot to be called, when Start Button is pressed.
 * \section encrypting Encrypting
 * If Encrypting mode is active the data from text browser or from file from file selector
 * will be opened and checked in size.
 * \note File size limit is 16MB
 *
 * Then the EncryptDialog opens and image and key is selected.
 * Then the ViewPC::encrypt signal is called to start ModelPC::encrypt
 * \section decrypting Decrypting
 * Else, the image from file selector is transmitted to ModelPC::decrypt
 */
void ViewPC::on_startButton_clicked()
{
    if(isEncrypt)
    {
        // Getting the data
        QString text = ui->text->toPlainText();
        QByteArray data;
        if(text.isEmpty()) {
            if(inputFileName.isEmpty()) {
                alert("No input file or text was not given. Cannot continue!", true);
                return;
            }
            // Opening the file
            QFile file(inputFileName);
            if (!file.open(QIODevice::ReadOnly))
            {
                alert("Cannot open file. Cannot continue!", true);
                return;
            }
            // Check the data size
            auto size = file.size();
            if(size > pow(2, 24)) {
                alert("Your file is too big, our systems can handle it, but it requires a lot of time. We decline.", true);
                file.close();
                return;
            }
            data = file.readAll();
            file.close();
        }
        else
            data = text.toUtf8();
        // Select image via EncryptDialog
        EncryptDialog * dialog = new EncryptDialog(data);
        dialog->exec();
        if(!dialog->success)
            return;

        // Get the data
        QByteArray encr_data = dialog->compr_data;
        long long key = dialog->key;

        // Check the key
        if(key > pow(2, 24)) {
            alert("Key is too big. Limit is 2^24. Cannot continue!", true);
            return;
        }
        // Save the key
        QByteArray key_data = bytes((long int) (key / 65536)) + bytes((long int) (key / 256) % 256);
        key_data += bytes(key % 256);

        encr_data = key_data + encr_data;
        emit encrypt(encr_data, dialog->inputFileName);
    }
    else
    {
        // Get the filename of the image
        if(!ui->text->toPlainText().isEmpty())
            alert("Obviously, the text browser isn't supported for decryption, use File Dialog instead.");
        if(inputFileName.isEmpty()) {
            alert("File not selected. Cannot continue!", true);
            return;
        }
        emit decrypt(inputFileName);
    }
}
/*!
 * \brief ViewPC::alert Slot to create QMessageBox with message
 * \param message Message to be shown
 * \param isWarning Flag, if message is critical.
 */
void ViewPC::alert(QString message, bool isWarning)
{
    // Create message box
    QMessageBox box;
    if(isWarning)
        box.setIcon(QMessageBox::Warning);
    else
        box.setIcon(QMessageBox::Information);
    box.setText(message);
    box.setWindowIcon(QIcon(":/icons/mail.png"));
    box.setWindowTitle("Message");
    box.exec();
}
/*!
 * \brief ViewPC::saveData Slot to be called to save data using QFileDialog.
 * \param Edata Encrypted data to be saved.
 * \sa ModelPC::encrypt
 */
void ViewPC::saveData(QByteArray Edata)
{
    // Save data using QFileDialog
    QString outputFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/untitled.txt",
                               tr("Text(*.txt);;All files (*)"));
    QFile writeFile(outputFileName);
    if (!writeFile.open(QIODevice::WriteOnly))
    {
        alert("Cannot access file path. Cannot continue!", true);
        return;
    }
    writeFile.write(Edata);
    writeFile.close();
    alert("Decryption completed!");
}
/*!
 * \brief ViewPC::saveImage Slot to be called to save image using QFileDialog.
 * \param image Image to be saved.
 * \sa ModelPC::decrypt
 */
void ViewPC::saveImage(QImage * image)
{
    // Save image using QFileDialog
    QString outputFileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
                               "/untitled.png",
                               tr("Images(*.png)"));
    if(!image->save(outputFileName, nullptr, 100)) {
        alert("Cannot save file. Unable to continue!", true);
        return;
    }
    alert("Encryption completed!");
}
/*!
 * \brief ViewPC::setProgress Slot to set the value of the ProgressDialog (ViewPC::dialog).
 * \param val New value of the dialog.
 * If -1, creates ProgressDialog, if 101 closes the dialog.
 * \sa ViewPC::abortCircuit(), ModelPC::setProgress()
 */
void ViewPC::setProgress(int val)
{
    if(val < 0) {
        // Create dialog
        dialog = new QProgressDialog("Cryption in progress.", "Cancel", 0, 100);
        connect(dialog, SIGNAL(canceled()), this, SLOT(abortCircuit()));
        progressDialogClosed = false;
        dialog->setWindowTitle("Processing");
        dialog->setWindowIcon(QIcon(":/icons/loading.png"));
        dialog->show();
    }
    else if(val >= 100) {
        // Close dialog
        dialog->setValue(100);
        QThread::msleep(25);
        dialog->close();
        dialog->reset();
        progressDialogClosed = true;
    }
    // Update the progress
    else if(!progressDialogClosed)
        dialog->setValue(val);
}
/*!
 * \brief ViewPC::abortCircuit Slot to close ProgressDialog (ViewPC::dialog)
 */
void ViewPC::abortCircuit()
{
    // Set the flag
    progressDialogClosed = true;
    // Close the dialog
    dialog->close();
    dialog->reset();
    emit abortModel();
}
/*!
 * \brief ViewPC::setEncryptMode Set the encrpt mode (ViewPC::isEncrypt)
 * \param encr
 */
void ViewPC::setEncryptMode(bool encr)
{
    ui->text->setEnabled(encr);
    isEncrypt = encr;
}
/*!
 * \brief ViewPC::setVersion Set the version of the app from ControllerPC
 * \param version Version as QString
 */
void ViewPC::setVersion(QString version)
{
    // Version setup
    versionString = version;
}

QByteArray ViewPC::bytes(long long n)
{
    return QByteArray::fromHex(QByteArray::number(n, 16));
}
/*!
 * \brief ViewPC::on_actionAbout_triggered Opens about page
 */
void ViewPC::on_actionAbout_triggered()
{
    AboutPC about;
    about.setVersion(versionString);
    about.exec();
}

/*!
 * \brief ViewPC::on_actionHelp_triggered Opens online documentation
 */
void ViewPC::on_actionHelp_triggered()
{
    QUrl docLink("http://doc.alex.unaux.com/picturecrypt");
    QDesktopServices::openUrl(docLink);
}
