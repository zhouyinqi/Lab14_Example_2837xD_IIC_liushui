#pragma once

#include <QAbstractItemView>
#include <QComboBox>
#include <QScreen>
#include <QScrollBar>

class FullTextComboBox final : public QComboBox
{
public:
    explicit FullTextComboBox(QWidget *parent = nullptr) : QComboBox(parent)
    {
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        setMinimumWidth(180);
        view()->setTextElideMode(Qt::ElideNone);
        view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        connect(this, &QComboBox::currentTextChanged, this,
                [this](const QString &text) { setToolTip(text); });
    }

    void showPopup() override
    {
        int textWidth = 0;
        for (int index = 0; index < count(); ++index) {
            textWidth = qMax(textWidth, fontMetrics().horizontalAdvance(itemText(index)));
            setItemData(index, itemText(index), Qt::ToolTipRole);
        }
        const int desiredWidth = qMax(width(), textWidth + 48);
        const int screenWidth = screen() ? screen()->availableGeometry().width() : desiredWidth;
        // Expand the list, not the entire window; very long items can scroll.
        view()->setMinimumWidth(qMin(desiredWidth, qMax(1, screenWidth - 32)));
        QComboBox::showPopup();
        view()->horizontalScrollBar()->setValue(0);
    }
};
