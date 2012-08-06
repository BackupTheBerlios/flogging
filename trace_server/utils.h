/**
 * Copyright (C) 2011 Mojmir Svoboda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
#pragma once
#include <QStandardItem>
#include <QString>
#include <QDir>
#include <QFile>
#include <QSettings>
#include "sessionstate.h"
#include "constants.h"

inline void write_list_of_strings (QSettings & settings, char const * groupname, char const * groupvaluename, QList<QString> const & lst)
{
	settings.beginWriteArray(groupname);
	for (int i = 0, ie = lst.size(); i < ie; ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue(groupvaluename, lst.at(i));
		qDebug("store to registry %i/%i: %s", i,ie, lst.at(i).toStdString().c_str());
	}
	settings.endArray();
}

inline void read_list_of_strings (QSettings & settings, char const * groupname, char const * groupvaluename, QList<QString> & lst)
{
	int const size = settings.beginReadArray(groupname);
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		QString val = settings.value(groupvaluename).toString();
		qDebug("read from registry: %s", val.toStdString().c_str());
		lst.push_back(val);
	}
	settings.endArray();
}


inline QString getPresetFileName (QString const & appdir, QString const & preset_name)
{
	QString presetdir = appdir + "/" + preset_name;
	QDir d;
	d.mkpath(presetdir);
	QString const fname = presetdir + "/" + g_presetFileName;
	return fname;
}

inline QString getDataTagFileName (QString const & appdir, QString const & app_name, QString const & tag)
{
	QString presetdir = appdir + "/" + app_name;
	QDir d;
	d.mkpath(presetdir);
	QString const fname = presetdir + "/" + tag;
	return fname;
}

inline bool existsFile (char const * fname)
{
	QFile file(fname);
	return file.exists();
}

inline QString getPresetPath (QString const & app_name, QString const & name)
{
	QString path;
	path.append(app_name);
	path.append("/");
	path.append(name);
	return path;
}

inline QStandardItem * findChildByText (QStandardItem * parent, QString const & txt)
{
	for (size_t i = 0, ie = parent->rowCount(); i < ie; ++i)
	{
		if (parent->child(i)->text() == txt)
			return parent->child(i);
	}
	return 0;
}

inline QList<QStandardItem *> addRow (QString const & str, bool checked )
{
	QList<QStandardItem *> row_items;
	QStandardItem * const name_item = new QStandardItem(str);
	name_item->setCheckable(true);
	name_item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	row_items << name_item;
	return row_items;
}

inline QList<QStandardItem *> addUncheckableRow (QString const & str)
{
	QList<QStandardItem *> row_items;
	QStandardItem * const name_item = new QStandardItem(str);
	name_item->setCheckable(false);
	row_items << name_item;
	return row_items;
}

inline QList<QStandardItem *> addTriRow (QString const & str, Qt::CheckState const checked, char const mode)
{
	QList<QStandardItem *> row_items;

	QStandardItem * const name_item = new QStandardItem(str);
	name_item->setCheckable(true);
	name_item->setEditable(false);
	row_items << name_item;
	QStandardItem * const mode_item = new QStandardItem(QString(mode));
	row_items.append(mode_item);
	name_item->setCheckable(false);
	return row_items;
}

inline QList<QStandardItem *> addTriRow (QString const & str, Qt::CheckState const checked, bool const inclusive)
{
	QList<QStandardItem *> row_items;

	QStandardItem * const name_item = new QStandardItem(str);
	name_item->setCheckable(true);
	name_item->setEditable(false);
	row_items << name_item;

	QString mode("E");
	if (inclusive)
		mode = "I";

	QStandardItem * const mode_item = new QStandardItem(mode);
	row_items.append(mode_item);
	name_item->setCheckable(false);
	return row_items;
}

	 
inline QList<QStandardItem *> addRowTriState (QString const & str, E_NodeStates const state)
{
	QList<QStandardItem *> row_items;
	QStandardItem * const name_item = new QStandardItem(str);
	name_item->setCheckable(true);
	name_item->setTristate(true);	
	name_item->setCheckState(static_cast<Qt::CheckState>(state));
	row_items << name_item;
	return row_items;
}

/*inline void flipCheckState (QStandardItem * item)
{
	bool const checked = (item->checkState() == Qt::Checked);
	item->setCheckState(checked ? Qt::Unchecked : Qt::Checked);
}

inline void flipCheckStateChilds (QStandardItem * node)
{
	flipCheckState(node);
	int const rc = node->rowCount();
	for (int r = 0; r < rc; ++r)
	{
		QStandardItem * const child = node->child(r, 0);
		flipCheckStateChilds(child);
	}
}*/

inline void setCheckState (QStandardItem * const node, Qt::CheckState const state)
{
	if (node)
		node->setCheckState(state);
}

