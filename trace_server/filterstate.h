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
 * OUT OF OR IN SessionState WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QColor>
#include <tlv_parser/tlv_parser.h>
#include <tlv_parser/tlv_decoder.h> // htons!
#include <filters/file_filter.hpp>
#include <boost/serialization/nvp.hpp>
#include "tls.h"
#include "config.h"

class PersistentFilter;
class MainWindow;

struct SessionStats {
	size_t m_Read_B;
	size_t m_Write_B;
	//hptimer_t m_StartT;
};

struct Dict {
	QList<QString> m_names;
	QList<QString> m_strvalues;
	QList<int> m_values;

	QString findNameFor (QString const & strval) const
	{
		for (int i = 0, ie = m_strvalues.size(); i < ie; ++i)
			if (m_strvalues.at(i) == strval)
				return m_names.at(i);
		return QString();
	}

	template <class ArchiveT>
	void serialize (ArchiveT & ar, unsigned const version)
	{
		ar & boost::serialization::make_nvp("names", m_names);
		ar & boost::serialization::make_nvp("strvalues", m_strvalues);
		ar & boost::serialization::make_nvp("values", m_values);
	}
};

class FilterState
{
public:
	explicit FilterState (QObject *parent = 0);
	~FilterState ();

	// filter state

	// blocks
	void appendCollapsedBlock (QString tid, int from, int to, QString file, QString line);
	bool findCollapsedBlock (QString tid, int from, int to) const;
	bool eraseCollapsedBlock (QString tid, int from, int to);
	bool isBlockCollapsed (QString tid, int row) const;
	bool isBlockCollapsedIncl (QString tid, int row) const;

	// text colorization
	void appendToColorRegexFilters (QString const & str);
	void removeFromColorRegexFilters (QString const & str);
	bool isMatchedColorizedText (QString str, QColor & color, E_ColorRole & role) const;
	void setRegexColor (QString const & s, QColor col);
	void setColorRegexChecked (QString const & s, bool checked);

	void clearFilters ();
	void onClearColorizedRegexFilter () { m_colorized_texts.clear(); }
	void onClearScopeFilter () { m_collapse_blocks.clear(); }

	template <class ArchiveT>
	void serialize (ArchiveT & ar, unsigned const version)
	{
	}


signals:
	
private:
public:

	friend class Connection;
	friend class FilterProxyModel;
	//friend class FilterWidget;
	friend class PersistentFilter;

	QList<ColorizedText>	m_colorized_texts;
	QList<CollapsedBlock>	m_collapse_blocks;
};

bool saveFilterState (FilterState const & s, char const * filename);
bool loadFilterState (FilterState & s, char const * filename);
bool loadFilterState (FilterState const & src, FilterState & target);

