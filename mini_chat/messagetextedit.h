#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H
#include "QTextEdit"
#include "global.h"
class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget* parent = nullptr);
    ~MessageTextEdit();
    QVector<MsgInfo> getMsgList();
    void insertFileFromUrl(const QStringList& urls);

signals:
    void send();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void focusOutEvent(QFocusEvent* event) override;
private:
    void insertImages(const QString& url);
    void insertTextFile(const QString& url);
    bool canInsertFromMimeData(const QMimeData* source)const override;
    void insertFromMimeData(const QMimeData* source)override;

private:
    bool isImage(QString url);
    void insertMsgList(QVector<MsgInfo>& list, QString flag, QString text, QPixmap pix);

    QStringList getUrl(QString text);
    QPixmap getFileIconPixmap(const QString& url);//获取文件图标和大小信息，并转换为图片
    QString getFilesSize(qint64 size);

private slots:
    void textEditChanged();

private:
    QVector<MsgInfo> mMsgList;
    QVector<MsgInfo> mGetMsgList;


};

#endif // MESSAGETEXTEDIT_H