inline bool checkChildState (QStandardItem * const node, Qt::CheckState const state)
{
	bool result = true;
	if (node->rowCount() == 0)
		result &= (node->checkState() == state);
	else
		for (int r = 0, re = node->rowCount(); r < re; ++r)
		{
			QStandardItem * const child = node->child(r, 0);
			result &= checkChildState(child, state);
		}
	return result;
}

inline void setCheckStateChilds (QStandardItem * const node, Qt::CheckState const state)
{
	setCheckState(node, state);
	int const rc = node->rowCount();
	//qDebug("node=%s, state=%u", node->text().toStdString().c_str(), state);
	for (int r = 0; r < rc; ++r)
	{
		QStandardItem * const child = node->child(r, 0);
		setCheckStateChilds(child, state);
	}
}

inline void setCheckStateReverse (QStandardItem * n, Qt::CheckState const state)
{
	while (n)
	{
		n->setCheckState(state);
		n = n->parent();
	}
}

inline void collectPath (QStandardItem const * n, std::vector<QString> & tokens)
{
	while (n)
	{
		tokens.push_back(n->text());
		n = n->parent();
	}
}

inline void reassemblePath (std::vector<QString> const & tokens, QString & path)
{
	for (std::vector<QString>::const_reverse_iterator it = tokens.rbegin(), ite = tokens.rend(); it != ite; ++it)
	{
		path += *it;
		path += '/';
	}
}

template <class F>
void set_state_to_childs (file_filter<F> const & ff, std::string const & file, E_NodeStates state);
template <class F>
void set_state_to_parents (file_filter<F> const & ff, typename file_filter<F>::node_t * node, E_NodeStates state);
template <class F>
void set_state_to_childs (file_filter<F> const & ff, typename file_filter<F>::node_t * node, E_NodeStates state);

template <class F>
inline void set_state_to_topdown (file_filter<F> const & ff, std::string const & file, E_NodeStates fw_state, E_NodeStates rev_state)
{
	typedef typename file_filter<F>::tokenizer_t tokenizer_t;
	tokenizer_t tok(file, ff.separator);
	typename file_filter<F>::node_t * level = ff.root;
	typename tokenizer_t::const_iterator it = tok.begin(), ite = tok.end();
	while (it != ite)
	{
		level = file_filter<F>::node_t::node_child(level, *it);
		if (level == 0)
			return;

		++it;
		if (it == ite)
		{
			level->data.m_state = fw_state;
			set_state_to_childs(ff, level, fw_state);
			set_state_to_parents(ff, level, rev_state);
		}
	}
}

template <class F>
inline void set_state_to_childs (file_filter<F> const & ff, std::string const & file, E_NodeStates state)
{
	typename file_filter<F>::tokenizer_t tok(file, ff.separator);
	typename file_filter<F>::node_t * level = ff.root;
	typename file_filter<F>::tokenizer_t::const_iterator it = tok.begin(), ite = tok.end();
	while (it != ite)
	{
		level = file_filter<F>::node_t::node_child(level, *it);
		if (level == 0)
			return;

		++it;
		if (it == ite)
		{
			level->data.m_state = state;
			set_state_to_childs(ff, level, state);
		}
	}
}

template <class F>
inline void set_state_to_childs (file_filter<F> const & ff, typename file_filter<F>::node_t * node, E_NodeStates state)
{
	node = node->children;
	while (node)
	{
		node->data.m_state = state;
		set_state_to_childs(ff, node, state);
		node = node->next;
	}
}

template <class F>
inline void set_state_to_parents (file_filter<F> const & ff, typename file_filter<F>::node_t * node, E_NodeStates state)
{
	while (node = node->parent)
	{
		node->data.m_state = state;
		set_state_to_parents(ff, node, state);
	}
}



/*inline void syncCheckBoxesWithFileFilters (QStandardItem const * node, E_FilterMode const fmode, file_filter & ff, strings_t & stack)
{
	stack.push_back(node->text().toStdString());
	ff.exclude_to_state(stack, static_cast<E_NodeStates>(node->checkState()));
	bool const checked = (node->checkState() == Qt::Checked);
	//if (fmode == e_Include)
	//	ff.exclude_to_state(stack, !checked);
	//else
	//	ff.exclude_to_state(stack, checked);

	bool const descend = ((fmode == e_Include && checked) || (fmode == e_Exclude && !checked));
	if (descend)
	{
		int const rc = node->rowCount();
		for (int r = 0; r < rc; ++r)
		{
			QStandardItem const * const child = node->child(r, 0);
			syncCheckBoxesWithFileFilters(child, fmode, ff, stack);
		}
	}
	stack.pop_back();
}

inline void syncCheckBoxesWithFileFilters (QStandardItem const * node, E_FilterMode const fmode, file_filter & ff)
{
	strings_t stack;
	stack.reserve(256);
	syncCheckBoxesWithFileFilters(node, fmode, ff, stack);
}
*/

