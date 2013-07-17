#include "logwidget.h"
#include <QStatusBar>
#include "logs/logtablemodel.h"
#include "utils.h"

namespace logs {

void LogWidget::findTextInAllColumns (QString const & text, int from_row, int to_row, bool only_first)
{
	LogTableModel * model = static_cast<LogTableModel *>(m_table_view_proxy ? m_table_view_proxy->sourceModel() : m_table_view_widget->model());
	for (int i = from_row, ie = to_row; i < ie; ++i)
	{
		for (int j = 0, je = model->columnCount(); j < je; ++j)
		{
			if (isModelProxy()) // @TODO: dedup!
			{
				QModelIndex const idx = model->index(i, j, QModelIndex());
				QModelIndex const curr = m_table_view_proxy->mapFromSource(idx);

				if (idx.isValid() && model->data(idx).toString().contains(text, Qt::CaseInsensitive))
				{
					m_table_view_widget->selectionModel()->setCurrentIndex(curr, QItemSelectionModel::Select);
					m_last_search_row = idx.row();
					m_last_search_col = idx.column();
					if (only_first)
						return;
				}
			}
			else
			{
				QModelIndex const idx = model->index(i, j, QModelIndex());
				if (idx.isValid() && model->data(idx).toString().contains(text, Qt::CaseInsensitive))
				{
					m_table_view_widget->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::Select);
					m_last_search_row = idx.row();
					m_last_search_col = idx.column();
					if (only_first)
						return;
				}
			}
		}
	}
}

bool LogWidget::matchTextInCell (QString const & text, int row, int col)
{
	LogTableModel * model = static_cast<LogTableModel *>(m_table_view_proxy ? m_table_view_proxy->sourceModel() : m_table_view_widget->model());
	QModelIndex const idx = model->index(row, col, QModelIndex());
	if (idx.isValid() && model->data(idx).toString().contains(text, Qt::CaseInsensitive))
	{
		qDebug("found string %s: src=%i,%i", text.toStdString(), row, col);
		if (m_table_view_proxy)
		{
			QModelIndex const curr = m_table_view_proxy->mapFromSource(idx);
			m_table_view_widget->selectionModel()->setCurrentIndex(curr, QItemSelectionModel::Select);
			m_table_view_widget->scrollTo(m_table_view_proxy->mapFromSource(idx), QTableView::PositionAtCenter);
		}
		else
		{
			m_table_view_widget->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::Select);
			m_table_view_widget->scrollTo(idx, QTableView::PositionAtCenter);
		}
		m_last_search_row = idx.row();
		return true;
	}
	return false;
}

void LogWidget::endOfSearch ()
{
	qDebug("end of search");
	// flash icon
	//m_connection->getMainWindow()->statusBar()->showMessage(tr("End of document!"));
	m_last_search_row = 0;
}

void LogWidget::findTextInColumn (QString const & text, int col, int from_row, int to_row)
{
	for (int i = from_row, ie = to_row; i < ie; ++i)
		if (matchTextInCell(text, i, col))
			return;
	endOfSearch();
}
void LogWidget::findTextInColumnRev (QString const & text, int col, int from_row, int to_row)
{
	bool found = false;
	for (int i = from_row, ie = to_row; i --> ie; )
		if (matchTextInCell(text, i, col))
			return;

	endOfSearch();
}


void LogWidget::selectionFromTo (int & from, int & to) const
{
	from = 0;
	LogTableModel const * model = static_cast<LogTableModel *>(m_table_view_proxy ? m_table_view_proxy->sourceModel() : m_table_view_widget->model());
	to = model->rowCount();
	QItemSelectionModel const * selection = m_table_view_widget->selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();
	if (indexes.size() < 1)
		return;

	std::sort(indexes.begin(), indexes.end());
	from = indexes.first().row();
}

void LogWidget::findAllTexts (QString const & text)
{
	m_last_search = text;
	int from = 0;
	LogTableModel const * model = static_cast<LogTableModel *>(m_table_view_proxy ? m_table_view_proxy->sourceModel() : m_table_view_widget->model());
	int to = model->rowCount();
	findTextInAllColumns(text, from, to, false);
}

void LogWidget::findText (QString const & text, tlv::tag_t tag)
{
	if (m_last_search != text)
	{
		m_last_search_row = 0;	// this is a new search
		m_last_search = text;
		int const col_idx = findColumn4Tag(tag);
		m_last_search_col = col_idx;

		if (m_last_search.isEmpty())
		{
			m_last_search_row = m_last_search_col = 0;
			return;
		}


		//@TODO: clear selection?
		int from, to;
		selectionFromTo(from, to);
		findTextInColumn(m_last_search, col_idx, from, to);
	}
	else
	{
		LogTableModel const * model = static_cast<LogTableModel *>(m_table_view_proxy ? m_table_view_proxy->sourceModel() : m_table_view_widget->model());
		int const to = model->rowCount();
		findTextInColumn(m_last_search, m_last_search_col, m_last_search_row + 1, to);
	}
}

void LogWidget::findText (QString const & text)
{
	m_last_search = text;
	m_last_search_row = 0;
	m_last_search_col = -1;

	if (m_last_search.isEmpty())
	{
		m_last_search_row = m_last_search_col = 0;
		return;
	}

	int from, to;
	selectionFromTo(from, to);
	findTextInAllColumns(m_last_search, from, to, true);
}

