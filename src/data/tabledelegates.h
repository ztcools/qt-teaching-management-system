#ifndef TABLEDELEGATES_H
#define TABLEDELEGATES_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QBuffer>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QFile>

// ==============================================
// 1. 下拉框委托（性别、进度等）
// ==============================================
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ComboBoxDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    // 设置下拉选项
    void setItems(const QStringList& items) {
        m_items = items;
    }

    // 双击 → 创建编辑器
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        QComboBox* editor = new QComboBox(parent);
        editor->addItems(m_items);
        return editor;
    }

    // 从单元格 → 赋值给编辑器
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        if (!combo) return;

        QString text = index.data(Qt::EditRole).toString();
        combo->setCurrentText(text);
    }

    // 编辑器 → 保存回单元格
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        if (!combo) return;

        model->setData(index, combo->currentText(), Qt::EditRole);
    }

private:
    QStringList m_items;
};

// ==============================================
// 2. 日期选择委托
// ==============================================
class DateEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateEditDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        QDateEdit* editor = new QDateEdit(parent);
        editor->setDisplayFormat("yyyy-MM-dd");
        editor->setCalendarPopup(true);
        return editor;
    }

    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override
    {
        QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor);
        if (!dateEdit) return;

        QDate date = index.data(Qt::EditRole).toDate();
        dateEdit->setDate(date);
    }

    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override
    {
        QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor);
        if (!dateEdit) return;

        model->setData(index, dateEdit->date().toString("yyyy-MM-dd"), Qt::EditRole);
    }
};

// ==============================================
// 3. 图片显示 + 双击选择图片委托
// ==============================================
class ImageDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ImageDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
        m_photoSize = 60; // 单元格图片显示大小
    }

    // 绘制图片（显示用）
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        QByteArray imgData = index.data(Qt::UserRole).toByteArray();
        if (imgData.isEmpty()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QPixmap pix;
        if (pix.loadFromData(imgData) && !pix.isNull()) {
            painter->save();
            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            QPixmap scaled = pix.scaled(m_photoSize, m_photoSize,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

            // 图片居中显示
            QRect pixRect = option.rect;
            int x = pixRect.center().x() - scaled.width() / 2;
            int y = pixRect.center().y() - scaled.height() / 2;

            painter->drawPixmap(x, y, scaled);
            painter->restore();
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }

    // 双击事件 → 选择图片
    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override
    {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QString path = QFileDialog::getOpenFileName(
                    nullptr, "选择照片", "",
                    "图片(*.png *.jpg *.jpeg *.bmp)");

                if (!path.isEmpty()) {
                    QFile file(path);
                    if (file.open(QIODevice::ReadOnly)) {
                        QByteArray data = file.readAll();
                        model->setData(index, data, Qt::UserRole);
                        file.close();
                    }
                }
                return true;
            }
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    // 大小提示
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override
    {
        return QSize(m_photoSize + 10, m_photoSize + 10);
    }

private:
    int m_photoSize;
};

#endif // TABLEDELEGATES_H
