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

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>


class QToolButton;

/**
 * @brief QLineEdit with a RTL clear button (system theme)
 *
 * ESC also clears the contents
 */
class LineEdit : public QLineEdit
{
	Q_OBJECT

public:
	LineEdit(QWidget* parent = 0);

protected:
	void resizeEvent(QResizeEvent*);

private slots:
	void updateCloseButton(const QString& text);

private:
	QToolButton* clearButton;

	// QWidget interface
protected:
	virtual void keyReleaseEvent(QKeyEvent*);
};

#endif // LIENEDIT_H

