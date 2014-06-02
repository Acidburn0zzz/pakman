/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/
// Based on:
// https://blog.qt.digia.com/blog/2007/06/06/lineedit-with-a-clear-button/
// -> http://git.forwardbias.in/?p=lineeditclearbutton.git;a=tree;hb=HEAD
//
// Slightly modified by Thomas Binkau

#include "lineedit.h"

#include <QToolButton>
#include <QStyle>
#include <QKeyEvent>
#include "src/icons.h"


LineEdit::LineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	clearButton = new QToolButton(this);
	clearButton->setIcon(iconClearRTL());
	clearButton->setCursor(Qt::ArrowCursor);
	clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	clearButton->hide();

	connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));

	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
	QSize msz = minimumSizeHint();
	setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
	               qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void LineEdit::resizeEvent(QResizeEvent *)
{
	QSize sz = clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	clearButton->move(rect().right() - frameWidth - sz.width(),
	                  (rect().bottom() + 1 - sz.height())/2);
}

void LineEdit::updateCloseButton(const QString& text)
{
	clearButton->setVisible(!text.isEmpty());
}

void LineEdit::keyReleaseEvent(QKeyEvent* key)
{
	QLineEdit::keyReleaseEvent(key);
	if (key->key() == Qt::Key_Escape)
		clear();
}
