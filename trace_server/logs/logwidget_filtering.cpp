#include "logwidget.h"
#include <QListView>
#include <QFile>
#include <QRegExp>
#include <tlv_parser/tlv_encoder.h>
#include "utils.h"
#include "utils_qstandarditem.h"
#include "filterproxymodel.h"
#include "logs/logtablemodel.h"
#include "qtsln/qtcolorpicker/qtcolorpicker.h"

namespace logs {

void LogWidget::onInvalidateFilter ()
{
	QItemSelectionModel const * selection = selectionModel();
	if (!selection)
		return;

	QModelIndexList const old_selection = selection->selectedIndexes();

	//  fantomas ended here
	QModelIndexList srcs;
	if (isModelProxy())
	{
		for (int i = 0, ie = old_selection.size(); i < ie; ++i)
		{
			QModelIndex const & pxy_idx = old_selection.at(i);
			QModelIndex const src_idx = m_proxy_model->mapToSource(pxy_idx);

			//qDebug("update filter: pxy=(%2i, %2i) src=(%2i, %2i)", pxy_idx.row(), pxy_idx.column(), src_idx.row(), src_idx.column());
			srcs.push_back(src_idx);
		}
	}
	else
		srcs = old_selection;

	if (isModelProxy())
		static_cast<FilterProxyModel *>(m_proxy_model)->force_update();
	else
	{
		m_src_model->emitLayoutChanged();
	}

	syncSelection(srcs);

	scrollToCurrentTagOrSelection();
}

void LogWidget::syncSelection (QModelIndexList const & sel)
{
	selectionModel()->clearSelection();

	for (int i = 0, ie = sel.size(); i < ie; ++i)
	{
		QModelIndex idx = sel.at(i);
		if (isModelProxy())
		{
			idx = m_proxy_model->mapFromSource(sel.at(i));
			//qDebug("syncSelection: pxy src=(%2i, %2i)", idx.row(), idx.column());
		}
		//else qDebug("syncSelection: src=(%2i, %2i)", idx.row(), idx.column());

		if (idx.isValid())
			selectionModel()->setCurrentIndex(idx, QItemSelectionModel::Select);
	}
}

void LogWidget::setupFilteringProxy (int state)
{
	QItemSelectionModel const * selection = selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if (state == Qt::Unchecked)
	{
		if (indexes.size() < 1)
			return;
		QModelIndexList srcs;
		for (int i = 0, ie = indexes.size(); i < ie; ++i)
		{
			QModelIndex const & pxy_idx = indexes.at(i);
			QModelIndex const src_idx = m_proxy_model->mapToSource(pxy_idx);
			srcs.push_back(src_idx);
		}

		setModel(m_src_model);
		setSelectionModel(m_src_selection);

		for (int i = 0, ie = srcs.size(); i < ie; ++i)
			selectionModel()->setCurrentIndex(srcs.at(i), QItemSelectionModel::Select);
	}
	else if (state == Qt::Checked)
	{
		setModel(m_proxy_model);
		setSelectionModel(m_proxy_selection);

		static_cast<FilterProxyModel *>(m_proxy_model)->force_update();
		onInvalidateFilter();

		QModelIndexList pxys;
		for (int i = 0, ie = indexes.size(); i < ie; ++i)
		{
			QModelIndex const & src_idx = indexes.at(i);
			QModelIndex const pxy_idx = m_proxy_model->mapFromSource(src_idx);
			//qDebug("on: src(r=%i c=%i) -> pxy(r=%i c=%i)", src_idx.row(), src_idx.column(), pxy_idx.row(), pxy_idx.column());
			if (pxy_idx.isValid())
				pxys.push_back(pxy_idx);
		}

		for (int i = 0, ie = pxys.size(); i < ie; ++i)
		{
			//qDebug("on: pxy r=%i c=%i", pxys.at(i).row(), pxys.at(i).column());
			selectionModel()->setCurrentIndex(pxys.at(i), QItemSelectionModel::Select);
		}
	}

	//if (m_column_setup_done)
	//	setupColumnSizes(true);

	if (m_config.m_in_view)
		scrollToCurrentTagOrSelection();
		//QTimer::singleShot(1000, this, SLOT(scrollToCurrentTagOrSelection()));
}

void LogWidget::clearFilters (QStandardItem * node)
{
	if (node)
	{
		if (node->checkState() == Qt::Checked)
		{
			node->setCheckState(Qt::Unchecked);
		}
		for (int i = 0, ie = node->rowCount(); i < ie; ++i)
			clearFilters(node->child(i));
	}
}

void LogWidget::clearFilters ()
{
	//@TODO: call all functions below
	//m_filter_state.clearFilters();
}

void LogWidget::onClearCurrentFileFilter ()
{
	m_filter_state.onClearFileFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentCtxFilter ()
{
	m_filter_state.onClearCtxFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentTIDFilter ()
{
	m_filter_state.onClearTIDFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentColorizedRegexFilter ()
{
	m_filter_state.onClearColorizedRegexFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentRegexFilter ()
{
	m_filter_state.onClearRegexFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentStringFilter ()
{
	m_filter_state.onClearStringFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentScopeFilter ()
{
	m_filter_state.onClearScopeFilter();
	onInvalidateFilter();
}
void LogWidget::onClearCurrentRefTime ()
{
	onClearRefTime();
	onInvalidateFilter();
}

void LogWidget::onExcludeFileLine (QModelIndex const & row_index)
{
	QString file = findString4Tag(tlv::tag_file, row_index);
	QString line = findString4Tag(tlv::tag_line, row_index);
	qDebug("appending: %s:%s", file.toStdString().c_str(), line.toStdString().c_str());
	QString const fileline = file + "/" + line;
	QModelIndex const result = filterWidget()->fileModel()->stateToItem(fileline, Qt::Unchecked);
	if (!result.isValid())
	{
		Q_ASSERT("nonexistent index");
		qFatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
	onInvalidateFilter();
}

void LogWidget::onFileColOrExp (QModelIndex const & idx, bool collapsed)
{
	QStandardItemModel const * const model = static_cast<QStandardItemModel *>(filterWidget()->getWidgetFile()->model());
	QStandardItem * const node = model->itemFromIndex(idx);

	std::vector<QString> s;	// @TODO: hey piggy, to member variables
	s.clear();
	s.reserve(16);
	QStandardItem * parent = node;
	QModelIndex parent_idx = model->indexFromItem(parent);
	while (parent_idx.isValid())
	{
		QString const & val = model->data(parent_idx, Qt::DisplayRole).toString();
		s.push_back(val);
		parent = parent->parent();
		parent_idx = model->indexFromItem(parent);
	}

	QString file;
	for (std::vector<QString>::const_reverse_iterator it=s.rbegin(), ite=s.rend(); it != ite; ++it)
		file += QString("/") + *it;

	m_filter_state.m_file_filters.set_to_state(file, TreeModelItem(static_cast<E_NodeStates>(node->checkState()), collapsed));
}

void LogWidget::onFileExpanded (QModelIndex const & idx)
{
	onFileColOrExp(idx, false);
}

void LogWidget::onFileCollapsed (QModelIndex const & idx)
{
	onFileColOrExp(idx, true);
}

void LogWidget::appendToTIDFilters (QString const & item)
{
	QStandardItem * root = filterWidget()->m_tid_model->invisibleRootItem();
	QStandardItem * child = findChildByText(root, item);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addRow(item, true);
		root->appendRow(row_items);
	}
}

void LogWidget::appendToLvlWidgets (FilteredLevel const & flt)
{
	QStandardItem * root = filterWidget()->m_lvl_model->invisibleRootItem();
	QStandardItem * child = findChildByText(root, flt.m_level_str);
	if (child == 0)
	{
		E_LevelMode const mode = static_cast<E_LevelMode>(flt.m_state);
		QList<QStandardItem *> row_items = addTriRow(flt.m_level_str, Qt::Checked, lvlModToString(mode));
		row_items[0]->setCheckState(flt.m_is_enabled ? Qt::Checked : Qt::Unchecked);
		root->appendRow(row_items);
		filterWidget()->getWidgetLvl()->sortByColumn(0, Qt::AscendingOrder);
	}
}

void LogWidget::appendToLvlFilters (QString const & item)
{
	bool enabled = false;
	E_LevelMode lvlmode = e_LvlInclude;
	if (m_filter_state.isLvlPresent(item, enabled, lvlmode))
		return;

	QStandardItem * root = filterWidget()->m_lvl_model->invisibleRootItem();
	QStandardItem * child = findChildByText(root, item);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addTriRow(item, Qt::Checked, true);
		row_items[0]->setCheckState(Qt::Checked);
		root->appendRow(row_items);
		filterWidget()->getWidgetLvl()->sortByColumn(0, Qt::AscendingOrder);
		m_filter_state.appendLvlFilter(item);
	}
}

void LogWidget::appendToCtxWidgets (FilteredContext const & flt)
{
	QStandardItem * root = filterWidget()->m_ctx_model->invisibleRootItem();
	QStandardItem * child = findChildByText(root, flt.m_ctx_str);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addRow(flt.m_ctx_str, true);
		row_items[0]->setCheckState(flt.m_is_enabled ? Qt::Checked : Qt::Unchecked);
		root->appendRow(row_items);
	}
}


void LogWidget::appendToCtxFilters (QString const & item, bool checked)
{
	bool enabled = false;
	if (m_filter_state.isCtxPresent(item, enabled))
		return;

	QStandardItem * root = filterWidget()->m_ctx_model->invisibleRootItem();
	QStandardItem * child = findChildByText(root, item);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addRow(item, true);
		row_items[0]->setCheckState(Qt::Checked);
		root->appendRow(row_items);
		m_filter_state.appendCtxFilter(item);
	}
}

bool LogWidget::appendToFilters (DecodedCommand const & cmd)
{
	QString line;
	for (size_t i=0, ie=cmd.tvs.size(); i < ie; ++i)
	{
		if (cmd.tvs[i].m_tag == tlv::tag_line)
		{
			line = cmd.tvs[i].m_val;
			break;
		}

		if (cmd.tvs[i].m_tag == tlv::tag_tid)
		{
			int const idx = m_tls.findThreadId(cmd.tvs[i].m_val);
			if (cmd.hdr.cmd == tlv::cmd_scope_entry)
				m_tls.incrIndent(idx);
			if (cmd.hdr.cmd == tlv::cmd_scope_exit)
				m_tls.decrIndent(idx);
			appendToTIDFilters(cmd.tvs[i].m_val);
		}

		if (cmd.tvs[i].m_tag == tlv::tag_ctx)
		{
			appendToCtxFilters(cmd.tvs[i].m_val, false);
		}
		if (cmd.tvs[i].m_tag == tlv::tag_lvl)
		{
			appendToLvlFilters(cmd.tvs[i].m_val);
		}
	}

	/*for (size_t i=0, ie=cmd.tvs.size(); i < ie; ++i)
	{
		if (cmd.tvs[i].m_tag == tlv::tag_file)
		{
			QString file(cmd.tvs[i].m_val);
			QModelIndex const ret = m_file_model->insertItem(file + "/" + line);
			if (ret.isValid())
				m_main_window->getWidgetFile()->hideLinearParents();
		}
	}*/
	return true;
}

void LogWidget::appendToRegexFilters (QString const & str, bool checked, bool inclusive)
{
	m_filter_state.appendToRegexFilters(str, checked, inclusive);
}

void LogWidget::removeFromRegexFilters (QString const & val)
{
	m_filter_state.removeFromRegexFilters(val);
}

void LogWidget::recompileRegexps ()
{
	for (int i = 0, ie = m_filter_state.m_filtered_regexps.size(); i < ie; ++i)
	{
		FilteredRegex & fr = m_filter_state.m_filtered_regexps[i];
		QStandardItem * root = filterWidget()->m_regex_model->invisibleRootItem();
		QString const qregex = fr.m_regex_str;
		QStandardItem * child = findChildByText(root, qregex);
		fr.m_is_enabled = false;
		if (!child)
			continue;
		QRegExp regex(qregex);
		if (regex.isValid())
		{
			fr.m_regex = regex;
			bool const checked = (child->checkState() == Qt::Checked);
			if (child && checked)
			{
				child->setData(QBrush(Qt::green), Qt::BackgroundRole);
				child->setToolTip(tr("ok"));
				fr.m_is_enabled = true;
			}
			else if (child && !checked)
			{
				child->setData(QBrush(Qt::yellow), Qt::BackgroundRole);
				child->setToolTip(tr("regex not enabled"));
			}
		}
		else
		{
			if (child)
			{
				child->setData(QBrush(Qt::red), Qt::BackgroundRole);
				child->setToolTip(regex.errorString());
			}
		}
	}

	onInvalidateFilter();
}

void LogWidget::appendToStringWidgets (FilteredString const & flt)
{
	QStandardItem * root = filterWidget()->m_string_model->invisibleRootItem();
	QStandardItem * child = findChildByText(root, flt.m_string);
	if (child == 0)
	{
		bool const mode = static_cast<bool>(flt.m_state);
		QList<QStandardItem *> row_items = addTriRow(flt.m_string, flt.m_is_enabled ? Qt::Checked : Qt::Unchecked, mode);
		row_items[0]->setCheckState(flt.m_is_enabled ? Qt::Checked : Qt::Unchecked);
		root->appendRow(row_items);
	}
}
void LogWidget::appendToStringFilters (QString const & str, bool checked, int state)
{
	m_filter_state.appendToStringFilters(str, checked, state);
}

void LogWidget::removeFromStringFilters (QString const & val)
{
	m_filter_state.removeFromStringFilters(val);
}

void LogWidget::recompileStrings ()
{
	onInvalidateFilter();
}

void LogWidget::appendToColorRegexFilters (QString const & val)
{
	m_filter_state.appendToColorRegexFilters(val);
}

void LogWidget::removeFromColorRegexFilters (QString const & val)
{
	m_filter_state.removeFromColorRegexFilters(val);
}

void LogWidget::loadToColorRegexps (QString const & filter_item, QString const & color, bool enabled)
{
	m_filter_state.appendToColorRegexFilters(filter_item);
	m_filter_state.setRegexColor(filter_item, QColor(color));
	m_filter_state.setRegexChecked(filter_item, enabled);
}

void LogWidget::onColorRegexChanged ()
{
	for (int i = 0, ie = m_filter_state.m_colorized_texts.size(); i < ie; ++i)
	{
		ColorizedText & ct = m_filter_state.m_colorized_texts[i];
		QStandardItem * root = filterWidget()->m_color_regex_model->invisibleRootItem();
		QString const qregex = ct.m_regex_str;
		QStandardItem * child = findChildByText(root, qregex);
		QModelIndex const idx = filterWidget()->m_color_regex_model->indexFromItem(child);
		if (!child)
			continue;

		if (QtColorPicker * w = static_cast<QtColorPicker *>(filterWidget()->getWidgetColorRegex()->indexWidget(idx)))
		{
			ct.m_qcolor = w->currentColor();
		}
	}
	onInvalidateFilter();
}

void LogWidget::recompileColorRegexps ()
{
	for (int i = 0, ie = m_filter_state.m_colorized_texts.size(); i < ie; ++i)
	{
		ColorizedText & ct = m_filter_state.m_colorized_texts[i];
		QStandardItem * root = filterWidget()->m_color_regex_model->invisibleRootItem();
		QString const qregex = ct.m_regex_str;
		QStandardItem * child = findChildByText(root, qregex);
		QModelIndex const idx = filterWidget()->m_color_regex_model->indexFromItem(child);
		ct.m_is_enabled = false;
		if (!child)
			continue;

		if (filterWidget()->getWidgetColorRegex()->indexWidget(idx) == 0)
		{
			QtColorPicker * w = new QtColorPicker(filterWidget()->getWidgetColorRegex(), qregex);
			w->setStandardColors();
			w->setCurrentColor(ct.m_qcolor);

			connect(w, SIGNAL(colorChanged(const QColor &)), this, SLOT(onColorRegexChanged()));
			filterWidget()->getWidgetColorRegex()->setIndexWidget(idx, w);
		}
		else
		{
			QtColorPicker * w = static_cast<QtColorPicker *>(filterWidget()->getWidgetColorRegex()->indexWidget(idx));
			w->setCurrentColor(ct.m_qcolor);
		}

		QRegExp regex(qregex);
		if (regex.isValid())
		{
			ct.m_regex = regex;

			bool const checked = (child->checkState() == Qt::Checked);
			if (child && checked)
			{
				child->setData(QBrush(Qt::green), Qt::BackgroundRole);
				child->setToolTip(tr("ok"));
				ct.m_is_enabled = true;
			}
			else if (child && !checked)
			{
				child->setData(QBrush(Qt::yellow), Qt::BackgroundRole);
				child->setToolTip(tr("not checked"));
			}
		}
		else
		{
			if (child)
			{
				child->setData(QBrush(Qt::red), Qt::BackgroundRole);
				child->setToolTip(regex.errorString());
			}
		}
	}

	onInvalidateFilter();
}

void LogWidget::loadToRegexps (QString const & filter_item, bool inclusive, bool enabled)
{
	m_filter_state.appendToRegexFilters(filter_item, inclusive, enabled);
}

void LogWidget::onFilterFileComboChanged (QString str)
{
	if (str.isEmpty())
	{
		filterWidget()->getWidgetFile()->setModel(filterWidget()->m_file_model);
	}
	else
	{
		if (filterWidget()->getWidgetFile()->model() != filterWidget()->m_file_proxy)
		{
			filterWidget()->getWidgetFile()->setModel(filterWidget()->m_file_proxy);
			filterWidget()->m_file_proxy->setSourceModel(filterWidget()->m_file_model);
		}
		filterWidget()->m_file_proxy->setFindString(str);
	}
}

void LogWidget::onCancelFilterFileButton ()
{
}

void LogWidget::onCutParentValueChanged (int i)
{
	filterWidget()->fileModel()->onCutParentValueChanged(i);
	filterWidget()->getWidgetFile()->hideLinearParents();
}
void LogWidget::onCollapseChilds ()
{
	filterWidget()->fileModel()->collapseChilds(filterWidget()->getWidgetFile());
}

}