void LogWidget::findNext (QString const & text)
{
	int from, to;
	selectionFromTo(from, to);
	if (text != m_last_search)
	{
		m_last_search = text;
	}

	if (!m_last_clicked.isValid())
	{
		int const col_idx = findColumn4Tag(tlv::tag_msg);
		m_last_search_col = col_idx < 0 ? 0 : col_idx;
	}

	if (m_last_search.isEmpty())
	{
		m_last_search_row = 0;
		return;
	}
	findTextInColumn(m_last_search, m_last_search_col, m_last_search_row + 1, to);
}

void LogWidget::findPrev (QString const & text)
{
	int from, to;
	selectionFromTo(from, to);
	if (!m_last_clicked.isValid())
	{
		int const col_idx = findColumn4Tag(tlv::tag_msg);
		m_last_search_col = col_idx < 0 ? 0 : col_idx;
	}

	if (text != m_last_search)
	{
		m_last_search = text;
	}

	if (m_last_search.isEmpty())
	{
		m_last_search_row = to;
		return;
	}
	int const last = m_last_search_row > 0 ? m_last_search_row - 1 : to;
	findTextInColumnRev(m_last_search, m_last_search_col, last, 0);
}

QString LogWidget::findString4Tag (tlv::tag_t tag, QModelIndex const & row_index) const
{
	return findVariant4Tag(tag, row_index).toString();
}

QVariant LogWidget::findVariant4Tag (tlv::tag_t tag, QModelIndex const & row_index) const
{
	int const idx = m_tags2columns[tag];
	if (idx == -1)
		return QVariant();

	LogTableModel * model = static_cast<LogTableModel *>(m_table_view_proxy ? m_table_view_proxy->sourceModel() : m_table_view_widget->model());

	QModelIndex const model_idx = model->index(row_index.row(), idx, QModelIndex());
	if (model_idx.isValid())
	{
		QVariant value = model->data(model_idx);
		return value;
	}
	return QVariant();
}


void LogWidget::scrollToCurrentTag ()
{
	if (m_config.m_auto_scroll)
		return;

	if (m_color_tag_rows.size() == 0)
		return;

	if (m_current_tag == -1)
		m_current_tag = 0;

	if (m_current_tag >= m_color_tag_rows.size())
		m_current_tag = 0;

	if (m_current_tag < m_color_tag_rows.size())
	{
		int const tag_row = m_color_tag_rows[m_current_tag];
		QModelIndex const tag_idx = model()->index(tag_row, 0);

		//qDebug("scrollToCurrentTag: current=%2i src row=%2i ", sessionState().m_current_tag, tag_row);

		if (isModelProxy())
			scrollTo(m_table_view_proxy->mapFromSource(tag_idx), QAbstractItemView::PositionAtCenter);
		else
			scrollTo(tag_idx, QAbstractItemView::PositionAtCenter);
	}
}

void LogWidget::scrollToCurrentSelection ()
{
	if (m_config.m_auto_scroll)
		return;

	QItemSelectionModel const * selection = selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if (indexes.size() == 0)
		return;

	if (m_current_selection == -1)
		m_current_selection = 0;

	if (m_current_selection >= indexes.size())
		m_current_selection = 0;

	QModelIndex const idx = indexes.at(m_current_selection);
	qDebug("scrollToSelection[%i] row=%i", m_current_selection, idx.row());
	if (isModelProxy())
	{
		QModelIndex const own_idx = m_table_view_proxy->index(idx.row(), idx.column());
		scrollTo(own_idx, QAbstractItemView::PositionAtCenter);
	}
	else
	{
		QModelIndex const own_idx = model()->index(idx.row(), idx.column());
		scrollTo(own_idx, QAbstractItemView::PositionAtCenter);
	}
}

void LogWidget::scrollToCurrentTagOrSelection ()
{
	if (m_color_tag_rows.size() > 0)
		scrollToCurrentTag();
	else
		scrollToCurrentSelection();
}

void LogWidget::nextToView ()
{
	if (m_color_tag_rows.size() > 0)
	{
		++m_current_tag;
		scrollToCurrentTag();
	}
	else
	{
		++m_current_selection;
		scrollToCurrentSelection();
	}
}

void LogWidget::onFindFileLine (QModelIndex const &)
{
	//@FIXME: unused args
	qDebug("find file:line for idx=(%i,col)", m_last_clicked.row());

	bool const scroll_to_item = true;
	bool const expand = true;
	findTableIndexInFilters(m_last_clicked, scroll_to_item, expand);
}

void LogWidget::addColorTagRow (int row)
{
	for (int i = 0, ie = m_color_tag_rows.size(); i < ie; ++i)
		if (m_color_tag_rows.at(i) == row)
		{
			removeColorTagRow(row);
			return;
		}
	m_color_tag_rows.push_back(row);
}

bool LogWidget::findColorTagRow (int row) const
{
	for (int i = 0, ie = m_color_tag_rows.size(); i < ie; ++i)
		if (m_color_tag_rows.at(i) == row)
			return true;
	return false;
}

void LogWidget::removeColorTagRow (int row)
{
	m_color_tag_rows.erase(std::remove(m_color_tag_rows.begin(), m_color_tag_rows.end(), row), m_color_tag_rows.end());
}




}