#pragma once
#include <qlineedit.h>
#include <qcombobox.h>

class LineEdit : public QLineEdit
{
	Q_OBJECT;
public:
	LineEdit(QWidget* parent = Q_NULLPTR);
};

class ComboBox : public QComboBox
{
	Q_OBJECT;
public:
	ComboBox(QWidget* parent = Q_NULLPTR);
};

class WComboBox : public QComboBox
{
	Q_OBJECT;
public:
	WComboBox(QWidget* parent = Q_NULLPTR);
};
